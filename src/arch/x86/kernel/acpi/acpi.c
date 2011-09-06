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
#include <arch/x86/acpi/acpi.h>

ol_acpi_rsdp_t rsdp;

static inline ol_acpi_madt_t
ol_acpi_get_madt()
{
        ol_acpi_rsdt_t rsdt = (void*)rsdp->rsdt;
        
}

ol_madt_apic_t
ol_acpi_enumerate_apics()
{
        
}
