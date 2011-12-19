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
  
  uint16_t portbase;
};

/*
 * Below are the definitions of the receive and transmit command and status
 * structures
 */

/* receive structures first */
struct rxcommand
{
  uint bufsize : 14;
  uint eor : 1; /* end of Rx descriptor. When this bit is set, it indicates that
                   it is the last one in the descriptor ring */
  uint own : 1; /* When set, indicates that the descriptor is owned by the NIC, 
                   and is ready to receive a packet. The OWN bit is set by the 
                   driver after having pre-allocated the buffer at initialization, 
                   or the host has released the buffer to the driver.
                 */
  struct vlan_tag
  {
    uint vidh : 4;
    uint vidl : 8;
    uint prio : 3; /* priority flag */
    uint cfi : 1;
  } vlan;
  
  uint tava : 1; /* tag available flag */
  uint32_t rxbufl; /* lower buffer address */
  uint32_t rxbuffh; /* higher buffer address */ 
};

void init_rtl_device(struct ol_pci_dev *);
static void print_mac(struct ol_pci_dev *);
static void sent_command_registers(struct rtlcommand *, uint16_t);
static int read_command_registers(struct rtlcommand *, uint16_t);
static void add_rtl_device(struct rtl_cfg *cfg);
static int reset_rtl_device(struct rtl_cfg *cfg);

static inline get_rtl_port_base(struct ol_pci_dev *dev)
{
  return (uint16_t)(ol_pci_read_dword(dev, 0x10)&PCI_IO_SPACE_MASK);
}

#ifdef __cplusplus
}
#endif
#endif