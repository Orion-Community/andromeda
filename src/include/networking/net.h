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

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_ADDR_SIZE 6
  
/* loops */
#define for_each_queue_item_safe(head, carriage) for((carriage) = (head); \
                                                    (carriage)->next != null, \
                                                    (carriage) != (carriage)->next; \
                                                    (carriage) = (carriage)->next)
  
  typedef void* net_buff_data_t;
  
/**
 * Incoming packets will be queued using this structure. When a packed arrives,
 * it will be appended after the last current entry. The core driver will empty
 * the queue from the beginning on, this creates a FIFO situation. Used like an
 * inverted Java Queue object.
 */
struct net_queue
{
  struct net_queue *next;
  struct net_queue *previous;
  
  struct net_buff *packet;
} __attribute__((packed));
  
struct netbuf
{
  uint16_t length,data_len;
  void *framebuf;
};

struct netdev
{
  uint32_t (*rx)();
  void (*tx)(struct netbuf*);
  uint8_t hwaddr[MAC_ADDR_SIZE]; /* The NIC's MAC address */
  struct netbuf buf; /* Current processed frame buffer */
  struct net_queue *queue_head;
};

struct net_buff
{
  struct net_buff *next;
  struct net_buff *previous;
  
  unsigned int lenth;
  unsigned short data_len;
  struct netdev dev;
  
  net_buff_data_t transport_hdr;
  net_buff_data_t network_hdr;
  net_buff_data_t datalink_hdr;
} __attribute__((packed));

typedef void(*tx_hook_t)(struct net_buff*);
typedef net_buff_data_t(*rx_hook_t)();

/**
 * \fn register_net_dev(dev)
 * \brief Register a NIC device driver in the core driver.
 */
int register_net_dev(struct netdev* dev);
int net_rx();
struct net_buff *alloc_buff_frame(unsigned int frame_len);
static int free_net_buff_list(struct net_buff* nb);
static int process_net_buff(struct net_buff* buff);
static int net_buff_append_list(struct net_buff *alpha, struct net_buff *beta);
static struct net_queue *remove_first_queue_entry(struct net_queue queue);
static int net_queue_append_list(struct net_queue queue, struct net_queue* item);
static void process_rx_packet(struct net_buff *packet);

/**
 * \fn net_rx_vfio(vfile, buf, size)
 * 
 * Receive a buffer from the device driver.
 */
static int net_rx_vfio(struct vfile *, char*, size_t);

/**
 * \fn net_tx_vfio(vfile, buf, size)
 * 
 * Transmit a buffer using virtual files.
 */
static int net_tx_vfio(struct vfile*, char*, size_t);

#ifdef __cplusplus
}
#endif
#endif /* __NET_H */
