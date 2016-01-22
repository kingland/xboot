/*
 * bus/pl011-uart.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <pl011-uart.h>

#define UART_DATA		(0x00)
#define UART_RSR		(0x04)
#define UART_FR			(0x18)
#define UART_ILPR		(0x20)
#define UART_IBRD		(0x24)
#define UART_FBRD		(0x28)
#define UART_LCRH		(0x2c)
#define UART_CR			(0x30)
#define UART_IFLS		(0x34)
#define UART_IMSC		(0x38)
#define UART_RIS		(0x3c)
#define UART_MIS		(0x40)
#define UART_ICR		(0x44)
#define UART_DMACR		(0x48)

#define UART_RSR_OE		(0x1 << 3)
#define UART_RSR_BE		(0x1 << 2)
#define UART_RSR_PE		(0x1 << 1)
#define UART_RSR_FE		(0x1 << 0)

#define UART_FR_TXFE	(0x1 << 7)
#define UART_FR_RXFF	(0x1 << 6)
#define UART_FR_TXFF	(0x1 << 5)
#define UART_FR_RXFE	(0x1 << 4)
#define UART_FR_BUSY	(0x1 << 3)

struct pl011_uart_private_data_t {
	char * clk;
	int txdpin;
	int txdcfg;
	int rxdpin;
	int rxdcfg;
	enum baud_rate_t baud;
	enum data_bits_t data;
	enum parity_bits_t parity;
	enum stop_bits_t stop;
	virtual_addr_t regbase;
};

static bool_t pl011_uart_setup(struct uart_t * uart, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop)
{
	struct pl011_uart_private_data_t * dat = (struct pl011_uart_private_data_t *)uart->priv;
	u32_t ibaud, divider, remainder, fraction;
	u8_t dreg, preg, sreg;
	u64_t uclk;

	switch(baud)
	{
	case B50:
		ibaud = 50;
		break;
	case B75:
		ibaud = 75;
		break;
	case B110:
		ibaud = 110;
		break;
	case B134:
		ibaud = 134;
		break;
	case B200:
		ibaud = 200;
		break;
	case B300:
		ibaud = 300;
		break;
	case B600:
		ibaud = 600;
		break;
	case B1200:
		ibaud = 1200;
		break;
	case B1800:
		ibaud = 1800;
		break;
	case B2400:
		ibaud = 2400;
		break;
	case B4800:
		ibaud = 4800;
		break;
	case B9600:
		ibaud = 9600;
		break;
	case B19200:
		ibaud = 19200;
		break;
	case B38400:
		ibaud = 38400;
		break;
	case B57600:
		ibaud = 57600;
		break;
	case B76800:
		ibaud = 76800;
		break;
	case B115200:
		ibaud = 115200;
		break;
	case B230400:
		ibaud = 230400;
		break;
	case B380400:
		ibaud = 380400;
		break;
	case B460800:
		ibaud = 460800;
		break;
	case B921600:
		ibaud = 921600;
		break;
	default:
		return FALSE;
	}

	switch(data)
	{
	case DATA_BITS_5:
		dreg = 0x0;
		break;
	case DATA_BITS_6:
		dreg = 0x1;
		break;
	case DATA_BITS_7:
		dreg = 0x2;
		break;
	case DATA_BITS_8:
		dreg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case PARITY_NONE:
		preg = 0x0;
		break;
	case PARITY_EVEN:
		preg = 0x3;
		break;
	case PARITY_ODD:
		preg = 0x1;
		break;
	default:
		return FALSE;
	}

	switch(stop)
	{
	case STOP_BITS_1:
		sreg = 0;
		break;
	case STOP_BITS_2:
		sreg = 1;
		break;
	case STOP_BITS_1_5:
	default:
		return FALSE;
	}

	dat->baud = baud;
	dat->data = data;
	dat->parity = parity;
	dat->stop = stop;

	/*
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = ROUND((64 * MOD(UART_CLK, (16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	uclk = clk_get_rate(dat->clk);
	divider = uclk / (16 * ibaud);
	remainder = uclk % (16 * ibaud);
	fraction = (8 * remainder / ibaud) >> 1;
	fraction += (8 * remainder / ibaud) & 1;

	write32(dat->regbase + UART_IBRD, divider);
	write32(dat->regbase + UART_FBRD, fraction);
	write32(dat->regbase + UART_LCRH, (1 << 4) | (dreg<<5 | sreg<<3 | preg<<1));
	return TRUE;
}

static void pl011_uart_init(struct uart_t * uart)
{
	struct pl011_uart_private_data_t * dat = (struct pl011_uart_private_data_t *)uart->priv;

	clk_enable(dat->clk);
	if(dat->txdpin >= 0)
	{
		gpio_set_cfg(dat->txdpin, dat->txdcfg);
		gpio_set_pull(dat->txdpin, GPIO_PULL_UP);
	}
	if(dat->rxdpin >= 0)
	{
		gpio_set_cfg(dat->rxdpin, dat->rxdcfg);
		gpio_set_pull(dat->rxdpin, GPIO_PULL_UP);
	}
	pl011_uart_setup(uart, dat->baud, dat->data, dat->parity, dat->stop);
	write32(dat->regbase + UART_CR, 0x0);
	write32(dat->regbase + UART_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

static void pl011_uart_exit(struct uart_t * uart)
{
	struct pl011_uart_private_data_t * dat = (struct pl011_uart_private_data_t *)uart->priv;

	write32(dat->regbase + UART_CR, 0x0);
	clk_disable(dat->clk);
}

static ssize_t pl011_uart_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct pl011_uart_private_data_t * dat = (struct pl011_uart_private_data_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( !(read8(dat->regbase + UART_FR) & UART_FR_RXFE) )
			buf[i] = read8(dat->regbase + UART_DATA);
		else
			break;
	}

	return i;
}

static ssize_t pl011_uart_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct pl011_uart_private_data_t * dat = (struct pl011_uart_private_data_t *)uart->priv;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( (read8(dat->regbase + UART_FR) & UART_FR_TXFF) );
		write8(dat->regbase + UART_DATA, buf[i]);
	}

	return i;
}

static bool_t pl011_register_bus_uart(struct resource_t * res)
{
	struct pl011_uart_data_t * rdat = (struct pl011_uart_data_t *)res->data;
	struct pl011_uart_private_data_t * dat;
	struct uart_t * uart;
	char name[64];

	if(!clk_search(rdat->clk))
		return FALSE;

	dat = malloc(sizeof(struct pl011_uart_private_data_t));
	if(!dat)
		return FALSE;

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->clk = strdup(rdat->clk);
	dat->txdpin = rdat->txdpin;
	dat->txdcfg = rdat->txdcfg;
	dat->rxdpin = rdat->rxdpin;
	dat->rxdcfg = rdat->rxdcfg;
	dat->baud = rdat->baud;
	dat->data = rdat->data;
	dat->parity = rdat->parity;
	dat->stop = rdat->stop;
	dat->regbase = phys_to_virt(rdat->regbase);

	uart->name = strdup(name);
	uart->init = pl011_uart_init;
	uart->exit = pl011_uart_exit;
	uart->read = pl011_uart_read;
	uart->write = pl011_uart_write;
	uart->setup = pl011_uart_setup;
	uart->priv = dat;

	if(register_bus_uart(uart))
		return TRUE;

	free(dat->clk);
	free(uart->priv);
	free(uart->name);
	free(uart);
	return FALSE;
}

static bool_t pl011_unregister_bus_uart(struct resource_t * res)
{
	struct pl011_uart_private_data_t * dat;
	struct uart_t * uart;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart = search_bus_uart(name);
	if(!uart)
		return FALSE;
	dat = (struct pl011_uart_private_data_t *)uart->priv;

	if(!unregister_bus_uart(uart))
		return FALSE;

	free(dat->clk);
	free(uart->priv);
	free(uart->name);
	free(uart);
	return TRUE;
}

static __init void pl011_bus_uart_init(void)
{
	resource_for_each_with_name("pl011-uart", pl011_register_bus_uart);
}

static __exit void pl011_bus_uart_exit(void)
{
	resource_for_each_with_name("pl011-uart", pl011_unregister_bus_uart);
}

bus_initcall(pl011_bus_uart_init);
bus_exitcall(pl011_bus_uart_exit);
