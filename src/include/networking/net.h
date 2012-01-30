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

#ifndef __NET_H
#define __NET_H

#include <stdlib.h>
#include <fs/vfs.h>
#include <sys/dev/pci.h>

#ifdef __cplusplus
extern "C" {
#endif
  
#define MAC_ADDR_SIZE 6

/* loops */
#define for_each_queue_item_safe(head, carriage) for((carriage) = (head); \
                                                    (carriage)->next != null, \
                                                    (carriage) != (carriage)->next; \
                                                    (carriage) = (carriage)->next)
#define for_each_net_buff_entry_safe(head, carriage) for_each_ll_entry_safe(head, \
                                                                       carriage)

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
} __attribute__((packed));

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

struct netdev
{
  uint8_t hwaddr[MAC_ADDR_SIZE]; /* The NIC's MAC address */
  atomic_t state;
  struct pci_dev *dev;
  void *device_data;
  uint64_t dev_id;
  struct net_queue *queue_head;
};

struct packet_type
{
  struct packet_type *next;
  struct packet_type *children;
  struct packet_type *parent;
  
  enum ptype type;
  int (*rx_hook)(struct net_buff*);
};

struct net_buff
{
  struct net_buff *next;
  struct net_buff *previous;

  unsigned int lenth;
  unsigned short data_len;
  struct netdev *dev;

  struct packet_type type;
  struct net_bridge *bridge;
  net_buff_data_t transport_hdr;
  net_buff_data_t network_hdr;
  net_buff_data_t datalink_hdr;
  
  unsigned char* head, data, tail, end;
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

struct device *get_net_driver(uint64_t id);

struct net_buff *alloc_buff_frame(unsigned int frame_len);
static int free_net_buff_list(struct net_buff* nb);

/**
 * \fn rx_process_net_buff(buff)
 * \brief Processes the received net_buff trough the entire network stack.
 * \warning Should only be called from net_rx_vfio(vfile, char*, size_t)
 *
 * \param buff The received net buffer.
 */
static int rx_process_net_buff(struct net_buff* buff);

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

void print_mac(struct netdev *netdev);

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

/**
 * \fn get_ptye(type)
 * \brief Returns the correspondending packet type
 * \param type The type identifier to look for.
 * \return 
 */
struct packet_type *get_ptype(struct packet_type *parent, enum ptype type);

/**
 * \fn add_ptye(parent, item)
 * \brief Adds a packet type to to tree.
 * \param parent Parent node.
 * \param item Item which should be added.
 */
void add_ptype(struct packet_type *parent, struct packet_type *item);

extern struct packet_type ptype_tree;
/**
 * \fn get_ptype_tree()
 * @return The packet_type tree.
 */
static inline struct packet_type*
get_ptype_tree()
{
  return &ptype_tree;
}

void debug_packet_type_tree();

#ifdef __cplusplus
}
#endif
#endif /* __NET_H */
