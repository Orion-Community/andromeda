/*
 * File:   ipv4.h
 * Author: michel
 *
 * Created on January 28, 2012, 2:42 AM
 */

#ifndef IPV4_H
#define	IPV4_H

#include <stdlib.h>
#include <networking/net.h>

#ifdef	__cplusplus
extern "C" {
#endif

  struct ipv4
  {
    uint version : 4;
    uint hdr_len : 4;
    uint tos : 8; /* type of service */
    uint length : 16;
    uint id : 16;
    uint flags : 3;
    uint offset : 13;
    uint ttl : 8;
    uint protocol : 8;
    uint checksum : 16;
    uint src_addr;
    uint dst_addr;
    uint options;
  };

  static enum ptype netif_rx_ip(struct net_buff *buff);
#ifdef	__cplusplus
}
#endif

#endif	/* IPV4_H */

