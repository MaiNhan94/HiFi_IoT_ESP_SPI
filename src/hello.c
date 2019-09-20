/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <metal/led.h>
#include <metal/gpio.h>
#include <metal/timer.h>
#include <metal/spi.h>
#include <metal/clock.h>

#define ESP_CS	9
#define BLUE	21
#define GREEN	19
#define RED		22
#define MOSI	3
#define CLK		5

#define METAL_SPI_TXDATA_FULL         (1 << 31)
#define METAL_SIFIVE_SPI0_TXDATA 72UL

#define METAL_SPI_CSMODE_MASK         3
#define METAL_SPI_CSMODE_AUTO         0
#define METAL_SPI_CSMODE_HOLD         2
#define METAL_SPI_CSMODE_OFF          3

struct metal_gpio* gpio1;

struct metal_spi *spi;

const struct metal_clock *clk;

volatile uint32_t* txdata = 0x10024048;
volatile uint32_t* rxdata = 0x1002404C;
volatile uint32_t *csmode = 0x10024018;
volatile uint32_t *gpio = 0x1001200C;
volatile uint32_t* csdef = 0x10024014;
volatile uint32_t* delay0 = 0x10024028;

void delay(int number_of_seconds) //not actually number of seconds
{
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds);
}
void send_cmd(char* cmd);
void get_esp();
void handshake();


int main()
{
	printf("Start Programe\n");
	printf("clock rate: %d\n", clock());
	gpio1 = metal_gpio_get_device(0);
	if(gpio1 == NULL)
	{
		printf("get gpio device failed\n");
	}
//	LED_RED = metal_gpio_get_device(RED);
//	LED_BLUE = metal_gpio_get_device(BLUE);
//	LED_GREEN = metal_gpio_get_device(GREEN);

	spi = metal_spi_get_device(0);
	if(spi == NULL)
	{
		printf("Failed to get spi device\n");
		return 1;
	}
	else
	{
		printf("success to get spi device\n");
	}

//	metal_gpio_enable_output(gpio1,ESP_CS);
	metal_gpio_enable_output(gpio1,RED);
	metal_gpio_enable_output(gpio1,BLUE);
	metal_gpio_enable_output(gpio1,GREEN);

	metal_gpio_set_pin(gpio1, ESP_CS, 1);
	metal_gpio_set_pin(gpio1, RED, 0);
	metal_gpio_set_pin(gpio1, GREEN, 1);
	metal_gpio_set_pin(gpio1, BLUE, 1);

	/* Initialize the SPI device to 2Mhz */
//	handshake();
//	metal_spi_init(spi, 100000);
//	metal_spi_set_baud_rate(spi, 100000);
//	uint32_t* abcxyz = 0x10024000;
	*csmode = 0;
//	*abcxyz = 0x80000050;
	uint32_t* csid = 0x10024010;
//	*csid = 0;
	printf("csid: 0x%02x\n", *csid);

	printf("csdef: 0x%02x\n", *csdef);
//	*csdef = 11;
	printf("baud rate: %d\n", metal_spi_get_baud_rate(spi));
	/* CPOL = 0, CPHA = 0, MSB-first, CS active low */
//	struct metal_spi_config config = {
//		.protocol = METAL_SPI_SINGLE,
//		.polarity = 0,
//		.phase = 0,
//		.little_endian = 0,
//		.cs_active_high = 0,
//		.csid = 0,
//	};
	char* cmd = "AT+RST\r\n";
	send_cmd(cmd);
	metal_gpio_set_pin(gpio1, ESP_CS, 0);
	get_esp();
	metal_gpio_set_pin(gpio1, ESP_CS, 1);
	printf("Send data via SPI bus\n");
	delay(10000);

	while(1)
	{
		metal_gpio_set_pin(gpio1, GREEN, 1);
		delay(10000);
		metal_gpio_set_pin(gpio1, GREEN, 0);
		delay(10000);
	}

}

void send_cmd(char* cmd)
{
	int len = strlen(cmd);
	printf("delay0: 0x%02x\n", (*delay0>>16)&0xFF);
	*delay0 = (*delay0&0xFF00FFFF);
	printf("delay0: 0x%02x\n", (*delay0>>16)&0xFF);
	char abc[] = {0x02, 0x00, 0x00, 0x00};
	char xyz[4];// = {len, 0x00, 0x00, 0x41};
	xyz[0] = len;
	xyz[1] = 0;
	xyz[2] = 0;
	xyz[3] = 0x41;

	*csmode = 2;
	//*csdef = 0x0e;//0111
	for(int i = 0; i<4; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = abc[i];
//		memcpy((void*)(0x10024048), (abc + i), 1);

		while (*rxdata & METAL_SPI_TXDATA_FULL);
	}
//	*csdef = 0x0f;
	*csmode = 0;


	delay(50);
	*csmode = 2;
//	*csdef = 0x0e;
	for(int i = 0; i<4; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = xyz[i];
//		memcpy((void*)(0x10024048), (xyz + i), 1);
		while (*rxdata & METAL_SPI_TXDATA_FULL);

	}
//	*csdef = 0x0f;
	*csmode = 0;

	delay(50);

	*csmode = 2;
//	*csdef = 0x0e;
	for (int i = 0; i < len; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = cmd[i];
//		memcpy((void*)(0x10024048), cmd + i, 1);
		while (*rxdata & METAL_SPI_TXDATA_FULL);
	}
//	*csdef = 0x0f;
	*csmode = 0;
}
void get_esp()
{
	char abc[] = {0x01, 0x00, 0x00, 0x00};
//	metal_gpio_set_pin(gpio1, ESP_CS, 0);
	for(int i = 0; i<4; i ++)
	{
		memcpy((void*)(0x10024048), (abc + i), 1);
	}
//	metal_gpio_set_pin(gpio1, ESP_CS, 1);

//	metal_gpio_set_pin(gpio1, ESP_CS, 0);
	for(int i = 0; i < 128; i ++)
	{
		memset((void*)(0x10024048), 0x00, 1);
	}
//	metal_gpio_set_pin(gpio1, ESP_CS, 1);
}

void handshake()
{
	metal_gpio_enable_output(gpio1,MOSI);
	metal_gpio_enable_output(gpio1,CLK);
	metal_gpio_set_pin(gpio1, MOSI, 0);
	metal_gpio_set_pin(gpio1, CLK, 1);
	delay(10000);
	metal_gpio_set_pin(gpio1, MOSI, 1);
	delay(3);
	metal_gpio_set_pin(gpio1, MOSI, 0);
	metal_gpio_set_pin(gpio1, CLK, 0);
	delay(5);
	metal_gpio_set_pin(gpio1, MOSI, 1);
	metal_gpio_set_pin(gpio1, CLK, 1);
}
