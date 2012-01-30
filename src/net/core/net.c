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
#include <andromeda/drivers.h>
#include <networking/net.h>
#include <networking/eth/eth.h>
#include <networking/netlayer.h>

static struct net_queue *net_core_queue;
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
  struct device *dev = kalloc(sizeof(*dev));
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
  struct net_queue *head = kalloc(sizeof(*head));
  memset(head, 0, sizeof(*head));
  net_set_queue(head);
  init_ptype_tree();
  init_eth();
  netif_netlayer_init();
  initialized = TRUE;
  //debug_packet_type_tree();
}

int
register_net_dev(struct device *dev, struct netdev* netdev)
{
  dev->type = net_dev;
  dev->open = &device_open_driver_io;
  dev->driver->io->fs_data = (void*)netdev;
  dev->driver->io->fs_data_size = sizeof(*netdev);
  
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
  if(dev == NULL)
    return -E_NULL_PTR;
  if(device_detach(dev, device_find_id(dev, id)) == -E_NOTFOUND)
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
  if(head == NULL || x == NULL)
    return -E_NULL_PTR;
  else
  {
    struct net_buff *carriage;
    for_each_net_buff_entry_safe(head, carriage)
    {
      if(carriage->next == NULL)
      {
        carriage->next = x;
        x->previous = x;
        x->next = NULL;
      }
      else
        continue;
    }
  }
  return -E_SUCCESS;
}

struct net_buff *
alloc_buff_frame(unsigned int frame_len)
{
  struct net_buff *buff = kalloc(sizeof(*buff));
  buff->lenth = frame_len;
  buff->datalink_hdr = kalloc(frame_len);
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

  if(nxt)
    free_net_buff_list(nxt);
  else
    return -E_SUCCESS;
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
  return -E_NOFUNCTION;
}

/**
 * \fn net_rx_vfio(vfile, buf, size)
 *
 * Receive a buffer from the device driver.
 */
static size_t
net_rx_vfio(struct vfile *file, char *buf, size_t size)
{
  struct net_buff *buffer = (struct net_buff*)buf;
  debug("Packet arrived in the core driver successfully. MAC address:");
  print_mac(buffer->dev);
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
  struct device *dev_driver = device_find_id(core_dev, ((struct netdev*)buf)->dev_id);
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
  while(carriage != NULL)
  {
    if(carriage->next == NULL)
    {
      carriage->next = item;
      carriage->next->previous = carriage;
      item->next = NULL;
      break;
    }
    
    if(carriage->next != carriage)
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
  if(head == item)
  {
    head->next->previous = NULL;
    net_set_queue(head->next);
    head->next = NULL;

  }
  struct net_queue *carriage = head;
  while(carriage != NULL)
  {
    if(carriage == item)
    {
      if(carriage->next == NULL)
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
    else if(carriage->next == NULL)
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
  for(i = 0; i<5; i++)
    printf("%x:", netdev->hwaddr[i]);

  printf("%x\n", netdev->hwaddr[5]);
}

static void 
init_ptype_tree()
{
  struct packet_type *root = &ptype_tree;
  memset(root, 0, sizeof(*root));
}

void 
add_ptype(struct packet_type *parent, struct packet_type *item)
{
  struct packet_type *carriage = parent->children;
  do
  {
    if(carriage == NULL)
    {
      parent->children = item;
      return;
    }
    if(carriage->next == NULL)
    {
      carriage->next = item;
      item->parent = parent;
      return;
    }
    carriage = carriage->next;
  } while(carriage != NULL);
}

static struct packet_type *
get_ptype(enum ptype type)
{
  return NULL;
}

#if 1
void debug_packet_type_tree()
{
  struct packet_type *carriage = get_ptype_tree()->children;
  if(carriage == NULL)
    return;
  do
  {
    debug("packet child: %x\n", carriage->type);
    carriage = carriage->next;
  }while(carriage != NULL);
}
#endif
