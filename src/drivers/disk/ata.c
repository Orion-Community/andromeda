/*
 *   Public ide header file.
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
#include <sys/dev/pci.h>
#include <sys/disk/ide.h>

int
ol_ata_detect_dev_type (ol_ata_dev_t dev)
{
	ol_ata_soft_reset(dev); /* waits until master drive is ready again */
        outb(OL_ATA_FEATURES(dev->base_port), 4);
	outb(OL_ATA_DRIVE_SELECT(dev->base_port), 0xA0 | (dev->slave << 4));
	inb(dev->dcr); /* wait 400ns for drive select to work */
	inb(dev->dcr);
	inb(dev->dcr);
	inb(dev->dcr);
	uint8_t cl = inb(OL_ATA_MID_LBA(dev->base_port));
	uint8_t ch = inb(OL_ATA_HIGH_LBA(dev->base_port));

	/* differentiate ATA, ATAPI, SATA and SATAPI */
	if(cl == 0x14 && ch == 0xEB) return OL_ATA_PATAPI;
	if(cl == 0x69 && ch == 0x96) return OL_ATA_SATAPI;
	if(cl == 0 && ch == 0) return OL_ATA_PATA;
	if(cl == 0x3c && ch == 0xc3) return OL_ATA_SATA;
	return OL_ATA_UNKNOWN;
}

static void
ol_ata_soft_reset(ol_ata_dev_t dev)
{
        outb(dev->dcr, 4); /* set the srst bit - e.g. do a software reset */
        outb(dev->dcr, 0);
}
