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
//#define MOSI	3
//#define CLK		5
#define WF_INT	10

#define METAL_SPI_TXDATA_FULL         (1 << 31)
#define METAL_SIFIVE_SPI0_TXDATA 72UL

#define METAL_SPI_CSMODE_MASK         3
#define METAL_SPI_CSMODE_AUTO         0
#define METAL_SPI_CSMODE_HOLD         2
#define METAL_SPI_CSMODE_OFF          3

#define spi_base	0x10024000

#define _sckdiv		0x00
#define _sckmode	0x04
#define _csid		0x10
#define _csdef		0x14
#define _csmode		0x18
#define _delay0		0x28
#define _delay1		0x2C
#define _fmt		0x40
#define _txdata		0x48
#define _rxdata		0x4C
#define _txmark		0x50
#define _rxmark		0x54
#define _fctrl		0x60
#define _ffmt		0x64
#define _ie			0x70
#define _ip			0x74

struct metal_gpio* gpio1;

struct metal_spi *spi;

//const struct metal_clock *clk;

volatile uint32_t* txdata = 0x10024048;
volatile uint32_t* rxdata = 0x1002404C;
volatile uint32_t *csmode = 0x10024018;
volatile uint32_t *gpio = 0x1001200C;
volatile uint32_t* csdef = 0x10024014;
volatile uint32_t* delay0 = 0x10024028;

//volatile uint32_t *spi_sckdiv = spi_base|_sckdiv;
//volatile uint32_t *spi_sckmode = spi_base|_sckmode;
//volatile uint32_t *spi_csid = spi_base|_csid;
//volatile uint32_t *spi_csdef = spi_base|_csdef;
//volatile uint32_t *spi_csmode = spi_base|_csmode;
//volatile uint32_t *spi_delay0 = spi_base|_delay0;
//volatile uint32_t *spi_delay1 = spi_base|_delay1;
//volatile uint32_t *spi_fmt = spi_base|_fmt;
//volatile uint32_t *spi_txdata = spi_base|_txdata;
//volatile uint32_t *spi_rxdata = spi_base|_rxdata;
volatile uint32_t *spi_txmark = spi_base|_txmark;
volatile uint32_t *spi_rxmark = spi_base|_rxmark;
//volatile uint32_t *spi_fctrl = spi_base|_fctrl;
//volatile uint32_t *spi_ffmt = spi_base|_ffmt;
volatile uint32_t *spi_ie = spi_base|_ie;
volatile uint32_t *spi_ip = spi_base|_ip;

char rx_buf[32];
char cmd_len_buf[4];
char rec_cmd[] = {0x01, 0x00, 0x00, 0x00};
int16_t cmd_len;
uint32_t temp;
uint32_t timeout;

volatile uint32_t *gpio_input_en = 0x10012004;
volatile uint32_t *gpio_input_val = 0x10012000;

void delay(int number_of_seconds) //not actually number of seconds
{
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds);
//	for(int i = 0; i < number_of_seconds; i++)
//	{
//
//	}
}
char send_cmd(char* cmd);
char get_esp();
char get_data_esp();
//void spi_init();
void input_enable(char pin);
uint32_t get_input_val(char pin);

int main()
{
	printf("Start Programe1\n");
	gpio1 = metal_gpio_get_device(0);
	if(gpio1 == NULL)
	{
		printf("get gpio device failed\n");
	}

	metal_gpio_enable_output(gpio1,RED);
	metal_gpio_enable_output(gpio1,BLUE);
	metal_gpio_enable_output(gpio1,GREEN);

//	metal_gpio_disable_input(gpio1, ESP_CS);
//	metal_gpio_enable_output(gpio1, ESP_CS);

//	metal_gpio_disable_input(gpio1, WF_INT);

	metal_gpio_set_pin(gpio1, RED, 1);
	metal_gpio_set_pin(gpio1, GREEN, 1);
	metal_gpio_set_pin(gpio1, BLUE, 1);

	/* Initialize the SPI device to 2Mhz */
//	handshake();
//	metal_spi_init(spi, 100000);
//	metal_spi_set_baud_rate(spi, 100000);
//	uint32_t* abcxyz = 0x10024000;

//	*csmode = 0;
//	uint32_t* csid = 0x10024010;

//	send_cmd("AT\r\n");
//	while(!get_input_val(WF_INT));
//	get_data_esp();
	*spi_txmark = 1;
	*spi_rxmark = 0;
	*spi_ie = 3;

	input_enable(WF_INT);


	send_cmd("AT\r\n");
	while(!get_input_val(WF_INT));
	get_data_esp();

	send_cmd("AT+CWMODE=1\r\n");
	while(!get_input_val(WF_INT));
	get_data_esp();

	send_cmd("AT+CWJAP=\"SHCVN02\",\"khongduoc\"\r\n");
//	send_cmd("AT+CWJAP=\"SHCVN02\",\"khongduoc\"\r\n");
	while(!get_input_val(WF_INT));
	get_data_esp();
	while(!get_input_val(WF_INT));
	get_data_esp();
	while(!get_input_val(WF_INT));
	get_data_esp();
//	send_cmd("AT+CIPSNTPCFG=1,1,\"sg.pool.ntp.org\"\r\n");
//	while(!get_input_val(WF_INT));
//	if(get_esp())
//	{
//		printf("OK\n");
//	}
	while(1)
	{
		while(send_cmd("AT+CIPSTART=\"TCP\",\"hifivedemo.s3-ap-southeast-1.amazonaws.com\",80,2\r\n"));
		timeout = 0;
		printf("abcxyz\n");
		while(!get_input_val(WF_INT))
		{
			delay(2000);
			timeout++;
			if(timeout > 100)
			{
				goto X;
			}
		}
		get_data_esp();
		while(!get_input_val(WF_INT));
		get_data_esp();

		send_cmd("AT+CIPSEND=76\r\n");
		timeout = 0;
		while(!get_input_val(WF_INT))
		{
			delay(2000);
			timeout++;
			if(timeout > 100)
			{
				goto X;
			}
		}
		get_data_esp();
		timeout = 0;
		while(!get_input_val(WF_INT))
		{
			delay(200);
			timeout++;
			if(timeout > 100)
			{
				goto X;
			}
		}
		get_data_esp();
		send_cmd("GET /data.txt HTTP/1.1\r\nHost: hifivedemo.s3-ap-southeast-1.amazonaws.com\r\n\r\n");
		printf("get data 1\n");
		timeout = 0;
		while(!get_input_val(WF_INT))
		{
			delay(200);
			timeout++;
			if(timeout > 100)
			{
				goto X;
			}
		}
		get_data_esp();
		printf("get data 2\n");
		timeout = 0;
		while(!get_input_val(WF_INT))
		{
			delay(200);
			timeout++;
			if(timeout > 100)
			{
				goto X;
			}
		}
		get_data_esp();
		printf("get data 3\n");
		timeout = 0;
		while(!get_input_val(WF_INT))
		{
			delay(2000);
			timeout++;
			if(timeout > 100)
			{
				goto X;
			}
		}
		switch (get_esp())
		{
			case 0:
			{
				printf("get data 4\n");
				switch (get_data_esp())
				{
					case 2:
					{
						metal_gpio_set_pin(gpio1, RED, 0);
						metal_gpio_set_pin(gpio1, GREEN, 1);
						metal_gpio_set_pin(gpio1, BLUE, 1);
						break;
					}
					case 3:
					{
						metal_gpio_set_pin(gpio1, RED, 1);
						metal_gpio_set_pin(gpio1, GREEN, 0);
						metal_gpio_set_pin(gpio1, BLUE, 1);
						break;
					}
					case 4:
					{
						metal_gpio_set_pin(gpio1, RED, 1);
						metal_gpio_set_pin(gpio1, GREEN, 1);
						metal_gpio_set_pin(gpio1, BLUE, 0);
						break;
					}
				}
				break;
			}
			case 1:
			{
				metal_gpio_set_pin(gpio1, RED, 0);
				metal_gpio_set_pin(gpio1, GREEN, 1);
				metal_gpio_set_pin(gpio1, BLUE, 1);
				break;
			}
			case 2: {
				metal_gpio_set_pin(gpio1, RED, 1);
				metal_gpio_set_pin(gpio1, GREEN, 0);
				metal_gpio_set_pin(gpio1, BLUE, 1);
				break;
			}
			case 3: {
				metal_gpio_set_pin(gpio1, RED, 1);
				metal_gpio_set_pin(gpio1, GREEN, 1);
				metal_gpio_set_pin(gpio1, BLUE, 0);
				break;
			}
		}
//		timeout = 0;
//		while(!get_input_val(WF_INT))
//		{
//			delay(500);
//			timeout++;
//			if(timeout > 100)
//			{
//				goto X;
//			}
//		}

		printf("finish getting data\n");
		while(send_cmd("AT+CIPCLOSE\r\n"));
		timeout = 0;
		while(!get_input_val(WF_INT));

		if(get_data_esp() != 1)
		{
			if(0)
			{
X:				printf("TIMEOUT\n");
			}
			while(!get_input_val(WF_INT));
			get_data_esp();
		}
	}
}

char send_cmd(char* cmd)
{
	int len = strlen(cmd);
	char abc[] = {0x02, 0x00, 0x00, 0x00};
	char xyz[] = {len, 0x00, 0x00, 0x41};
	printf("%s\n", cmd);
	/* send 2-0-0-0 */
	*csmode = 2;
	*delay0 = (*delay0 & 0xFF00FFFF);
	for(int i = 0; i<4; i ++)
	{
		*txdata = abc[i];
		temp = (*rxdata);
	}
	while(!(*spi_ip & 1));
	*csmode = 0;
	delay(500);

	*csmode = 2;
	*delay0 = (*delay0 & 0xFF00FFFF);
	while(!get_input_val(WF_INT));
	/* send length */
	for(int i = 0; i<4; i ++)
	{
		*txdata = xyz[i];
		temp = (*rxdata);
		while(!(*spi_ip & 1));
	}
	*csmode = 0;

	delay(100);
	timeout = 0;
	while(!get_input_val(WF_INT))
	{
		delay(10);
		if(timeout++>1000)
		{
			printf("co timeout\n");
			return 1;
		}
	}

	*delay0 = (*delay0 & 0xFF00FFFF);
	*csmode = 2;
	for (int i = 0; i < len; i ++)
	{
		*txdata = cmd[i];
//		while (*txdata & METAL_SPI_TXDATA_FULL);
		temp = (*rxdata);
//		while (temp & METAL_SPI_TXDATA_FULL)
//		{
//			temp = (*rxdata);
//			if(!get_input_val(WF_INT))
//			{
//				*csmode = 0;
//				break;
//			}
//		}
		while(!(*spi_ip & 1));
	}
	*csmode = 0;
	delay(10000);
	return 0;
}
char get_esp()
{

	cmd_len = 0;
	memset(rx_buf, 0, 32);
	*delay0 = (*delay0 & 0xFF00FFFF);

	while((*spi_ip & 2) == 2)
	{
		temp = *rxdata;
		printf("abc\n");
	}
	*csmode = 2;
	for(int i = 0; i < 4; i++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = rec_cmd[i];
	}
	while(!(*spi_ip & 1));
	*csmode = 0;

	while((*spi_ip & 2) == 2)
	{
		temp = *rxdata;
		printf("[FIFO data]: %d\n", temp);
	}

	while(!get_input_val(WF_INT));
	delay(100);

	*delay0 = (*delay0 & 0xFF00FFFF);
	*csmode = 2;
	for(int i = 0; i < 4; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = 0;
		while(!(*spi_ip & 1));
	}
	*csmode = 0;
	while(!get_input_val(WF_INT));
	delay(100);

	char count = 0;
	while((*spi_ip & 2) == 2)
	{
		rx_buf[count] = (*rxdata);
		printf("[FIFO data]: %d\n", rx_buf[count]);
		count++;
	}
	cmd_len = rx_buf[0] + rx_buf[1]*128;
	printf("[0]: %d,[1]: %d,[2]: %d,[3]: %d\n", rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3]);
	printf("[lenght]: %d\n", cmd_len);
	delay(10);
	*delay0 = (*delay0 & 0xFF00FFFF);
	*csmode = 2;
//	char keepdata = 0;
	for(int i = 0; i < cmd_len - 4; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = 0;
	}
	while(!(*spi_ip & 1));
	while((*spi_ip & 2) == 2)
	{
		temp = *rxdata;
		printf("abc\n");
	}
	for(int i = 0; i < 4; i++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = 0;
	}
	while(!(*spi_ip & 1));
	*csmode = 0;
	memset(rx_buf, 0, 32);
	count = 0;
	while((*spi_ip & 2) == 2)
	{
		rx_buf[count] = (*rxdata);
		printf("[FIFO data]: %d\n", rx_buf[count]);
		count++;
	}
	if(strcmp(rx_buf, "\r\n\r\n") == 0)
	{
		printf("[test]: detect/r/n\n");
		return 0;
	}
	else if(strcmp(rx_buf, "LEDR") == 0)
	{
		printf("[test]: detect LEDR\n");
		return 1;
	}
	else if(strcmp(rx_buf, "LEDG") == 0)
	{
		printf("[test]: detect LEDG\n");
		return 2;
	}
	else if(strcmp(rx_buf, "LEDB") == 0)
	{
		printf("[test]: detect LEDB\n");
		return 3;
	}
	return 0;
}
char get_data_esp()
{
	cmd_len = 0;
	memset(rx_buf, 0, 32);
	*delay0 = (*delay0 & 0xFF00FFFF);

	/* read data from FIFO to reset FIFO */
	while((*spi_ip & 2) == 2)
	{
		temp = *rxdata;
		while ((temp & METAL_SPI_TXDATA_FULL) == METAL_SPI_TXDATA_FULL)
		{
			temp = *rxdata;
		}
	}

	/* start to send 1-0-0-0 */
	*csmode = 2;
	for(int i = 0; i < 4; i++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = rec_cmd[i];
	}
	while(!(*spi_ip & 1));
	*csmode = 0;

	/* start to read FIFO data to reset FIFO */
	while((*spi_ip & 2) == 2)
	{
		temp = *rxdata;
		while ((temp & METAL_SPI_TXDATA_FULL) == METAL_SPI_TXDATA_FULL)
		{
			temp = *rxdata;
		}
		printf("[FIFO]: 0x%02x\n", temp);
	}
	/* end to read FIFO data */

	while(!get_input_val(WF_INT));
	delay(100);

	/* start to send 0-0-0-0 to get length */
	while(!(*spi_ip & 1));
	*csmode = 2;
	for(int i = 0; i < 4; i ++)
	{
		*txdata = 0;
		while(!(*spi_ip & 1));
	}
	*csmode = 0;

	/* get length data from FIFO */
	char count = 0;
	while((*spi_ip & 2) == 2)
	{
		rx_buf[count] = (*rxdata);
		while ((rx_buf[count] & METAL_SPI_TXDATA_FULL))
		{
			rx_buf[count] = (*rxdata);
		}
		printf("[FIFO]: 0x%02x\n", rx_buf[count]);
		count++;
	}
	printf("[debug]: count: %d\n", count);
	if(rx_buf[3] == 66)
	{
		cmd_len = rx_buf[0] + rx_buf[1]*128;
		printf("[length]: %d\n", cmd_len);
	}
	else
	{
		printf("[ERROR]: get length failed\n");
		printf("[0]: %d,[1]: %d,[2]: %d,[3]: %d, [4]: %d\n", rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3], rx_buf[4]);
		return -1;
	}

	memset(rx_buf, 0, 32);
	while(!get_input_val(WF_INT));

	delay(100);

	*delay0 = (*delay0 & 0xFF00FFFF);
	*csmode = 2;
	for(int i = 0; i < cmd_len - 4; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = 0;
	}
	while(!(*spi_ip & 1));
	while((*spi_ip & 2) == 2)
	{
		temp = *rxdata;
		printf("abc\n");
	}
	for(int i = 0; i < 4; i++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = 0;
	}
	while(!(*spi_ip & 1));
	*csmode = 0;
	memset(rx_buf, 0, 32);
	count = 0;
	while((*spi_ip & 2) == 2)
	{
		rx_buf[count] = (*rxdata);
		printf("[FIFO data]: %d\n", rx_buf[count]);
		count++;
	}

	if(strcmp(rx_buf, "LEDR") == 0)
	{
		printf("[test]: detect LEDR\n");
		return 2;
	}
	else if(strcmp(rx_buf, "LEDG") == 0)
	{
		printf("[test]: detect LEDG\n");
		return 3;
	}
	else if(strcmp(rx_buf, "LEDB") == 0)
	{
		printf("[test]: detect LEDB\n");
		return 4;
	}
	printf("[Messgase]: %s\n", rx_buf);
	return 0;
}
void input_enable(char pin)
{
	*gpio_input_en |= 1<<pin;
	printf("input_en register value: 0x%04x\n", *gpio_input_en);
}
uint32_t get_input_val(char pin)
{
	uint32_t mask =  1<<pin;
	uint32_t val = (*gpio_input_val & mask) << pin;
	return val;
}
