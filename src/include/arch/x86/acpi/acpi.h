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

#define OL_APIC_ENABLE 0x1
#define OL_PCAT_COMPAT 0x1 /* A one indicates that the system also has 
                              a PC-AT-compatible dual-8259 setup. The 8259 
                              vectors must be disabled (that is, masked) when 
                              enabling the ACPI APIC operation. */

#ifdef __cplusplus
extern "C"
{
#endif

    struct ol_madt_field_header
    {
        uint8_t type, length;
    } __attribute__((packed));
    typedef struct ol_madt_field_header *ol_madt_field_header_t;

    struct ol_madt_apic
    {
        uint8_t type, length, proc_id, apic_id;
        uint32_t flags : 1;
    } __attribute__((packed));
    typedef struct ol_madt_apic *ol_madt_apic_t;

    struct ol_madt_ioapic
    {
        uint8_t type, length, id, reserved;
        uint32_t address, global_system_interrupt_base;
    } __attribute__((packed));
    typedef struct ol_madt_ioapic *ol_madt_ioapic_t;

    struct ol_madt
    {
        char signature[4];
        uint32_t length;
        uint8_t rev;
        uint8_t checksum;
        char oemid[6];
        char oem_table_id[8];
        uint32_t rev_id;
        char creatorid[4];
        uint32_t creator_rev;
        uint32_t lapic_addr;
        uint32_t flags;
    } __attribute__((packed));
    typedef struct ol_madt *ol_acpi_madt_t;

    struct ol_rsdt
    {
        char signature[4];
        uint32_t length;
        uint8_t rev, checksum;
        char oemid[6], oemtableid[8];
        uint32_t oem_rev, creatorid, creator_rev;
    } __attribute__((packed));
    typedef struct ol_rsdt *ol_acpi_rsdt_t;

    struct ol_acpi_rsdp
    {
        /* universal part */
        char signature[8];
        uint8_t checksum;
        char oemid[6];
        uint8_t revision;
        uint32_t rsdt;

#ifdef ACPI2        /* acpi 2.0+ part */
        uint32_t length;
        uint64_t xsdt_address;
        uint8_t extendedChecksum;
        uint8_t reserved[3];
#endif
    } __attribute__((packed));
    typedef struct ol_acpi_rsdp *ol_acpi_rsdp_t;

    extern ol_acpi_rsdp_t rsdp;

    static ol_acpi_madt_t
    ol_acpi_get_madt();

    static void**
    ol_acpi_enumerate(uint8_t type);

    ol_madt_ioapic_t
    ol_acpi_get_ioapic();
    
    struct ol_madt_apic *
    acpi_get_apic();

#ifdef __cplusplus
}
#endif

#endif
