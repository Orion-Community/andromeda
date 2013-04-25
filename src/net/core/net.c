/*
 *   Andromeda Project - Network stack - protocol independent - core driver.
 *   Copyright (C) 2011  Michel Megens - dev@michelmegens.net
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

/**
 * \file net.c
 * \brief Contains the implementation of the net core driver.
 */

#include <stdlib.h>

#include <arch/x86/timer.h>

#include <andromeda/drivers.h>
#include <andromeda/system.h>

#include <networking/net.h>
#include <networking/eth/eth.h>
#include <networking/netlayer.h>

#include <lib/byteorder.h>

static struct net_queue *net_core_queue;
static struct net_queue *net_tx_core_queue;
struct protocol ptype_tree;
static bool initialized = FALSE;

static size_t net_rx_vfio(struct vfile *file, char *buf, size_t size);
static size_t net_tx_vfio(struct vfile *file, char *buf, size_t size);
static struct net_queue* remove_queue_entry(struct net_queue*, struct net_queue*);
static void init_ptype_tree();
static int check_net_buff_tstamp(struct net_buff *buff);
static int vlan_untag(struct net_buff *buff);

/**
 * \fn net_get_queue
 * \brief Returns the current net_queue head.
 * @return The current net_queue head.
 */
static inline struct net_queue *
net_get_queue()
{
        return net_core_queue;
}

/**
 * \fn net_set_queue(head)
 * \brief Sets a new new net_queue head.
 * \param new_head The new queue head.
 */
static inline void
net_set_queue(struct net_queue *new_head)
{
        net_core_queue = new_head;
}

/**
 * \fn init_netif()
 * \brief Initialize the net core driver.
 * @return Error code.
 */
int
init_netif()
{
        struct device *dev = kmalloc(sizeof (*dev));
        device_id_alloc(dev);
        dev_setup_driver(dev, net_rx_vfio, net_tx_vfio);
        dev->type = net_core_dev;
        dev->open = &device_open_driver_io;

        /*
         * attach the device driver to the the virtual bus.
         */
        struct device *virtual = dev_find_devtype(get_root_device(), virtual_bus);
        device_attach(virtual, dev);
        /*
         * initialize the queue
         */
        struct net_queue *head = kmalloc(sizeof (*head));
        memset(head, 0, sizeof (*head));
        net_set_queue(head);
        init_ptype_tree();
        init_eth();
        netif_netlayer_init();
        initialized = TRUE;
}

int
register_net_dev(struct device *dev, struct netdev* netdev)
{

        dev->type = net_dev;
        dev->open = &device_open_driver_io;
        dev->driver->io->fs_data = (void*) netdev;
        dev->driver->io->fs_data_size = sizeof (*netdev);

        /*
         * attach the device driver to the core driver.
         */
        struct device *core_dev = dev_find_devtype(dev_find_devtype(get_root_device(),
                                                                    virtual_bus), net_core_dev);
        device_attach(core_dev, dev);
        return -E_SUCCESS;
}

int
unregister_net_dev(uint64_t id)
{
        struct device *dev = dev_find_devtype(dev_find_devtype(get_root_device(),
                                                               virtual_bus), net_core_dev);
        if (dev == NULL)
                return -E_NULL_PTR;
        if (device_detach(dev, device_find_id(dev, id)) == -E_NOTFOUND)
                return -E_NOTFOUND;
        else
        {
                kfree(dev->driver->io->fs_data); // free the netdev data
                kfree(dev->driver->io);
                kfree(dev->driver);
                kfree(dev);
                return -E_SUCCESS;
        }
}

/**
 * \fn net_buff_append_list(head, x)
 * \brief The net_buff item x will be appended to the net_buff head <i>head</i>.
 * \param head The list head.
 * \param x Item to be appended.
 * \return Error code.
 */
static int
net_buff_append_list(struct net_buff *head, struct net_buff *x)
{
        if (head == NULL || x == NULL)
                return -E_NULL_PTR;
        else
        {
                struct net_buff *carriage, *tmp;
                for_each_net_buff_entry_safe(head, carriage, tmp)
                {
                        if (carriage->next == NULL)
                        {
                                carriage->next = x;
                                x->previous = x;
                                x->next = NULL;
                                return -E_SUCCESS;
                        }
                        else
                                continue;
                }
        }
}

struct net_buff *
alloc_buff_frame(unsigned int frame_len)
{
        struct net_buff *buff = kmalloc(sizeof (*buff));
        buff->length = frame_len;
        buff->head = kmalloc(frame_len);
        buff->data = buff->head;
        buff->tail = buff->head;
        buff->end = buff->head + frame_len;

        return buff;
}

static int
free_net_buff_list(struct net_buff* nb)
{
        kfree(nb->transport_hdr);
        kfree(nb->network_hdr);
        kfree(nb->datalink_hdr);
        struct net_buff *nxt = nb->next;
        kfree(nb);

        if (nxt)
                free_net_buff_list(nxt);
        else
                return -E_SUCCESS;
}

void
net_buff_reserve(struct net_buff *nb, int size)
{
        nb->data += size;
        nb->tail += size;
}

void *
net_buff_inc_header(struct net_buff *buff, unsigned int len)
{
        buff->data -= len;
        buff->length += len;
        return buff->data;
}

/**
 * \fn netif_rx_process(struct net_buff*)
 * \brief Receives, processess and polls for incoming packets.
 *
 * @param nb The first packet to handle.
 * @return State of handled packet
 *
 * This function handles all packets which are fresh from the device driver or
 * the netif queue (see <i>netif_process_queue(head, load)</i>). After handling
 * a packet it will poll from the device driver for more incoming packets.
 */
static enum packet_state
netif_rx_process(nb)
struct net_buff *nb;
{
        struct protocol *prot, *tmp, *root = get_ptype_tree();
        struct netdev *dev;
        enum packet_state retval = P_DROPPED;
        auto enum packet_state handle_packet(struct net_buff*);

        if(check_net_buff_tstamp(nb))
        {
                netif_drop_net_buff(nb);
                retval = P_LOST;
                goto out;
        }

        dev = nb->dev;

        if(netpoll_dev(dev))
        {
                netif_drop_net_buff(nb);
                retval = P_NOTCOMPATIBLE;
                goto out;
        }

        /*
         * init header pointers
         */
        net_buff_reset_datalink_hdr(nb);
        net_buff_reset_net_hdr(nb);
        net_buff_reset_tail(nb);
        nb->type = dev->frame_type;

        next:
        do
        {
                if((retval = handle_packet(nb)) == P_QUEUED)
                {
                        for_each_ll_entry_safe(root, prot, tmp)
                        {
                                if(nb->type == prot->type)
                                {
                                        prot->notify();
                                        break;
                                }
                        }
                        break;
                }
        }
        while(retval == P_ANOTHER_ROUND);

        if(nb->dev->rx_pull_handle)
        {
                /*
                 * if a pull function is given, try to pull for more packets
                 */
                switch(retval = dev->rx_pull_handle(nb))
                {

                        case P_ANOTHER_ROUND:
                                goto next;
                                break;
                        case P_DELIVERED:
                                break;
                        case P_DROPPED:
                                goto out;
                                break;
                        case P_DONE:
                                break;
                        case P_LOST:
                                goto out;
                                break;
                        case P_QUEUED:
                                retval = P_DONE;
                                goto out;
                                break;
                        default:
                                warning("Packet handled incorrectly");
                                break;
                }
        }

        if(retval == P_DELIVERED)
        {
                do
                        retval = handle_packet(nb);
                while(retval == P_ANOTHER_ROUND);
                if(retval == P_QUEUED)
                {
                        for_each_ll_entry_safe(root, prot, tmp)
                        {
                                if(prot->type == nb->type)
                                {
                                        prot->notify();
                                        break;
                                }
                        }
                }
                return retval;
        }

        out:
        if(retval != P_DONE)
        {
                free_net_buff_list(nb);
                return retval;
        }
        if(retval == P_DONE)
                return retval;


        /*
         * Handles one packet and returns the result of the packet handler. If
         * a certain packet type is not known within andromeda, the return type
         * will be enum packet_state.P_NOTCOMPATIBLE.
         */
        auto enum packet_state
        handle_packet(struct net_buff *buff)
        {
                protocol_deliver_handler_t handle;
                for_each_ll_entry_safe(root, prot, tmp)
                {
                        if(buff->type == prot->type)
                        {
                                handle = prot->deliver_packet;
                                atomic_inc(&buff->users);
                                if(buff->type == ETHERNET &&
                                        buff->raw_vlan != 0)
                                        vlan_untag(buff);
                                if(!buff)
                                        return P_NOTCOMPATIBLE;
                                break;
                        }
                }

                if(handle)
                        return handle(buff);
                else
                        return P_NOTCOMPATIBLE;
        }
}

/**
 * \fn netif_process_queue(head, load)
 * \brief Processes <i>load</i> amount of packets from the queue. The packets
 * taken from the queue are passed to <i>netif_process_net_buff(buff)</i>.
 * \param head The head of the receive queue.
 * \param load The amount of packets to process
 */
static int
netif_process_queue(struct net_queue *head, unsigned int load)
{
        struct net_queue *carriage, *tmp;
        unsigned int i = 0;
        int retval;
        auto struct net_buff *get_entry(struct net_queue*);

        for_each_ll_entry_safe_count(head, carriage, tmp, i)
        {
                struct net_buff *buff = get_entry(carriage);
                if(buff == NULL)
                        continue;

                netif_rx_process(buff);
                if(i >= load)
                        break;
        }

        return retval;

        auto struct net_buff *get_entry(struct net_queue *entry)
        {
                if(remove_queue_entry(head, entry) == NULL)
                        return NULL;
                struct net_buff *ret = entry->packet;
                kfree(entry);
                return ret;
        }
}

static int
netif_start_tx(struct net_buff *buff)
{
        if(buff->next)
        {
                /* just one buffer */
                struct device *dev = get_net_core_driver();
                struct vfile *io = dev->open(dev);
                io->write(io, (void*)buff, sizeof(*buff));
        }
        else
        {
                /* sent buffer list one by one */
                struct net_buff *carriage, *tmp;
                for_each_ll_entry_safe(buff, carriage, tmp)
                {
                        struct device *dev = get_net_core_driver();
                        struct vfile *io = dev->open(dev);
                        io->write(io, (void*)buff, sizeof(*buff));
                }
        }
}

/**
 * \fn netif_drop_net_buff
 * \brief Drop a packet.
 * \param buff Packet(s) to drop
 */
void
netif_drop_net_buff(struct net_buff *buff)
{
        return;
}

/**
 * \fn net_rx_vfio(vfile, buf, size)
 *
 * Receive a buffer from the device driver.
 */
static size_t
net_rx_vfio(struct vfile *file, char *buf, size_t size)
{
        struct net_buff *buffer = (struct net_buff*) buf;
        netif_rx_process(buffer);
        debug("Packet arrived in the core driver successfully. Protocol type: %x\n",
                buffer->vlan->protocol_tag
        );
        //print_mac(buffer->dev);
        return -E_SUCCESS;
}

/**
 * \fn net_tx_vfio(vfile, buf, size)
 *
 * Transmit a buffer using virtual files.
 */
static size_t
net_tx_vfio(struct vfile *file, char *buf, size_t size)
{
        struct device *core_dev = dev_find_devtype(dev_find_devtype(get_root_device(),
                                                                    virtual_bus), net_core_dev);
        struct device *dev_driver = device_find_id(core_dev, ((struct netdev*) buf)->dev_id);
        struct vfile *io = dev_driver->open(dev_driver);
        io->write(file, buf, size);
        return -E_NOFUNCTION;
}

struct device *
get_net_driver(uint64_t id)
{
        struct device *dev = dev_find_devtype(dev_find_devtype(get_root_device(),
                                                               virtual_bus), net_core_dev);
        return device_find_id(dev, id);
}

/**
 * \fn net_queue_append_list(head, item)
 * \brief This function will append an item to the end of the core driver queue.
 * \param head The queue head.
 * \param item The item which should be added after the last current item.
 * \return Error code.
 */
static int
net_queue_append_list(struct net_queue *queue, struct net_queue* item)
{
        struct net_queue *carriage = queue;
        while (carriage != NULL)
        {
                if (carriage->next == NULL)
                {
                        carriage->next = item;
                        carriage->next->previous = carriage;
                        item->next = NULL;
                        break;
                }

                if (carriage->next != carriage)
                        carriage = carriage->next;
                else
                        break;
        }

        return -E_SUCCESS;
}

/**
 * \fn remove_queue_entry(head,queue_entry)
 * \brief Removes the giver entry from the queue.
 * \param queue The queue head.
 * \return  The removed entry.
 */
static struct net_queue *
remove_queue_entry(struct net_queue *head, struct net_queue *item)
{
        if (head == item)
        {
                head->next->previous = NULL;
                net_set_queue(head->next);
                //head->next = NULL;

        }
        struct net_queue *carriage = head;
        while (carriage != NULL)
        {
                if (carriage == item)
                {
                        if (carriage->next == NULL)
                        { /* we are at the end of the list, so shorten it.. */
                                carriage->previous->next = NULL;
                                goto out;
                        }
                        else
                        { /* we are somewhere in the list, but not at the start, and not at the end */
                                item->next->previous = carriage;
                                carriage->next = item->next;
                                goto out;
                        }
                }
                else if (carriage->next == NULL)
                {
                        break;
                }
                carriage = carriage->next;
        }
        /* error return */
fail:
        return NULL;

        /* success return */
out:
        return carriage;
}

void
print_mac(struct netdev *netdev)
{
        int i;
        for (i = 0; i < 5; i++)
                printf("%x:", netdev->hwaddr[i]);

        printf("%x\n", netdev->hwaddr[5]);
}

static void
init_ptype_tree()
{
        struct protocol *root = &ptype_tree;
        memset(root, 0, sizeof (*root));
        root->type = ROOT;
}

void
add_ptype(struct protocol *head, struct protocol *item)
{
        struct protocol *carriage, *tmp;
        for_each_ll_entry_safe(head, carriage, tmp)
        {
                if(carriage->next == NULL)
                {
                        carriage->next = item;
                        item->previous = carriage;
                        break;
                }
        }
}

struct protocol *
get_ptype(struct protocol *head, enum ptype type)
{
        struct protocol *carriage, *tmp;
        for_each_ll_entry_safe(head, carriage, tmp)
        {
                if(carriage->type == type)
                        return carriage;
        }
}

static int
check_net_buff_tstamp(struct net_buff *buff)
{
        unsigned long long current = get_cpu_tick();
        if(buff->tstamp > current)
                return -E_CORRUPT;
        else
                return -E_SUCCESS;
}

static int
vlan_untag(struct net_buff *buff)
{
        if(buff->raw_vlan == 0)
                return -E_ALREADY_INITIALISED;

        unsigned int raw = buff->raw_vlan;
        buff->vlan = kmalloc(sizeof(*(buff->vlan)));

        if(buff->vlan == NULL)
                return -E_NOMEM;

        buff->raw_vlan = 0; /* make sure it doesn't get detagged again */

        buff->vlan->protocol_tag = ntohs((raw >> 16) & 0xffff);
        raw = ntohs(raw & 0xffff);
        buff->vlan->priority = (raw >> 13) & 3;
        buff->vlan->format_indicator = (raw >> 12) & 1;
        buff->vlan->vlan_id = raw & 0xfff;
        return -E_SUCCESS;
}

#if 1

void
debug_packet_type_tree()
{
        struct protocol *carriage = get_ptype(get_ptype_tree(), IPv4);
        if (carriage == NULL)
                return;

        debug("packet type: %x\n", carriage->type);
}
#endif
