/*
 *   Andromeda Project - General network header.
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

/** \file */

#ifndef __NET_H
#define __NET_H

#include <stdlib.h>
#include <fs/vfs.h>
#include <sys/dev/pci.h>
#include <lib/byteorder.h>
#include <andromeda/drivers.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAC_ADDR_SIZE 6

        /* loops */
#define for_each_queue_item_safe(head, carriage) for((carriage) = (head); \
                                                    (carriage)->next != null, \
                                                    (carriage) != (carriage)->next; \
                                                    (carriage) = (carriage)->next)
#define for_each_net_buff_entry_safe(head, carriage, tmp) for_each_ll_entry_safe(\
                                                        head, carriage, tmp)

#define RX_BUFFER_SIZE (1024*8)+16+1500 /* 8KiB + header + 1 extra frame */
#define TX_BUFFER_SIZE 1500+16 /* 1 frame + header */
typedef void* net_buff_data_t;


enum ptype
{
        ROOT = 0,
        ETHERNET,
        ARP,
        IPv4,
        IPv6,
        IMCP,
        TCP,
        UDP
};

/**
 * \enum packet_state
 * \brief The current state of a packet.
 *
 * \var P_DELIVERED
 * \brief The packet has been delivered to the location it has to be.
 * \var P_ANOTHER_ROUND
 * \brief The protocol handler found another protocol inside and needs to be
 * handled again.
 * \var P_DROPPED
 * \brief The received packet does not belong to the receiving machine.
 * \var P_LOST
 * \brief The packet is lost during processing. This can mean that an encapsulated
 * protocol is not supported.
 */
enum packet_state
{
        P_DELIVERED,
        P_ANOTHER_ROUND,
        P_QUEUED,
        P_DONE,
        P_DROPPED,
        P_LOST,
        P_NOTCOMPATIBLE,
};

/**
  * \struct net_queue
  * \brief Incoming packets will be queued using this structure. When a packed
  * arrives, it will be appended after the last current entry. The core driver
  * will empty the queue from the beginning on, this creates a FIFO situation.
  * Used like an inverted Java Queue object.
  */
struct net_queue
{
        struct net_queue *next;
        struct net_queue *previous;

        struct net_buff *packet;
}
__attribute__((packed));

/**
  * \struct net_bridge
  * \brief Used to bridge a packet between two network interfaces.
  * \see struct netdev
  */
struct net_bridge
{
        /**
          * \var input
          * \brief The input interface.
          *
          * \var output
          * \brief The output interface.
          */
        struct netdev *input;
        struct netdev *output;
};

typedef enum ptype (*netif_rx_pull)(struct net_buff*);
typedef enum ptype (*protocol_deliver_handler_t)(struct net_buff*);
typedef void (*protocol_queue_notify_handler_t)();

struct protocol
{
        struct protocol *next;
        struct protocol *previous;

        enum ptype type;
        protocol_deliver_handler_t deliver_packet;
        protocol_queue_notify_handler_t notify;
};

/**
 * \struct vlan_tag
 * \brief The Virtual Lan info struct.
 */
typedef struct vlan_tag
{
        /**
         * \var protocol_tag
         * \brief Indicates the protocol it is encapsulated in.
         * \var priority
         * \brief Frame priority.
         * \var format_indicator
         * \brief Token ring ecapsulation.
         * \var vlan_id
         * \brief ID of the virtual lan.
         */
        unsigned short protocol_tag;
        uint priority : 3;
        uint format_indicator : 1;
        uint vlan_id : 12;
} *vlan_tag_t;

struct netdev
{
        uint8_t hwaddr[MAC_ADDR_SIZE]; /* The NIC's MAC address */
        atomic_t state;
        struct pci_dev *dev;
        void *device_data;
        uint64_t dev_id;
        struct net_queue *queue_head;
        enum ptype frame_type;
        netif_rx_pull rx_pull_handle;
        uint poll_support : 1;
};

/**
 * \struct net_buff
 * \brief The protocol independent network stack buffer.
 */
struct net_buff
{
       /**
        * \var next
        * \brief Next pointer
        * \var previous
        * \brief Prev pointer
        * \var length
        * \brief Used length
        * \var total_length
        * \brief Allocated length
        * \var data_len
        * \brief Length of the actual data
        * \var transport_hdr
        * \brief Pointer to the transport header
        * \var network_hdr
        * \brief Pointer to the network header
        * \var datalink_hdr
        * \brief Pointer to the datalink header
        * \var head
        * \brief Buffer head
        * \var data
        * \brief Data head
        * \var tail
        * \brief Buffer tail pointer
        * \var end
        * \brief End of the buffer
        */
        struct net_buff *next;
        struct net_buff *previous;

        unsigned int length, total_len;
        unsigned long long tstamp;
        unsigned short data_len;
        struct netdev *dev;

        enum ptype type;
        struct vlan_tag *vlan;
        __32be raw_vlan;
        struct net_bridge *bridge;
        net_buff_data_t transport_hdr;
        net_buff_data_t network_hdr;
        net_buff_data_t datalink_hdr;

        unsigned char* head, *data, *tail, *end;
        atomic_t users;
} __attribute__((packed));

/**
  * \fn register_net_dev(dev)
  * \brief Register a NIC device driver in the core driver.
  */
int register_net_dev(struct device *dev, struct netdev* netdev);

/**
  * \fn unregister_net_dev(dev)
  * \brief Unregisters the given <i>netdev</i> in the kernel.
  *
  * @param dev The device id to unregister.
  * @return E code.
  */
int unregister_net_dev(uint64_t id);

/**
 * \fn netif_rx_process(struct net_buff*)
 * \brief Receives, processess and polls for incoming packets.
 *
 * \param nb The first packet to handle.
 * \return State of handled packet
 * \see netif_process_queue(struct net_queue *head, unsigned int load)
 * \see struct net_buff, struct netdev
 *
 * This function handles all packets which are fresh from the device driver or
 * the netif queue (see <i>netif_process_queue(head, load)</i>). After handling
 * a packet it will poll from the device driver for more incoming packets.
 */
/*
static enum packet_state netif_rx_process(struct net_buff *nb);
*/
struct device *get_net_driver(uint64_t id);

struct net_buff *alloc_buff_frame(unsigned int frame_len);
#if 0
static int free_net_buff_list(struct net_buff* nb);
/**
  * \fn net_buff_append_list(head, x)
  * \brief The net_buff item x will be appended to the net_buff head <i>head</i>.
  * \param head The list head.
  * \param x Item to be appended.
  * \return Error code.
  */
static int net_buff_append_list(struct net_buff *head, struct net_buff *x);

/**
  * \fn remove_queue_entry(head,queue_entry)
  * \brief Removes the giver entry from the queue.
  * \param queue The queue head.
  * \return  The removed entry.
  */
static struct net_queue *remove_queue_entry(struct net_queue *head,
                                            struct net_queue *item);
/**
  * \fn net_queue_append_list(head, item)
  * \brief This function will append an item to the end of the core driver queue.
  * \param head The queue head.
  * \param item The item which should be added after the last current item.
  * \return Error code.
  */
static int net_queue_append_list(struct net_queue *queue, struct net_queue* item);

/**
 * \fn vlan_untag(buff)
 * \brief Converts raw vlan tags to a better readable the better readable struct
 * vlan_tag.
 * \see vlan_tag
 */
static int vlan_untag(struct net_buff *buff);

static int check_net_buff_tstamp(struct net_buff *buff);
#endif
void print_mac(struct netdev *netdev);

/**
 * \fn netif_drop_net_buff(buff)
 * \brief Packet is not valid/usable, drop it.
 * \param buff The packet to drop.
 */
void netif_drop_net_buff(struct net_buff *buff);

#if 0
/**
  * \fn net_rx_vfio(vfile, buf, size)
  *
  * Receive a buffer from the device driver.
  */
static size_t net_rx_vfio(struct vfile *, char*, size_t);

/**
  * \fn net_tx_vfio(vfile, buf, size)
  *
  * Transmit a buffer using virtual files.
  */
static size_t net_tx_vfio(struct vfile*, char*, size_t);

static void init_ptype_tree();
#endif
/**
  * \fn add_ptye(parent, item)
  * \brief Adds a packet type to to tree.
  * \param parent Parent node.
  * \param item Item which should be added.
  */
void add_ptype(struct protocol *parent, struct protocol *item);

#if 0
/**
 * \fn netif_process_queue(struct net_queue *head, unsigned int load)
 * \brief Processes <i>load</i> amount of packets from the queue.
 * \param head The head of the receive queue.
 * \param load The amount of packets to process
 * \return The error code. Returns 0 on success.
 *
 * The packets taken from the queue are passed to
 * <i>netif_process_net_buff(buff)</i>.
 */
static int netif_process_queue(struct net_queue *head, unsigned int load);
#endif

struct protocol *get_ptype(struct protocol *head, enum ptype type);

extern struct protocol ptype_tree;

/**
 * \fn init_netif()
 * \brief Initialize the net core driver.
 * @return Error code.
 */
int init_netif();

/**
  * \fn get_ptype_tree()
  * @return The packet_type tree.
  */
static inline struct protocol*
get_ptype_tree()
{
        return &ptype_tree;
}

/*
 * Packet buffer pointer functions
 */

void net_buff_reserve(struct net_buff *nb, int size);

static inline void
net_buff_reset_tail(struct net_buff *buff)
{
        buff->tail = buff->data;
}

static inline void
net_buff_reset_net_hdr(struct net_buff *buff)
{
        buff->network_hdr = buff->data;
}

static inline void
net_buff_reset_datalink_hdr(struct net_buff *buff)
{
        buff->datalink_hdr = buff->data;
}

static inline int
netpoll_dev(struct netdev *dev)
{
        return !(dev->poll_support);
}

/**
 * \fn  get_net_core_driver()
 * \brief Returns the core device structure for net-core.
 */
static inline struct device *
get_net_core_driver()
{
        return dev_find_devtype(dev_find_devtype(
                                get_root_device(), virtual_bus), net_core_dev);
}

void debug_packet_type_tree();

#ifdef __cplusplus
}
#endif
#endif /* __NET_H */
