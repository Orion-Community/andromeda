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

#include <stdlib.h>
#include <arch/x86/timer.h>
#include <andromeda/drivers.h>
#include <networking/net.h>
#include <networking/eth/eth.h>
#include <networking/netlayer.h>

static struct net_queue *net_core_queue;
static struct net_queue *net_tx_core_queue;
struct packet_type ptype_tree;
static bool initialized = FALSE;

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
        struct device *dev = kalloc(sizeof (*dev));
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
        struct net_queue *head = kalloc(sizeof (*head));
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
        struct net_buff *buff = kalloc(sizeof (*buff));
        buff->length = frame_len;
        buff->head = kalloc(frame_len);
        buff->data = buff->head;
        buff->tail = buff->head;
        buff->end = buff->head + frame_len;
        
        return buff;
}

static int
free_net_buff_list(struct net_buff* nb)
{
        free(nb->transport_hdr);
        free(nb->network_hdr);
        free(nb->datalink_hdr);
        struct net_buff *nxt = nb->next;
        free(nb);

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
 * \fn netif_process_net_buff(buff)
 * \brief Processes the received net_buff trough the entire network stack.
 * \warning Should only be called from net_rx_vfio(vfile, char*, size_t)
 *
 * \param buff The received net buffer.
 */
static int
netif_process_net_buff(struct net_buff *buff)
{
        struct packet_type *ptype, *old_type = kalloc(sizeof(*old_type)), *tmp, 
          *root = get_ptype_tree();
        struct netdev *dev;
        int retval = P_DROPPED;
        protocol_deliver_handler_t handle;
        
        if(check_net_buff_tstamp(buff))
        {
                netif_drop_net_buff(buff);
                goto out;
        }
        
        if(netpoll_dev(buff->dev))
        {
                netif_drop_net_buff(buff);
                goto out;
        }
        
        net_buff_reset_net_hdr(buff);
        net_buff_reset_datalink_hdr(buff);
        net_buff_reset_tail(buff);
        
        dev = buff->dev;
        buff->type = dev->frame_type;
        
        next_round:
        for_each_ll_entry_safe(root, ptype, tmp)
        {
                if(ptype->type == buff->type)
                {
                        handle = ptype->deliver_packet;
                        atomic_inc(&buff->users);
                        buff->type = ptype->type;
                        if(buff->type == ETHERNET)
                                vlan_untag(buff);
                        break;
                }
        }
        
        /*
         * We dont want to use NULL handles.
         */
        if(handle)
        {
                retval = handle(buff);
                if(retval == P_DROPPED || retval == P_LOST)
                        goto out;
                if(retval == P_ANOTHER_ROUND)
                        goto next_round;
        }
        
        if(buff->dev->rx_pull_handle)
        {
                /*
                 * if a pull function is given, try to pull for more packets
                 */
                switch(retval = dev->rx_pull_handle(buff))
                {
                        
                        case P_ANOTHER_ROUND:
                                goto next_round;
                                break;
                        case P_DELIVERED:
                                break;
                        case P_DROPPED:
                                goto out;
                                break;
                        case P_DONE:
                                break;
                        default:
                                warning("Packet handled incorrectly");
                                break;
                }
        }
        
        /*
         * If one last packet has dropped out..
         */
        for_each_ll_entry_safe(root, ptype, tmp)
        {
                if(retval == P_DELIVERED && ptype->type == buff->type)
                {
                        retval = ptype->deliver_packet(buff);
                        atomic_inc(&buff->users);
                        if(retval == P_DROPPED || retval == P_LOST)
                                goto out;
                        break;
                }
        }
        
        kfree(old_type);
        return retval;
        
        out:
        kfree(old_type);
        free_net_buff_list(buff);
        return retval;
}

static int
netif_start_tx(struct net_buff *buff)
{
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
        netif_process_net_buff(buffer);
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
                head->next = NULL;

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
        struct packet_type *root = &ptype_tree;
        memset(root, 0, sizeof (*root));
        root->type = ROOT;
}

void
add_ptype(struct packet_type *head, struct packet_type *item)
{
        struct packet_type *carriage, *tmp;
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

struct packet_type *
get_ptype(struct packet_type *head, enum ptype type)
{
        struct packet_type *carriage, *tmp;
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
        buff->vlan = kalloc(sizeof(*(buff->vlan)));
        
        if(buff->vlan == NULL)
                return -E_NOMEM;

        buff->raw_vlan = 0; /* make sure it doesn't get detagged again */
        
        buff->vlan->protocol_tag = (raw >> 16) & 0xffff;
        buff->vlan->priority = (raw >> 13) & 3;
        buff->vlan->format_indicator = (raw >> 12) & 1;
        buff->vlan->vlan_id = raw & 0xfff;
        return -E_SUCCESS;
}

#if 1

void
debug_packet_type_tree()
{
        struct packet_type *carriage = get_ptype(get_ptype_tree(), IPv4);
        if (carriage == NULL)
                return;

        debug("packet type: %x\n", carriage->type);
}
#endif
