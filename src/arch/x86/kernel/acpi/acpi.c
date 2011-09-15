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
#include <text.h>

#include <arch/x86/acpi/acpi.h>

ol_acpi_rsdp_t rsdp;

static ol_acpi_madt_t
ol_acpi_get_madt()
{
        ol_acpi_rsdt_t rsdt = (void*)rsdp->rsdt;
        
        void * table;
        uint32_t len = (rsdt->length - sizeof(*rsdt)) / 4, i = 0; /* default length */
        
        for(table = (void*)rsdt+sizeof(*rsdt); i<len; i++, table+=4)
        {
                if(!memcmp((void*)*((uint32_t*)table), "APIC", 4))
                {
                        /* go party, we found the madt - bail out */
                        return (ol_acpi_madt_t) *((uint32_t*)table);
                }
        }
}

ol_madt_apic_t*
ol_acpi_enumerate_apics()
{
        ol_acpi_madt_t madt = ol_acpi_get_madt();
        ol_madt_field_header_t header;
        ol_madt_apic_t* apics = kalloc(sizeof(ol_madt_apic_t));
        uint32_t i = 0;

        for(header = ((void*)madt)+sizeof(*madt); (void*)header < ((void*)madt)+
            madt->length; header = (ol_madt_field_header_t)(((void*)header)+
                                                            header->length))
        {
                if(!header->type) /* processor apics have type number 0 */
                {
                        apics[i] = (ol_madt_apic_t)header;
                        i++;
                }
        }
        return apics;
}
