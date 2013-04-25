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

#include <stdlib.h>
#include <sys/dev/pci.h>
#include <networking/rtl8168.h>
#include <networking/net.h>
#include <andromeda/drivers.h>
#include <andromeda/system.h>
#include <fs/vfs.h>
#include <arch/x86/irq.h>

/**
 * \file rtl8168.h
 * \brief RealTek 8168 NIC driver.
 */

static size_t rtl_rx_vfio(struct vfile *file, char *buf, size_t size);
static size_t rtl_tx_vfio(struct vfile *file, char *buf, size_t size);
static void sent_command_registers(struct rtlcommand *cmd, uint16_t port);
static int read_command_registers(struct rtlcommand *cmd, uint16_t port);
static void add_rtl_device(struct rtl_cfg *cfg);
static int reset_rtl_device(struct rtl_cfg *cfg);
static int rtl_generic_cfg_out(uint16_t port, void* data, uint8_t size);
static int rtl_generic_cfg_in(uint16_t port, void* store, uint8_t size);
static int rtl_conf_rx(struct rtl_cfg *cfg);

static struct rtl_cfg *rtl_devs = NULL;

void
rtl8168_irq_handler(unsigned int irq, irq_stack_t stack)
{
        struct netdev *dev = (struct netdev*) get_irq_data(irq)->irq_data;
        struct device *rtl = get_net_driver(dev->dev_id);

        struct vfile *io = rtl->open(rtl);
        struct net_buff *buff = alloc_buff_frame(496);
        buff->dev = dev;
        struct rtl_cfg *cfg = dev->device_data;
        unsigned short irq_state = 0;
        rtl_generic_cfg_in(cfg->portbase+RTL_IRQ_STATUS_PORT_OFFSET,
                                   &irq_state, sizeof(irq_state));
#ifdef RTL_DBG
        printf("IRQ status: %x\n", irq_state);
#endif
        if (io->read(io, (void*) buff, sizeof (*buff)) != -E_SUCCESS)
                warning("failure to call rtl io reader\n");

        return;
}

static int
rtl_setup_irq_handle(irq_handler_t handle, struct netdev *irq_data)
{
        struct irq_data *data = alloc_irq();
        data->base_handle = &do_irq;
        data->handle = handle;
        data->irq_data = irq_data;
        int ret = native_setup_irq_handler(data->irq);
        if (!ret)
                install_irq_vector(data);
        else
                warning("network card handler could not be installed!");
}

static void
get_mac(struct pci_dev *dev, struct netdev *netdev)
{
        uint8_t mac[MAC_ADDR_SIZE];
        uint16_t base = get_rtl_port_base(dev, 0);
        int i = 0;
        for (; i < 6; i++)
        {
                mac[i] = inb(base + i);
        }
        memcpy(netdev->hwaddr, mac, MAC_ADDR_SIZE);
        printf("MAC address: ");

        for (i = 0; i < 5; i++)
                printf("%x:", netdev->hwaddr[i]);

        printf("%x\n", netdev->hwaddr[5]);
}

void
init_rtl_device(struct pci_dev *dev)
{
        struct rtlcommand *cmd = kmalloc(sizeof (*cmd));
        struct rtl_cfg *cfg = kmalloc(sizeof (*cfg));
        cfg->next = NULL;

        int i = 0;
        uint16_t portbase;
        do
        {
                portbase = get_rtl_port_base(dev, i * 4);
                i++;
        }
        while (portbase == 0 && i <= 5);
        debug("RealTek base: %x\n", portbase);
        cfg->portbase = portbase;

        cfg->raw_rx_buff = kmalloc(RX_BUFFER_SIZE);
        cfg->rx_buff_length = RX_BUFFER_SIZE;
        cfg->raw_tx_buff = kmalloc(TX_BUFFER_SIZE);
        cfg->tx_buff_length = TX_BUFFER_SIZE;

        if (cmd == NULL)
                return;

        if (rtl_devs == NULL)
                rtl_devs = cfg;
        else
                add_rtl_device(cfg);

        reset_rtl_device(cfg);
        cmd->ccommand.rxvlan = 1;
        cmd->ccommand.rxchecksum = 1;
        cmd->tx_enable = 0;
        cmd->rx_enable = 0;
        cmd->reset = 0;
        cfg->command = cmd;

        sent_command_registers(cmd, portbase);
        read_command_registers(cmd, portbase);
        rtl_conf_rx(cfg);
        init_core_driver(dev, cfg);


        debug("Tx Enable flag: %x - RxChecksum: %x\n", cmd->tx_enable,
              cmd->ccommand.rxchecksum);
}

enum ptype
rtl_rx_pull_dev(struct net_buff *buff)
{
        return P_DONE;
}

int
init_core_driver(pci_dev_t pci, struct rtl_cfg *cfg)
{
        struct device *dev = kmalloc(sizeof (*dev));
        if (dev == NULL)
                panic("No memory in init_core_driver!");
        dev->dev_id = device_id_alloc(dev);
        dev_setup_driver(dev, rtl_rx_vfio, rtl_tx_vfio);
        cfg->device_id = dev->dev_id;
        struct netdev *netdev = kmalloc(sizeof (*netdev));
        netdev->dev = pci;
        netdev->rx_pull_handle = &rtl_rx_pull_dev;
        netdev->dev_id = dev->dev_id;
        netdev->frame_type = ETHERNET;
        netdev->poll_support = TRUE;
        netdev->device_data = cfg;
        get_mac(pci, netdev);
        rtl_setup_irq_handle(&rtl8168_irq_handler, netdev);
        register_net_dev(dev, netdev);
}

static enum packet_state
rtl_poll_data(struct net_buff *nb)
{
        struct rtl_cfg *cfg = nb->dev->device_data;
        uint16_t irq_state;

        rtl_generic_cfg_in(cfg->portbase+RTL_IRQ_STATUS_PORT_OFFSET, &irq_state,
                           sizeof(irq_state));
        if(irq_state & 1)
        {
                irq_state = 1;
                rtl_generic_cfg_out(cfg->portbase+RTL_IRQ_STATUS_PORT_OFFSET,
                                    &irq_state, sizeof(irq_state));
                /* insert short delay here */
                rtl_generic_cfg_in(cfg->portbase+RTL_IRQ_STATUS_PORT_OFFSET,
                                   &irq_state, sizeof(irq_state));
                if(irq_state & 1)
                        return P_ANOTHER_ROUND;
                else
                        return P_DELIVERED;
        }

        return P_NOTCOMPATIBLE;
}

static void
sent_command_registers(struct rtlcommand *cmd, uint16_t port)
{
        /* first of all we have to sent the C+ command register */
        uint16_t ccommand = (cmd->ccommand.rxvlan << 6) |
                (cmd->ccommand.rxchecksum << 5);

        outw(port + CPLUS_COMMAND_PORT_OFFSET, ccommand);

        /*
         * then the normal command register has to be sent to the device. when that is
         * done, we can continue configuring other registers.
         */
        uint8_t command = (cmd->tx_enable << 2) | (cmd->rx_enable << 3) |
                (cmd->reset << 4);
        outb(port + COMMAND_PORT_OFFSET, command);
}

static int
read_command_registers(struct rtlcommand *cmd, uint16_t port)
{
        uint16_t ccommand = inw(port + CPLUS_COMMAND_PORT_OFFSET);
        uint8_t command = inb(port + COMMAND_PORT_OFFSET);

        cmd->ccommand.rxvlan = (ccommand >> 6) & 1;
        cmd->ccommand.rxchecksum = (ccommand >> 5) & 1;

        cmd->tx_enable = (command >> 2) & 1;
        cmd->rx_enable = (command >> 3) & 1;
        cmd->reset = (command >> 4) & 1;

        return 0;
}

static void
add_rtl_device(struct rtl_cfg *cfg)
{
        struct rtl_cfg *carriage = cfg;
        for (; carriage->next != NULL, carriage->next = carriage;
             carriage = carriage->next)
        {
                if (carriage->next == NULL)
                {
                        carriage->next = cfg;
                        cfg->next = NULL;
                        break;
                }
        }
}

/**
 * \fn net_rx_vfio(vfile, buf, size)
 *
 * Receive a buffer from the device driver.
 */
static size_t
rtl_rx_vfio(struct vfile *file, char *buf, size_t size)
{

        struct device *dev = dev_find_devtype(dev_find_devtype(get_root_device(),
                                                               virtual_bus), net_core_dev);
        struct net_buff *buff = (struct net_buff*)buf;
        buff->raw_vlan = RAW_VLAN;
        if (dev == NULL)
                return -E_NULL_PTR;
        struct vfile *io = dev->open(dev);
        if (io == NULL)
                return -E_NULL_PTR;
        if (buf == NULL)
                return -E_NULL_PTR;
        io->read(io, (void*) buf, size);
        return -E_SUCCESS;
}

/**
 * \fn net_tx_vfio(vfile, buf, size)
 *
 * Transmit a buffer using virtual files.
 */
static size_t
rtl_tx_vfio(struct vfile *file, char *buf, size_t size)
{
        return -E_NOFUNCTION;
}

static int
reset_rtl_device(struct rtl_cfg *cfg)
{
        cfg->command->reset = 1;
        sent_command_registers(cfg->command, cfg->portbase);

        int i = 0;
        for (; i < 0x100000; i++)
        {
                read_command_registers(cfg->command, cfg->portbase);
                if (cfg->command->reset == 0)
                        return 0;
                else
                        continue;
        }
        debug("RTL8168 failed");
        return -1;
}

static struct rtl_cfg*
get_rtl_dev_list()
{
        return rtl_devs;
}

static int
get_rtl_dev_num()
{
        int i = 0;
        struct rtl_cfg *carriage;
        for (carriage = get_rtl_dev_list(); carriage != NULL, carriage != carriage->next;
             carriage = carriage->next)
        {
                i++;
                if (carriage->next == NULL)
                        break;
                else
                        continue;
        }
        return i;
}

/**
 * \fn get_rtl_device(dev)
 * \brief Get a device based on the device number in the list.
 *
 * @param dev Index in the device list.
 */
static struct rtl_cfg*
get_rtl_device(int dev)
{
        struct rtl_cfg *carriage = get_rtl_dev_list();
        int i = 0;
        for (; carriage != NULL, carriage != carriage->next; carriage = carriage->next)
        {
                if (i == dev)
                        break;
                if (carriage->next == NULL)
                        break;
                else
                        continue;
        }
}

/**
 * \fn rtl_generic_cfg_out(port, data, size)
 * \brief Send the data specified in <i>data</i> to the output port <i>port</i>.
 *
 * @param port The output port.
 * @param data Data to send.
 * @param size Size of the data to send (size <= 4)
 * @return Error code. Zero for success.
 */
static int
rtl_generic_cfg_out(port, data, size)
uint16_t port;
void *data;
uint8_t size;
{
        int retval = -E_SUCCESS;
        switch(size)
        {
                case 1:
                        outb(port, *((uint8_t*)data));
                        break;
                case 2:
                        outw(port, *((uint16_t*)data));
                        break;
                case 4:
                        outl(port, *((uint32_t*)data));
                        break;
                default:
                        retval = -E_GENERIC;
                        break;

        }
        return retval;
}

/**
 * \fn rtl_generic_cfg_in(port, store, size)
 * \brief Reads config info from port <i>port</i> and stores it in <i>store</i>.
 *
 * @param port The I/O port address.
 * @param store Memory space storage address.
 * @param size Size of the read.
 * @return Error code.
 */
static int
rtl_generic_cfg_in(port, store, size)
uint16_t port;
void *store;
uint8_t size;
{
        int retval = -E_SUCCESS;
        switch(size)
        {
                case 1:
                        writeb(store, inb(port));
                        break;
                case 2:
                        writew(store, inw(port));
                        break;
                case 4:
                        writel(store, inl(port));
                        break;
                default:
                        retval = -E_GENERIC;
                        break;
        }
        return retval;
}

static int
rtl_conf_rx(struct rtl_cfg *cfg)
{
        uint32_t raw = 0;
        struct rxconfig *rxc = kmalloc(sizeof(*rxc));
        if(!rxc)
                return -E_NOMEM;
        cfg->receive = rxc;

        /*
         * only accept packets which are conform to RFC's.
         */
        rxc->dest_addr_accept = 1;
        rxc->phys_match_accept = 1;
        rxc->multi_cast_accept = 1;
        rxc->broadcast_accept = 1;
        rxc->runt_accept = 0;
        rxc->error_accept = 0;

        rxc->eeprom = 1;
        rxc->dma_burst = 0b010;
        rxc->threshold = 0b111;

        raw = (rxc->dest_addr_accept) | (rxc->phys_match_accept << 1)
                | (rxc->multi_cast_accept << 2) | (rxc->broadcast_accept << 3)
                | (rxc->runt_accept << 4) | (rxc->error_accept << 5)
                | (rxc->eeprom << 6)
                | (rxc->dma_burst << 8) | (rxc->threshold << 13);
        outl(cfg->portbase+RTL_RX_CONFIG_PORT_OFFSET, raw);
#ifdef X86
        outl(cfg->portbase+RTL_RX_DESC_PORT_OFFSET, (uint32_t)cfg->raw_rx_buff);
#endif
        return -E_SUCCESS;
}

void
init_network()
{
        struct ol_pci_node *carriage = pcidevs;
        for (; carriage != NULL, carriage->next != carriage; carriage = carriage->next)
        {
                if (carriage->dev->class == NIC && carriage->dev->subclass == NIC_ETHERNET)
                {
                        init_rtl_device(carriage->dev);
                        return;
                }
                else
                        continue;
        }
        debug("no network card found");
}
