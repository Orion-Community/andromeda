/*
 *   OpenLoader - PS/2 Controller
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <mm/heap.h>
#include <sys/dev/ps2.h>
#include <sys/io.h>
#include <andromeda/system.h>

#include <text.h>

static void ol_ps2_sent_controller_command(ol_ps2_dev_t ctrl, uint8_t cmd);

static ol_ps2_dev_t ol_ps2_controller = NULL;
static ol_ps2_dev_t ol_ps2_keyboard = NULL;
static ol_ps2_dev_t ol_ps2_mouse = NULL;

static void
ol_ps2_update_status(ol_ps2_dev_t dev)
{
  dev->status = inb(OL_PS2_COMMAND_PORT);
  return;
}

static int
ol_ps2_init_dev(ol_ps2_dev_t dev, ol_ps2_dev_type_t type)
{
  dev->type = type;
  dev->flags = OL_PS2_ACTIVE_FLAG;
  if ((type & (OL_PS2_MOUSE | OL_PS2_KEYBOARD)) != 0)
  {
    /* we are dealing with a mouse or keyboard here*/
    dev->read_port = OL_PS2_OUTPUT_BUFFER;
    dev->write_port = OL_PS2_DATA_PORT;
    dev->sent_command = &ol_ps2_sent_controller_command;
  }
  else if ((type & OL_PS2_CONTROLLER) != 0)
  {
    /* this device should be configured as a PS/2 controller. */
    dev->write_port = OL_PS2_COMMAND_PORT;
    dev->read_port = OL_PS2_DATA_PORT;
  }
  else
    return -1;

  ol_ps2_update_status(dev);
  return 0;
}

static uint8_t
ol_ps2_await_ack(ol_ps2_dev_t dev)
{
  uint32_t i;
  for (i = 0; i < 0x80000; i++)
  {
    ol_ps2_update_status(dev);
    if ((dev->status & 0x1) != 0)
      goto read;
  }

  return 0xff; // return an error

  read: return inb(dev->read_port);
}

static void
ol_ps2_sent_controller_command(ol_ps2_dev_t ctrl, uint8_t cmd)
{
  while ((ctrl->status & 0x2) != 0)
    ol_ps2_update_status(ctrl);
  outb(ctrl->write_port, cmd);
}

static uint8_t
ol_ps2_read(ol_ps2_dev_t dev)
{
  while ((dev->status & 0x1) == 0)
    ol_ps2_update_status(dev);
  return inb(dev->read_port);
}

static int
ol_ps2_write(ol_ps2_dev_t dev, uint8_t val, bool ack)
{
  uint32_t i, readback = 0;
  for (i = 0; i < 0x80000; i++)
  {
    ol_ps2_update_status(dev);
    if ((dev->status & 0x2) == 0)
      goto write;
  }
  return -1;

  write: outb(dev->write_port, val);
  for (i = 0; i < 10; i++)
  {
    readback = ol_ps2_await_ack(dev);
    if (readback == 0xfa)
      return 0; /* got the ack byte, bail out */
  }
  return -1; /* sorry, but we didn't get an ack byte.. */
}

int
ol_ps2_init_keyboard()
{
  ol_ps2_keyboard = kmalloc(sizeof(struct ol_ps2_dev));
  ol_ps2_controller = kmalloc(sizeof(struct ol_ps2_dev));
  if ((!ol_ps2_init_dev(ol_ps2_keyboard, OL_PS2_KEYBOARD))
      && (!ol_ps2_init_dev(ol_ps2_controller, OL_PS2_CONTROLLER)))
  {
    ol_ps2_keyboard->sent_command(ol_ps2_controller, OL_PS2_INIT_KB_CMD);
  }
  return 0;
}

void
ol_detach_all_devices()
{
  if ((ol_ps2_keyboard->flags & OL_PS2_ACTIVE_FLAG) != 0
      && (ol_ps2_keyboard != NULL))
  {
    kfree(ol_ps2_keyboard);
  }
  if ((ol_ps2_mouse->flags & OL_PS2_ACTIVE_FLAG) != 0 && (ol_ps2_mouse != NULL))
  {
    kfree(ol_ps2_mouse);
  }
  if ((ol_ps2_controller->flags & OL_PS2_ACTIVE_FLAG) != 0
      && (ol_ps2_controller != NULL))
  {
    kfree(ol_ps2_controller);
  }
}

uint8_t
ol_ps2_get_keyboard_scancode()
{
  return ol_ps2_read(ol_ps2_keyboard);
}

int
ol_ps2_config_keyboard(uint8_t confval)
{
  return ol_ps2_write(ol_ps2_keyboard, confval, TRUE);
}
