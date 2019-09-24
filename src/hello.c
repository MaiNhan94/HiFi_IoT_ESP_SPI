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
volatile uint32_t *spi_csmode = spi_base|_csmode;
volatile uint32_t *spi_delay0 = spi_base|_delay0;
//volatile uint32_t *spi_delay1 = spi_base|_delay1;
volatile uint32_t *spi_fmt = spi_base|_fmt;
volatile uint32_t *spi_txdata = spi_base|_txdata;
volatile uint32_t *spi_rxdata = spi_base|_rxdata;
//volatile uint32_t *spi_txmark = spi_base|_txmark;
//volatile uint32_t *spi_rxmark = spi_base|_rxmark;
//volatile uint32_t *spi_fctrl = spi_base|_fctrl;
//volatile uint32_t *spi_ffmt = spi_base|_ffmt;
//volatile uint32_t *spi_ie = spi_base|_ie;
//volatile uint32_t *spi_ip = spi_base|_ip;


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
void send_cmd(char* cmd);
char get_esp();
char get_data_esp();
//void spi_init();
void input_enable(char pin);
uint32_t get_input_val(char pin);

int main()
{
	printf("Start Programe\n");
	printf("clock rate: %d\n", clock());
	input_enable(WF_INT);
	gpio1 = metal_gpio_get_device(0);
	if(gpio1 == NULL)
	{
		printf("get gpio device failed\n");
	}
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

	metal_gpio_enable_output(gpio1,RED);
	metal_gpio_enable_output(gpio1,BLUE);
	metal_gpio_enable_output(gpio1,GREEN);

//	metal_gpio_disable_input(gpio1, ESP_CS);
//	metal_gpio_enable_output(gpio1, ESP_CS);

	metal_gpio_set_pin(gpio1, RED, 1);
	metal_gpio_set_pin(gpio1, GREEN, 1);
	metal_gpio_set_pin(gpio1, BLUE, 1);

	/* Initialize the SPI device to 2Mhz */
//	handshake();
//	metal_spi_init(spi, 100000);
//	metal_spi_set_baud_rate(spi, 100000);
//	uint32_t* abcxyz = 0x10024000;

	*csmode = 0;
//	uint32_t* csid = 0x10024010;

	send_cmd("AT\r\n");
	while(!get_input_val(WF_INT));
	if(get_data_esp())
	{
		printf("OK\n");
	}
	send_cmd("AT\r\n");
	while(!get_input_val(WF_INT));
	if(get_data_esp())
	{
		printf("OK\n");
	}
	send_cmd("AT+CWMODE=1\r\n");
	if(get_data_esp())
	{
		printf("OK\n");
	}

	send_cmd("AT+CWJAP=\"MyWiFi\",\"99999999\"\r\n");
	while(!get_input_val(WF_INT));
	if(get_data_esp())
	{
		printf("OK\n");
	}
	while(!get_input_val(WF_INT));
	if(get_data_esp())
	{
		printf("OK\n");
	}
	while(!get_input_val(WF_INT));
	if(get_data_esp())
	{
		printf("OK\n");
	}
//	send_cmd("AT+CIPSNTPCFG=1,1,\"sg.pool.ntp.org\"\r\n");
//	while(!get_input_val(WF_INT));
//	if(get_esp())
//	{
//		printf("OK\n");
//	}
	while(1)
	{
		send_cmd("AT+CIPSTART=\"TCP\",\"hifivedemo.s3-ap-southeast-1.amazonaws.com\",80\r\n");
		while(!get_input_val(WF_INT));
		if(get_data_esp())
		{
			printf("OK\n");
		}
		while(!get_input_val(WF_INT));
		if(get_data_esp())
		{
			printf("OK\n");
		}

		send_cmd("AT+CIPSEND=76\r\n");
		while(!get_input_val(WF_INT));
		if(get_data_esp())
		{
			printf("OK\n");
		}
		while(!get_input_val(WF_INT));
		if(get_data_esp())
		{
			printf("OK\n");
		}
		send_cmd("GET /data.txt HTTP/1.1\r\nHost: hifivedemo.s3-ap-southeast-1.amazonaws.com\r\n\r\n");
		printf("get data 1\n");
		while(!get_input_val(WF_INT));
		if(get_data_esp())
		{
			printf("OK\n");
		}
		printf("get data 2\n");
		while(!get_input_val(WF_INT));
		if(get_data_esp())
		{
			printf("OK\n");
		}
		printf("get data 3\n");
		while(!get_input_val(WF_INT));
		if(get_esp())
		{
			printf("OK\n");
		}
		while(!get_input_val(WF_INT));
		printf("get data 4\n");
	//	get_data_esp();
		switch (get_data_esp())
		{
			case 2:
			{
				metal_gpio_set_pin(gpio1, GREEN, 1);
				metal_gpio_set_pin(gpio1, BLUE, 1);
				metal_gpio_set_pin(gpio1, RED, 0);
				break;
			}
			case 3:
			{
				metal_gpio_set_pin(gpio1, GREEN, 0);
				metal_gpio_set_pin(gpio1, BLUE, 1);
				metal_gpio_set_pin(gpio1, RED, 1);
				break;
			}
			case 4:
			{
				metal_gpio_set_pin(gpio1, GREEN, 1);
				metal_gpio_set_pin(gpio1, BLUE, 0);
				metal_gpio_set_pin(gpio1, RED, 1);
				break;
			}
		}
		printf("finish getting data\n");
		send_cmd("AT+CIPCLOSE\r\n");
		while(!get_input_val(WF_INT));
		if(get_data_esp() != 1)
		{
			printf("OK\n");
			while(!get_input_val(WF_INT));
			get_data_esp();
		}

	}
}

void send_cmd(char* cmd)
{
	int len = strlen(cmd);
	uint32_t temp;
	char abc[] = {0x02, 0x00, 0x00, 0x00};
	char xyz[] = {len, 0x00, 0x00, 0x41};
	printf("%s\n", cmd);
	while(!get_input_val(WF_INT))
	{
		*csmode = 2;
		*delay0 = (*delay0 & 0xFF00FFFF);
		for(int i = 0; i<4; i ++)
		{
			while (*txdata & METAL_SPI_TXDATA_FULL);
			*txdata = abc[i];
			temp = (*rxdata);
			while (temp & METAL_SPI_TXDATA_FULL)
			{
				temp = (*rxdata);
			}
		}
		*csmode = 0;
		delay(500);
	}

	*csmode = 2;
	*delay0 = (*delay0 & 0xFF00FFFF);
	while(!get_input_val(WF_INT));
	for(int i = 0; i<4; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = xyz[i];
		temp = (*rxdata);
		while (temp & METAL_SPI_TXDATA_FULL)
		{
			if(!get_input_val(WF_INT))
			{
				*csmode = 0;
				*delay0 = (*delay0 & 0xFF00FFFF);
			}
			temp = (*rxdata);
		}
	}
	*csmode = 0;
	while(!get_input_val(WF_INT));
	delay(100);
	*delay0 = (*delay0 & 0xFF00FFFF);
	*csmode = 2;
	for (int i = 0; i < len; i ++)
	{
		*txdata = cmd[i];
		while (*txdata & METAL_SPI_TXDATA_FULL);
		temp = (*rxdata);
		while (temp & METAL_SPI_TXDATA_FULL)
		{
			temp = (*rxdata);
			if(!get_input_val(WF_INT))
			{
				*csmode = 0;
				break;
			}
		}
	}
	*csmode = 0;
	delay(10000);
}
char get_esp()
{
	uint32_t rx_buf[16];

	int16_t cmd_len;
	char rec_cmd[] = {0x01, 0x00, 0x00, 0x00};
	memset(rx_buf, 0, 32);
	*delay0 = (*delay0 & 0xFF00FFFF);
	*spi_csmode = 2;
	for(int i = 0; i < 4; i++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = rec_cmd[i];
		rx_buf[i] = (*rxdata);
		while (rx_buf[i] & METAL_SPI_TXDATA_FULL)
		{
			if(!get_input_val(WF_INT)) *csmode = 0;
			rx_buf[i] = (*rxdata);
		}
	}
	*spi_csmode = 0;
	while(!get_input_val(WF_INT));
	delay(10);

	memcpy(rx_buf, rxdata, 4);

	*delay0 = (*delay0 & 0xFF00FFFF);
	*spi_csmode = 2;
	for(int i = 0; i < 4; i ++)
	{
		while (*txdata & METAL_SPI_TXDATA_FULL);
		*txdata = 0x00;
		rx_buf[i] = (*rxdata);
		while (rx_buf[i] & METAL_SPI_TXDATA_FULL)
		{
			rx_buf[i] = (*rxdata);
			if(!get_input_val(WF_INT))
			{
				*csmode = 0;
				*delay0 = (*delay0 & 0xFF00FFFF);
			}
		}
	}
	*spi_csmode = 0;

	if(rx_buf[3] == 'B')
	{
		cmd_len = rx_buf[0] + rx_buf[1]*128;
	}
	else
	{
		return 0;
	}
	memset(rx_buf, 0, 32);
	while(!get_input_val(WF_INT));
	delay(10);
	*delay0 = (*delay0 & 0xFF00FFFF);
	*spi_csmode = 2;
	uint16_t i = 0;
	uint32_t temp;
	while(get_input_val(WF_INT))
	{
		*txdata = i;
		temp = (*rxdata);
		while ((temp & METAL_SPI_TXDATA_FULL))
		{
			temp = (*rxdata);
			if(!get_input_val(WF_INT))
			{
				*csmode = 0;
				*delay0 = (*delay0 & 0xFF00FFFF);
			}
		}
		i++;
	}
	*spi_csmode = 0;
}
char get_data_esp()
{
	uint32_t rx_buf[32];
	char cmd_len_buf[4];
	int16_t cmd_len;
	char rec_cmd[] = {0x01, 0x00, 0x00, 0x00};
	memset(rx_buf, 0, 32);
	*delay0 = (*delay0 & 0xFF00FFFF);
	*spi_csmode = 2;
	for(int i = 0; i < 4; i++)
	{
		*txdata = rec_cmd[i];
		while (*txdata & METAL_SPI_TXDATA_FULL);
		rx_buf[i] = (*rxdata);
		while (rx_buf[i] & METAL_SPI_TXDATA_FULL)
		{
			rx_buf[i] = (*rxdata);
			if(!get_input_val(WF_INT))
			{
				*csmode = 0;
				*delay0 = (*delay0 & 0xFF00FFFF);
			}
		}
	}
	*spi_csmode = 0;
	while(!get_input_val(WF_INT));
	delay(100);

	memcpy(rx_buf, rxdata, 4);

	*delay0 = (*delay0 & 0xFF00FFFF);
	*spi_csmode = 2;
	for(int i = 0; i < 4; i ++)
	{
		*txdata = 0x00;
		while (*txdata & METAL_SPI_TXDATA_FULL);
		rx_buf[i] = (*rxdata);
		while (rx_buf[i] & METAL_SPI_TXDATA_FULL)
		{
			rx_buf[i] = (*rxdata);
			if(!get_input_val(WF_INT))
			{
				*csmode = 0;
				*delay0 = (*delay0 & 0xFF00FFFF);
			}
		}
	}
	*spi_csmode = 0;

	if(rx_buf[3] == 'B')
	{
		cmd_len = rx_buf[0] + rx_buf[1]*128;
	}
	else
	{
		return 0;
	}
	memset(rx_buf, 0, 32);
	while(!get_input_val(WF_INT));
	delay(100);

	*delay0 = (*delay0 & 0xFF00FFFF);
	*spi_csmode = 2;

	uint32_t temp;
	for(int i = 0; i < cmd_len; i++)
	{
		while ((*txdata & METAL_SPI_TXDATA_FULL));
		*txdata = i;
		rx_buf[i] = (*rxdata);
		while ((rx_buf[i] & METAL_SPI_TXDATA_FULL))
		{
			rx_buf[i] = (*rxdata);
			if(!get_input_val(WF_INT))
			{
				*csmode = 0;
				*delay0 = (*delay0 & 0xFF00FFFF);
			}
		}
	}
	*spi_csmode = 0;
//	delay(100);
		char mess[cmd_len+1];
		memset(mess, 0, cmd_len + 1);
		for(int i = 0; i < cmd_len; i++)
		{
			mess[i] = (char)rx_buf[i];
		}
		if((mess[cmd_len-4] == 'L')&&(mess[cmd_len-3] == 'E')&&(mess[cmd_len-2] == 'D'))
		{
			switch (mess[cmd_len-1])
			{
				case 'R':
				{
					return 2;
				}
				case 'G':
				{
					return 3;
				}
				case 'B':
				{
					return 4;
				}
			}

		}
		for(int i = 0; i < cmd_len - 1; i ++)
		{
			if((mess[i] == 'O')&&(mess[i+1] == 'K'))
			{
				printf("[result]: OK\n");
				return 1;
			}
		}
		printf("[message]: %s\n", mess);
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
