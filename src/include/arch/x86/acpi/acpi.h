/*
 *   The OpenLoader project - ACPI interface
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

#ifndef __ACPI_H
#define __ACPI_H

struct ol_rsdp 
{
    /* universal part */
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;

    /* acpi 2.0+ part */
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} __attribute__((packed));
typedef struct ol_rsdp *ol_acpi_rsdp_t;

extern ol_acpi_rsdp_t rsdp;

#endif
