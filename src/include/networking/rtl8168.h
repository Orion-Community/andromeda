/*
 *   OpenLoader - RealTek network card driver
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

#ifndef __RTL8168
#define __RTL8168

#include <sys/dev/pci.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* device identification numbers */
#define DEVICE_ID 0x8129
#define VENDOR_ID 0x10ec

#define CPLUS_COMMAND_PORT_OFFSET 0xe0
#define COMMAND_PORT_OFFSET 0x37

struct txconfig
{
  uint dma_burst : 3;
  uint crc : 1;
  uint loopback : 2;
  uint interframe_gap2 : 1;
  uint interframe_gap : 2;
  uint vID1 : 1;
  uint vID0 : 2;
};

struct rxconfig
{
  uint dest_addr_accept : 1;
  uint phys_match_accept : 1;
  uint multi_cast_accept : 1;
  uint broadcast_accept : 1;
  uint runt_accept : 1;
  uint error_accept : 1;
  
  uint eeprom : 1;
  uint dma_burst : 3;
  uint threshold : 3;
};

struct rtlcommand
{
  struct rtlccommand
  {
    uint rxvlan : 1;
    uint rxchecksum : 1;
  } ccommand;
  uint tx_enable : 1;
  uint rx_enable : 1;
  uint reset : 1;
};

struct rtl_cfg
{
  struct txconfig *transmit;
  struct rxconfig *receive;
  struct rtlcommand *command;
  struct rtl_cfg *next;
};

void print_mac(struct ol_pci_dev *dev);
void init_rtl_device(struct ol_pci_dev *);
static void sent_command_registers(struct rtlcommand *, uint16_t);
static int read_command_registers(struct rtlcommand *, uint16_t);
static void add_rtl_device(struct rtl_cfg *cfg);

static inline get_rtl_port_base(struct ol_pci_dev *dev)
{
  return (uint16_t)(ol_pci_read_dword(dev, 0x10)&PCI_IO_SPACE_MASK);
}

#ifdef __cplusplus
}
#endif
#endif