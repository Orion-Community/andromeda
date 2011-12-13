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
{};

struct rxconfig
{};

struct rtlcommand
{
  
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