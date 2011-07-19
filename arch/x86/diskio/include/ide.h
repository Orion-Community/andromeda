/*
 *   The interrupts header.
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

/* Status definitions */
#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

/* ATA I/O registers */
#define OL_MASTER_ATA_BASE 0x1f0

#define OL_MASTER_ATA_DATA 		OL_MASTER_ATA_BASE

#define OL_MASTER_ATA_ERR_INFO 		OL_MASTER_ATA_BASE+1
#define OL_MASTER_ATA_FEATURES		OL_MASTER_ATA_ERR_INFO

#define OL_MASTER_ATA_SECTOR_COUNT 	OL_MASTER_ATA_BASE+2
#define OL_MASTER_ATA_LOW_LBA 		OL_MASTER_ATA_BASE+3
#define OL_MASTER_ATA_MID_LBA		OL_MASTER_ATA_BASE+4
#define OL_MASTER_ATA_HIGH_LBA		OL_MASTER_ATA_BASE+5

#define OL_MASTER_ATA_DRIVE_SELECT	OL_MASTER_ATA_BASE+6
#define OL_MASTER_ATA_HEAD_SELECT	OL_MASTER_ATA_DRIVE_SELECT

#define OL_MASTER_ATA_COMMAND		OL_MASTER_ATA_BASE+7
#define OL_MASTER_ATA_STATUS		OL_MASTER_ATA_COMMAND

/* ATA PIO MODE commands */
#define OL_READ_LBA28_SECTORS 0x20
#define OL_READ_LBA48_SECTORS 0x24

// Max 4 partitions
struct partition_table 
{
	uint8_t boot_ind;
	uint8_t starting_head;
	uint16_t starting_sect_cyl; // bits 0-5 starting sector - bits 6-15 starting cylinder
	uint8_t sys_id;
	uint8_t ending_head;
	uint16_t ending_sect_cyl; // same story as byte 2 and 3
	uint32_t lba;
	uint32_t total_sectors; // total sectors in this partition
} bootdrive[4];
typedef struct partition_table OL_ptable

/**
 * Read from drive.
 * 
 * The relative lba value is the lba offset of the partition which contain the sectors.
 * 
 * NOTE: You !CANNOT! read outside a partition, even if those sectors exist.
 */
extern uint8_t ide_read(uint32_t sectors /* sectors to read */, uint32_t buffer /* destination buffer */, 
		OL_ptable * partition /* partition where the sectors are found in */, uint32_t lba /* relative lba */);

/**
 * Initialize the ide driver. It will set the partition table correctly.
 * 
 * NOTE: This function expects that the current partition tables are located at 0x0:0x7c00.
 */
extern OL_ptable * ide_init();
