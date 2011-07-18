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

/* Some error code definitions. */
#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

/* Command definitions */
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

// ATAPI only commands:
#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

/* IDE MASTER/SLAVE definitions */
#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

/* I/O registers */
#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

/* Channels */
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY    0x01
 
/* Directions */
#define      ATA_READ      0x00
#define      ATA_WRITE     0x01

// channel structures - master/slave
struct IDEChannelRegisters {
	uint16_t base;  // I/O Base.
	uint16_t ctrl;  // Control Base
	uint16_t bmide; // Bus Master IDE
	uint8_t  nIEN;  // nIEN (No Interrupt);
} channels[2];

// Max 4 drives
struct ide_device {
	uint8_t reserved;    // 0 (Empty) or 1 (This Drive really exists).
	uint8_t channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
	uint8_t drive;       // 0 (Master Drive) or 1 (Slave Drive).
	uint16_t type;        // 0: ATA, 1:ATAPI.
	uint16_t signature;   // Drive Signature
	uint16_t capabilities;// Features.
	uint32_t commandSets; // Command Sets Supported.
	uint32_t size;        // Size in Sectors.
	uint8_t model[41];   // Model in string.
} ide_devices[4];

/**
 * Read from a channel
 */
uint8_t ide_read(uint8_t chan, uint8_t reg);

/**
 * Write to a channel
 */
void ide_write(uint8_t chan, uint8_t reg, uint8_t data);

/**
 * Read the buffer
 */
void ide_read_buffer(uint8_t chan, uint8_t reg, uint32_t buf, uint32_t quads);