/*
 Andromeda
 Copyright (C) 2014 - 2015  Bart Kuivenhoven

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESbuffer_initS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <andromeda/system.h>
#include <andromeda/drivers.h>
#include <drivers/serial.h>
#ifdef X86
#include <arch/x86/bios.h>
#include <arch/x86/pic.h>
#endif
#include <ioctl.h>
#include <stdio.h>
#include <fs/vfs.h>

#define SERIAL_PORT_UNINITIALISED       0
#define SERIAL_PORT_READY               1

#define SERIAL_INTERRUPT_OFF            (uint8_t)(0<<0)
#define SERIAL_INTERRUPT_RX_READY       (uint8_t)(1<<0)
#define SERIAL_INTERRUPT_TX_READY       (uint8_t)(1<<1)
#define SERIAL_INTERRUPT_ERROR          (uint8_t)(1<<2)
#define SERIAL_INTERRUPT_MODEM_STATE    (uint8_t)(1<<3)

// <Status line>
// Bits 0 and 1
#define SERIAL_CHAR_LEN_5               0x0
#define SERIAL_CHAR_LEN_6               0x1
#define SERIAL_CHAR_LEN_7               0x2
#define SERIAL_CHAR_LEN_8               0x3

// Bit 2
#define SERIAL_STOP_SHORT               (0 << 2)
#define SERIAL_STOP_LONG                (1 << 2)

// Bits 3, 4, 5 and 6
#define SERIAL_PARITY_NONE              (0x0 << 3)
#define SERIAL_PARITY_ODD               (0x1 << 3)
#define SERIAL_PARITY_EVEN              (0x3 << 3)
#define SERIAL_PARITY_MARK              (0x5 << 3)
#define SERIAL_PARITY_SPACE             (0x7 << 3)

// Bit 7
#define SERIAL_DLAB                     (uint8_t)(1 << 7)
// </status line>

#define SERIAL_IRQ_PENDING              (1 << 0)
#define SERIAL_IRQ_MODEM                (0 << 1)
#define SERIAL_IRQ_TX_EMPTY             (1 << 1)
#define SERIAL_IRQ_RX_READY             (2 << 1)
#define SERIAL_IRQ_RECV_LINE            (3 << 1)
#define SERIAL_IRQ_TIMEOUT              (6 << 1)

#define SERIAL_IRQ_RESET_MASK           (7 << 1)
#define SERIAL_IRQ_FIFO_MASK            (3 << 6)

#define SERIAL_FIFO_ENABLE              (1 << 0)
#define SERIAL_FIFO_CLEAR_RX            (1 << 1)
#define SERIAL_FIFO_CLEAR_TX            (1 << 2)
#define SERIAL_FIFO_DMA                 (1 << 3)

#define SERIAL_FIFO_TRIG_1              (0x0 << 6)
#define SERIAL_FIFO_TRIG_4              (0x1 << 6)
#define SERIAL_FIFO_TRIG_8              (0x2 << 6)
#define SERIAL_FIFO_TRIG_14             (0x3 << 6)

#define SERIAL_MODEM_DTR                (1 << 0) // Data terminal Ready
#define SERIAL_MODEM_RTS                (1 << 1) // Ready to send
#define SERIAL_MODEM_OUT1               (1 << 2) // Enable or disable port
#define SERIAL_MODEM_OUT2               (1 << 3) // Interrupt type
#define SERIAL_MODEM_LOOP               (1 << 4)

#define SERIAL_STATUS_RX_READ           (1 << 0)
#define SERIAL_STATUS_ERR_OVERRUN       (1 << 1)
#define SERIAL_STATUS_ERR_PARITY        (1 << 2)
#define SERIAL_STATUS_ERR_FRAMING       (1 << 3)
#define SERIAL_STATUS_ERR_BREAK         (1 << 4)
#define SERIAL_STATUS_TX_EMPTY          (1 << 5)
#define SERIAL_STATUS_DATA_EMPTY        (1 << 6)
#define SERIAL_STATUS_ERR_FIFO          (1 << 7)

#define SERIAL_DATA_REG                 0
#define SERIAL_BOUD_LSB                 0
#define SERIAL_INTERRUPT                1
#define SERIAL_BOUD_MSB                 1

#define SERIAL_BAUD_BASE                115200

#define SERIAL_IRQ_CONTROL              2
#define SERIAL_FIFO_REGISTER            2

#define SERIAL_LINE_CONTROL             3
#define SERIAL_MODEM_CONTROL            4
#define SERIAL_LINE_STATUS              5
#define SERIAL_MODEM_STATUS             6
#define SERIAL_SCRATCH_REG              7

#define SERIAL_MAGIC                    0xC0DE

struct serial_port_data {
        uint16_t magic;
        uint8_t port_status;
        uint8_t char_len;
        uint8_t stop_char;
        uint8_t parity;
        uint16_t io_port;
        uint32_t baud;
        uint16_t irq1_id;
        uint16_t irq2_id;
        uint32_t opened;
        mutex_t port_lock;
        struct vfile* dev_file;
};

static int serial_read_data(uint16_t io_port, char* buffer, size_t len)
{
        if (buffer == NULL || len == 0) {
                return 0;
        }
        size_t i = 0;
        while ((inb(io_port + SERIAL_LINE_STATUS) & SERIAL_STATUS_RX_READ)) {
                buffer[i] = inb(io_port);
                i++;
                if (i >= len) {
                        break;
                }
        }

        return i;
}

#define BUF_LEN 32
static int serial_read(uint16_t io_port, int timeout)
{
        char* buf = kmalloc(BUF_LEN);
        if (buf == NULL) {
                inb(io_port);
                return -E_SUCCESS;
        }
        memset(buf, 0, BUF_LEN);

        serial_read_data(io_port, buf, BUF_LEN);

        if (timeout) {
                printf("timeout: %s\n", buf);
        } else {
                printf("out: %s\n", buf);
        }

        outb(io_port, buf[0]);
        outb(io_port, '\t');
        outb(io_port, 'a');
        outb(io_port, 'c');
        outb(io_port, 'k');
        outb(io_port, '\n');
        return -E_SUCCESS;
}

static int serial_interrupt(uint16_t int_no __attribute__((unused)),
                uint16_t int_id __attribute__((unused)),
                uint64_t r1 __attribute__((unused)),
                uint64_t r2 __attribute__((unused)),
                uint64_t r3 __attribute__((unused)),
                uint64_t r4 __attribute__((unused)), void* args)
{
        /**
         * \todo Change this to send a timer callback, for atomic processing
         */
        if (args == NULL) {
                warning("1 Something conflicts with the serial interrupt!\n");
                return -E_SUCCESS;
        }

        struct serial_port_data* data = args;
        if (data->magic != SERIAL_MAGIC) {
                warning("2 Something conflicts with the serial interrupt!\n");
                return -E_SUCCESS;
        }

        uint16_t interrupt = inb(data->io_port + SERIAL_IRQ_CONTROL);

        uint16_t interrupt_pending = interrupt & SERIAL_IRQ_PENDING;
        uint16_t interrupt_cause = interrupt & SERIAL_IRQ_RESET_MASK;

        while (interrupt_pending == 0) {
                switch (interrupt_cause) {
                case SERIAL_IRQ_MODEM:
                        /* Read modem status register to clear interrupt */
                        break;
                case SERIAL_IRQ_RX_READY:
                        /* Read receive buffer to clear interrupt */
                        serial_read(data->io_port, 0);
                        break;
                case SERIAL_IRQ_TX_EMPTY:
                        /* Read the interrupt identification register or write
                         * to the transmit holding buffer to clear interrupt
                         */
                        break;
                case SERIAL_IRQ_RECV_LINE:
                        /* Read line status register to clear interrupt */
                        break;
                case SERIAL_IRQ_TIMEOUT:
                        /* Read Receive buffer register to clear interrupt */
                        serial_read(data->io_port, 1);
                        break;
                default:
                        break;
                        /* A reserved case occurred,
                         * let's continue gracefully!
                         */
                }
                interrupt = inb(data->io_port + SERIAL_IRQ_CONTROL);

                interrupt_pending = interrupt & SERIAL_IRQ_PENDING;
                interrupt_cause = interrupt & SERIAL_IRQ_RESET_MASK;
        }

        return -E_SUCCESS;
}

static void drv_serial_disconnect(struct serial_port_data* port)
{
        if (port == NULL) {
                return;
        }

        mutex_lock(&port->port_lock);
        outb(port->io_port + SERIAL_IRQ_CONTROL, SERIAL_INTERRUPT_OFF);

        interrupt_deregister(X86_8259_INTERRUPT_BASE + 3, port->irq1_id);
        interrupt_deregister(X86_8259_INTERRUPT_BASE + 4, port->irq2_id);

        port->opened = 0;

        mutex_unlock(&port->port_lock);

        return;
}
static void drv_serial_connect(struct serial_port_data* port)
{
        /* Set up interrupt handlers */
        port->irq1_id = interrupt_register(X86_8259_INTERRUPT_BASE + 3,
                        serial_interrupt, port);
        port->irq2_id = interrupt_register(X86_8259_INTERRUPT_BASE + 4,
                        serial_interrupt, port);

        mutex_lock(&port->port_lock);
        /* Disable interrupts */
        outb(port->io_port + SERIAL_IRQ_CONTROL, SERIAL_INTERRUPT_OFF);

        /* Allow baud rate programming */
        outb(port->io_port + SERIAL_LINE_CONTROL, SERIAL_DLAB);

        /* Calculate baud divisor */
        uint16_t divisor = SERIAL_BAUD_BASE / port->baud;
        uint8_t low = (uint8_t) (divisor & 0xFF);
        uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);

        /* Set low byte */
        outb(port->io_port + 0, low);
        /* Set high byte */
        outb(port->io_port + 1, high);

        /* Set line control
         * This clears the DLAB (Divisor Latch Access Byte)
         */
        uint16_t line_control = port->char_len | port->parity | port->stop_char;
        outb(port->io_port + SERIAL_LINE_CONTROL, line_control);

        /*
         * Set up fifo:
         *
         * Enable FIFO
         * Clear TX
         * Clear RX
         * Trigger interrupt on 8 bytes
         */
        uint8_t fifo = SERIAL_FIFO_ENABLE | SERIAL_FIFO_CLEAR_RX
                       | SERIAL_FIFO_CLEAR_TX | SERIAL_FIFO_TRIG_8;
        outb(port->io_port + SERIAL_FIFO_REGISTER, fifo);

        /* Configure the modem */
        uint8_t modem = SERIAL_MODEM_DTR | SERIAL_MODEM_RTS | SERIAL_MODEM_OUT2;
        outb(port->io_port + SERIAL_MODEM_CONTROL, modem);

        /* Enable interrupts in the UART controller */
        uint8_t irq_config = SERIAL_INTERRUPT_TX_READY
                        | SERIAL_INTERRUPT_RX_READY | SERIAL_INTERRUPT_ERROR
                        | SERIAL_INTERRUPT_MODEM_STATE;
        outb(port->io_port + SERIAL_INTERRUPT, irq_config);

        port->opened = 1;

        mutex_unlock(&port->port_lock);
        debug("Just set up port: %X\n", port->io_port);

        return;
}

static size_t drv_serial_io_write(struct vfile* this, char* buffer,
                size_t idx __attribute__((unused)), size_t len)
{
        if (this == NULL || buffer == NULL || len == 0) {
                return 0;
        }

        struct device* dev = device_find_id(this->fs_data.device_id);
        if (dev == NULL) {
                return 0;
        }

        struct serial_port_data* data = dev->device_data;
        if (dev->device_data_size == sizeof(*data)) {
                return 0;
        }

        mutex_lock(&data->port_lock);

        /* Do the sending bits here */

        mutex_unlock(&data->port_lock);

        return 0;
}

static size_t drv_serial_io_read(struct vfile* this __attribute__((unused)),
                char* buffer __attribute__((unused)),
                size_t idx __attribute__((unused)),
                size_t len __attribute__((unused)))
{
        return 0;
}

static int drv_serial_ioctl(struct vfile* this, ioctl_t request, void* data)
{
        if (this == NULL || data == NULL) {
                return -E_NULL_PTR;
        }

        struct device* dev = this->fs_data.fs_data_struct;
        if (this->fs_data.fs_data_size != sizeof(*dev)) {
                return -E_CORRUPT;
        }
        struct serial_port_data* port = dev->device_data;
        if (dev->device_data_size != sizeof(*port)) {
                return -E_CORRUPT;
        }

        struct ioctl_serial_data* request_data = data;

        if (request >= IOCTL_UART_GET_BAUD) {
                /* Getting data, so no need to reinitialise */
                switch (request) {
                case IOCTL_UART_GET_BAUD:
                        request_data->baud = port->baud;
                        break;
                case IOCTL_UART_GET_CHARLEN:
                        request_data->charlen = port->char_len;
                        break;
                case IOCTL_UART_GET_PARITY:
                        request_data->parity = port->parity;
                        break;
                case IOCTL_UART_GET_STOPBIT:
                        request_data->stopchar = port->stop_char;
                        break;
                default:
                        return -E_INVALID_ARG;
                        break;
                }
        } else {
                /* Setting stuff, so reinitialise (if need be) */
                int status = port->port_status;
                if (status) {
                        drv_serial_disconnect(port);
                }
                switch (request) {
                case IOCTL_UART_SET_BAUD:
                        port->baud = request_data->baud;
                        break;
                case IOCTL_UART_SET_CHARLEN:
                        port->char_len = request_data->charlen;
                        break;
                case IOCTL_UART_SET_PARITY:
                        port->parity = request_data->parity;
                        break;
                case IOCTL_UART_SET_STOPBIT:
                        port->stop_char = request_data->stopchar;
                        break;
                default:
                        return -E_INVALID_ARG;
                }
                if (status) {
                        drv_serial_connect(port);
                }
        }
        return -E_SUCCESS;
}

static int drv_serial_init(struct device* this)
{
        if (this == NULL || this->device_data == NULL) {
                return -E_NULL_PTR;
        }

        if (this->driver != NULL) {
                return -E_ALREADY_INITIALISED;
        }

        this->driver = kmalloc(sizeof(*this->driver));
        if (this->driver == NULL) {
                return -E_NOMEM;
        }
        memset(this->driver, 0, sizeof(*this->driver));
        dev_setup_driver(this, drv_serial_io_read, drv_serial_io_write,
                        drv_serial_ioctl);

        return -E_SUCCESS;
}

static int drv_serial_open(struct vfile* this,
                char* path __attribute__((unused)),
                unsigned long len __attribute__((unused)))
{
        if (this == NULL) {
                return -E_NULL_PTR;
        }

        struct device* device = this->fs_data.fs_data_struct;
        if (this->fs_data.fs_data_size != sizeof(*device)) {
                return -E_CORRUPT;
        }

        struct serial_port_data* data = device->device_data;
        if (device->device_data_size != sizeof(*data)) {
                return -E_CORRUPT;
        }

        drv_serial_connect(data);

        return -E_SUCCESS;
}

static int drv_serial_close(struct vfile* this)
{
        if (this == NULL) {
                return -E_NULL_PTR;
        }

        struct device* device = this->fs_data.fs_data_struct;
        if (this->fs_data.fs_data_size != sizeof(*device)) {
                return -E_CORRUPT;
        }

        struct serial_port_data* data = device->device_data;
        if (device->device_data_size != sizeof(*data)) {
                return -E_CORRUPT;
        }

        drv_serial_disconnect(data);

        return -E_SUCCESS;
}

/**
 * \fn dev_serial_init
 * \brief Set up the device handle for the serial port
 *
 * \param parent The parent device
 * \param com_port The IO port to interface with this device
 * \param name The human readable name for the device
 *
 * \return Generic error codes
 */
static int dev_serial_init(struct device* parent, uint16_t com_port, char* name)
{
        if (parent == NULL) {
                warning("Null device as UART parent! Failed initialisation\n");
                return -E_NULL_PTR;
        }

        struct device* serial_device = kmalloc(sizeof(*serial_device));
        if (serial_device == NULL) {
                return -E_NOMEM;
        }
        memset(serial_device, 0, sizeof(*serial_device));
        /* Device is allocated, place the device data now */

        memcpy(serial_device->name, name, strlen(name) + 1);

        serial_device->lock = mutex_unlocked;
        serial_device->type = TTY;

        /* Let's do some device specific data */
        struct serial_port_data* port = kmalloc(sizeof(*port));
        if (port == NULL) {
                return -E_NOMEM;
        }
        memset(port, 0, sizeof(*port));

        /* Done allocating, let's configure */
        port->magic = SERIAL_MAGIC;
        port->io_port = com_port;
        port->baud = 9600;
        port->char_len = SERIAL_CHAR_LEN_8;
        port->stop_char = SERIAL_STOP_SHORT;
        port->parity = SERIAL_PARITY_NONE;
        port->port_lock = mutex_unlocked;
        port->port_status = 0;

        /* Configured, now attach to the device structure */
        serial_device->device_data = port;
        serial_device->device_data_size = sizeof(*port);

        /* Set up the driver data here */
        int ret = drv_serial_init(serial_device);

        /* Set up the serial connection */
        drv_serial_connect(port);

        /* Link up the appropriate device file to the internal uart data */
        port->dev_file = serial_device->driver->io;
        port->dev_file->open = drv_serial_open;
        port->dev_file->close = drv_serial_close;

        /* Hook up the device to its parent */
        device_attach(parent, serial_device);

        return ret;
}

static char* com_names[] = { "COM0", "COM1", "COM2", "COM3" };

int drv_serial_kickstart(struct device* parent)
{
        int ret = E_SUCCESS;
        int i = 0;
        uint16_t ports = 0;
#ifdef X86
        // Attempt to detect the ports through BIOS data
        uint16_t* com_port = &bda.COM0;
        for (; i < 4; i++) {
                if (com_port[i] != 0) {
                        ports++;
                        printf("Found com: %X\n", i);
                        ret |= dev_serial_init(parent, com_port[i],
                                        com_names[i]);

                }

        }
        uint16_t stat_ports[] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };
#endif
        // The ports were not found, fall back to static data
        if (ports == 0) {
                for (i = 0; i < 4; i++) {
                        ret |= dev_serial_init(parent, stat_ports[i],
                                        com_names[i]);
                }
        }

        debug("Doing serial kickstart\n");

        return -ret;
}
