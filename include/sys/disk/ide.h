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

#include <sys/dev/pci.h>

#ifndef __IDE_H
#define __IDE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ATA bus defines */
#define OL_MASTER_BUS 0x1f0
#define OL_DATA_BUS(x) (x)

#define OL_ATA_ERR_INFO(x) (x+1)
#define OL_ATA_FEATURES(x) (x+1)

#define OL_ATA_SECTOR_COUNT(x) (x+2)
#define OL_ATA_LOW_LBA(x) (x+3)
#define OL_ATA_MID_LBA(x) (x+4)
#define OL_ATA_HIGH_LBA(x) (x+5)

#define OL_ATA_DRIVE_SELECT(x) (x+6)
#define OL_ATA_HEAD_SELECT(x) (x+6)

#define OL_ATA_COMMAND(x) (x+7)
#define OL_ATA_STATUS(x) (x+7)

/* ATA device types */
#define OL_ATA_PATAPI 0x1
#define OL_ATA_SATAPI 0x2
#define OL_ATA_PATA 0x4
#define OL_ATA_SATA 0x8
#define OL_ATA_UNKNOWN 0xff

// Max 4 partitions
typedef struct partition_table
{
	uint8_t boot_ind;
	uint8_t starting_head;
	uint16_t starting_sect_cyl; // bits 0-5 starting sector - bits 6-15 starting cylinder
	uint8_t sys_id;
	uint8_t ending_head;
	uint16_t ending_sect_cyl; // same story as byte 2 and 3
	uint32_t lba;
	uint32_t total_sectors; // total sectors in this partition
} *ol_ptable_t;

typedef struct ol_ata_dev
{
        uint16_t base_port;
        uint16_t dcr; /* device control register */
        uint8_t slave;
        pci_dev_t pci_dev;
} *ol_ata_dev_t;

/**
 * Read from drive.
 *
 * The relative lba value is the lba offset of the partition which contain the sectors.
 *
 * NOTE: You !CANNOT! read outside a partition, even if those sectors exist.
 */
extern uint8_t ide_read(uint32_t sectors /* sectors to read */, uint32_t buffer /* destination buffer */,
		ol_ptable_t partition /* partition where the sectors are found in */, uint32_t lba /* relative lba */);

/**
 * Initialize the ide driver. It will set the partition table correctly. It returns the active partition.
 *
 * NOTE: This function expects that the current partition tables are located at 0x0:0x7c00.
 */
extern uint8_t ide_init(ol_ptable_t partitions);

/**
 * Get information about the drive.
 */
extern uint8_t ata_identify(void);

int
ol_ata_detect_dev_type(ol_ata_dev_t);

static void
ol_ata_soft_reset(ol_ata_dev_t);

#ifdef __cplusplus
}
#endif

#endif
