/*
    Andromeda
    Copyright (C) 2014  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESbuffer_initS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <andromeda/system.h>
#include <andromeda/drivers.h>
#include <drivers/serial.h>
#ifdef X86
#include <arch/x86/bios.h>
#endif
#include <stdio.h>

#define SERIAL_PORT_UNINITIALISED       0
#define SERIAL_PORT_READY               1

#define SERIAL_INTERRUPT_OFF            (uint8_t)0
#define SERIAL_INTERRUPT_ON             (uint8_t)1

// <Status line>
// Bits 0 and 1
#define SERIAL_CHAR_LEN_5               0x0
#define SERIAL_CHAR_LEN_6               0x1
#define SERIAL_CHAR_LEN_7               0x2
#define SERIAL_CHAR_LEN_8               0x3

// Bit 2
#define SERIAL_STOP_SHORT               (0 << 2)
#define SERIAL_STOP_LONG                (1 << 2)

// Bits 3, 4, 5 and 6
#define SERIAL_PARITY_NONE              (0x0 << 3)
#define SERIAL_PARITY_ODD               (0x1 << 3)
#define SERIAL_PARITY_EVEN              (0x3 << 3)
#define SERIAL_PARITY_MARK              (0x5 << 3)
#define SERIAL_PARITY_SPACE             (0x7 << 3)

// Bit 7
#define SERIAL_DLAB                     (uint8_t)(1 << 7)
// </status line>

#define SERIAL_MODEM_DTR                (1 << 0)
#define SERIAL_MODEM_TSR                (1 << 1)
#define SERIAL_MODEM_OUT1               (1 << 2)
#define SERIAL_MODEM_OUT2               (1 << 3)
#define SERIAL_MODEM_LOOP               (1 << 4)

#define SERIAL_DATA_REG                 0
#define SERIAL_BOUD_LSB                 0
#define SERIAL_INTERRUPT                1
#define SERIAL_BOUD_MSB                 1

#define SERIAL_FIFO_REG                 2
#define SERIAL_IRQ_CONTROL              2

#define SERIAL_LINE_CONTROL             3
#define SERIAL_MODEM_CONTROL            4
#define SERIAL_LINE_STATUS              5
#define SERIAL_MODEM_STATUS             6
#define SERIAL_SCRATCH_REG              7

struct serial_port_data {
        uint16_t io_port;
        uint16_t port_status;
        uint8_t baud;
        uint8_t char_len;
        uint8_t stop_char;
        uint8_t parity;
};

static int drv_serial_suspend(struct device* dev __attribute__((unused)))
{
        return -E_UNFINISHED;
}

static int drv_serial_resume(struct device* dev __attribute__((unused)))
{
        return -E_UNFINISHED;
}

static int drv_serial_setup(struct device* dev)
{
        if (dev == NULL)
                return -E_NULL_PTR;

        struct serial_port_data* data = dev->device_data;
        if (data == NULL || dev->device_data_size != sizeof(*data))
                return -E_NULL_PTR;

        uint16_t port = data->io_port;

        uint16_t baud = data->baud;
        uint8_t baud_low = baud & 0xFF;
        uint8_t baud_high = (baud >> 8 ) & 0xFF;

        // Select interrupt register
        outb(port+SERIAL_LINE_CONTROL, 0);
        // Disable serial interrupts
        outb(port+SERIAL_INTERRUPT, SERIAL_INTERRUPT_OFF);
        // Select baud control
        outb(port+SERIAL_LINE_CONTROL, SERIAL_DLAB);
        // Set baud rate
        outb(port+SERIAL_BOUD_LSB, baud_low);
        outb(port+SERIAL_BOUD_MSB, baud_high);
        // Disable baud control
        // Set up line control
        uint8_t line_control = data->char_len;
        line_control |= data->parity;
        line_control |= data->stop_char;
        outb(port+SERIAL_LINE_CONTROL, line_control);

        return -E_SUCCESS;
}

int drv_serial_init(struct device* parent, uint16_t io_port)
{
        struct device* serial = kmalloc(sizeof(*serial));
        if (serial == NULL)
                panic("Unable to set up a serial device descriptor");

        memset(serial, 0, sizeof(*serial));

        serial->type = tty;
        serial->driver = kmalloc(sizeof(*serial->driver));
        if (serial->driver == NULL)
                panic("Unable to set up a driver descriptor for serial");

        memset (serial->driver, 0, sizeof(*serial->driver));

        struct serial_port_data* data = kmalloc(sizeof(*data));
        if (data == NULL) {
                panic("Unable to allocate memory for serial port data");
        }
        data->io_port = io_port;

        serial->device_data_size = sizeof(*data);
        serial->device_data = data;

        serial->driver->attach = device_attach;
        serial->driver->detach = device_detach;
        serial->driver->suspend = drv_serial_suspend;
        serial->driver->resume = drv_serial_resume;
        serial->driver->find = device_find_id;

        parent->driver->attach(parent, serial);

        drv_serial_setup(serial);

        printf("Setting up serial: %X\n", io_port);
        return -E_SUCCESS;
}

int drv_serial_kickstart (struct device* parent)
{
        int ret = E_SUCCESS;
        int i = 0;
        uint16_t ports = 0;
#ifdef X86
        // Attempt to detect the ports through BIOS data
        uint16_t* com_port = &bda.COM0;
        for (; i < 4; i++) {
                if (com_port[i] != 0) {
                        ports ++;
                        printf("Found com: %X\n", i);
                        ret |= drv_serial_init(parent, com_port[i]);
                }

        }
        uint16_t stat_ports[] = {0x3f8, 0x2f8, 0x3e8, 0x2e8};
#endif
        // The ports were not found, fall back to static data
        if (ports == 0) {
                for (i = 0; i < 4; i++) {
                        ret |= drv_serial_init(parent, stat_ports[i]);
                }
        }

        return -ret;
}
