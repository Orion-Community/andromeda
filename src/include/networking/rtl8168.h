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
#include <networking/net.h>
#include <stdlib.h>
#include <arch/x86/irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/* device identification numbers */
#define DEVICE_ID 0x8129
#define VENDOR_ID 0x10ec

#define RAW_VLAN (0x81 << 16) | (0x2 << 13) | 0x20

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

/**
 * \struct struct rtl_cfg
 * \brief RealTek configuration data structure.
 */
struct rtl_cfg
{
  /**
   * \var transmit
   * \brief Transmit configuration
   *
   * \var receive
   * \brief Receive configuration register.
   *
   * \var command
   * \brief Command register. Also holds the CPLUS command register.
   *
   * \var next
   * \brief A pointer to another configuration.
   */
  struct txconfig *transmit;
  struct rxconfig *receive;
  struct rtlcommand *command;
  struct rtl_cfg *next;

  /**
   * \var portbase
   * \brief I/O base port
   *
   * \var device_id
   * \brief The device ID in the andromeda driver list.
   *
   * \var raw_rx_buff
   * \brief Memory space to receive data in.
   *
   * \var raw_tx_buff
   * \brief Memory space buffer to transmit data.
   */
  uint16_t portbase;
  uint64_t device_id;
  void *raw_rx_buff;
  void *raw_tx_buff;

  /**
   * \var rx_buff_length
   * \brief Allocated memory length of raw_rx_buff.
   * \see raw_rx_buff
   *
   * \var tx_buff_length
   * \brief Allocated memory length of raw_tx_buff.
   * \see raw_tx_buff
   */
  uint16_t rx_buff_length,
           tx_buff_length;
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
  struct vlan_tag vlan;
  uint tava : 1; /* tag available flag */
  uint32_t rxbufl; /* lower buffer address */
  uint32_t rxbuffh; /* higher buffer address */
};

/**
 * \fn rtl_conf_b(data,portbase,offset)
 * \brief Generic function to sent data to the rtl device.
 *
 * @param data Data to sent to the device.
 * @param portbase Portbase of the NIC.
 * @param offset Port register offset
 */
static inline void
rtl_conf_b(uint8_t data, uint16_t portbase, uint16_t offset)
{
  outb(portbase+offset, data);
}

static inline struct rtl_cfg*
rtl_cfg(struct netdev *dev)
{
  return (struct rtl_cfg*)dev->device_data;
}

/**
 * \fn rtl_conf_w(data,portbase,offset)
 * \brief Generic function to sent data to the rtl device.
 *
 * @param data Data to sent to the device.
 * @param portbase Portbase of the NIC.
 * @param offset Port register offset
 */
static inline void
rtl_conf_w(uint16_t data, uint16_t portbase, uint16_t offset)
{
  outw(portbase+offset, data);
}

/**
 * \fn rtl_conf_l(data,portbase,offset)
 * \brief Generic function to sent data to the rtl device.
 *
 * @param data Data to sent to the device.
 * @param portbase Portbase of the NIC.
 * @param offset Port register offset
 */
static inline void
rtl_conf_l(uint32_t data, uint16_t portbase, uint16_t offset)
{
  outl(portbase+offset, data);
}

void init_rtl_device(struct pci_dev *);
void init_network();

void rtl8168_irq_handler(unsigned int irq, irq_stack_t stack);
static int rtl_setup_irq_handle(irq_handler_t handle, struct netdev *irq_data);
static void get_mac(struct pci_dev *dev, struct netdev *netdev);
static void sent_command_registers(struct rtlcommand *, uint16_t);
static int read_command_registers(struct rtlcommand *, uint16_t);
static void add_rtl_device(struct rtl_cfg *cfg);
static int reset_rtl_device(struct rtl_cfg *cfg);
static int init_core_driver(pci_dev_t pci, struct rtl_cfg *cfg);
static struct rtl_cfg* get_rtl_dev_list();
static struct rtl_cfg* get_rtl_device(int dev);
static int get_rtl_dev_num();

/**
 * \fn net_rx_vfio(vfile, buf, size)
 *
 * Receive a buffer from the device driver.
 */
static size_t rtl_rx_vfio(struct vfile *file, char *buf, size_t size);

/**
 * \fn net_tx_vfio(vfile, buf, size)
 *
 * Transmit a buffer using virtual files.
 */
static size_t rtl_tx_vfio(struct vfile *file, char *buf, size_t size);

static inline uint16_t get_rtl_port_base(struct pci_dev *dev, uint8_t offset)
{
  uint16_t addr = (uint16_t)(ol_pci_read_dword(dev, 0x10+offset));
  if((addr & 1) != 0)
    return addr&PCI_IO_SPACE_MASK;
  else
    return 0;
}

#ifdef __cplusplus
}
#endif
#endif