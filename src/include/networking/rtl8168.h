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

#ifdef __cplusplus
extern "C" {
#endif

void rtl_init_device(struct ol_pci_dev *);

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
  uint tx_enable : 1;
  uint rx_enable : 1;
  uint reset : 1;
};

struct rtl8168
{
  struct txconfig *transmit;
  struct rxconfig *receive;
  struct rtlcommand *command;
};

#ifdef __cplusplus
}
#endif
#endif