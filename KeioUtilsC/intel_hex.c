// intel_hex.cpp

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "intel_hex.h"
#include "crc.h"


uint32_t firmware_crc = 0;
uint32_t firmware_size = 0;
char last_error[128] = "None";


uint32_t read_base16(char *c, int num_chars)
{
	uint32_t val = 0;

	while (num_chars--)
	{
		val += (*c <= '9' ? *c - '0' : *c - 'A' + 10) << (num_chars * 4);
		c++;
	}

	return val;
}

// load an Intel hex file into buffer
bool ihex_read_file(char *filename, uint8_t *buffer, unsigned int buffer_size)
{
	FILE *fp;
	bool res = true;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		snprintf(last_error, sizeof(last_error), "Unable to open %s.\n", filename);
		return false;
	}
	//quiet_printf("Loading %s...\n", filename);

	memset(buffer, 0xFF, buffer_size);
	uint32_t	base_addr = 0;

	int line_num = 0;
	while (feof(fp) != EOF)
	{
		line_num++;
		char line[1024];
		if (fgets(line, sizeof(line), fp) == NULL)
			break;
		//printf("%u:\t%s", line_num, line);

		if (line[0] != ':')
		{
			snprintf(last_error, sizeof(last_error), "Invalid line %d (missing colon)\n", line_num);
			res = false;
			break;
		}

		char *c = &line[1];
		uint8_t len = read_base16(c, 2);
		c += 2;
		uint16_t addr = read_base16(c, 4);
		c += 4;
		uint8_t type = read_base16(c, 2);
		c += 2;

		//printf("%u\t%X\t%u\n", len, addr, type);

		switch (type)
		{
		case 0:		// data record
			for (uint16_t i = 0; i < len; i++)
			{
				uint32_t absadr = base_addr + (addr++);
				if (absadr > buffer_size)
				{
					snprintf(last_error, sizeof(last_error), "Firmware image too large for buffer (%X).\n", absadr);
					res = false;
					goto exit;
				}
				buffer[absadr] = read_base16(c, 2);
				c += 2;
				if (absadr > firmware_size)
					firmware_size = absadr;
			}
			break;

		case 2:		// extended segment address record
			if (len != 2)
			{
				snprintf(last_error, sizeof(last_error), "Invalid line %d (bad extended segment address length: %u)\n", line_num, len);
				res = false;
				break;
			}
			base_addr = read_base16(c, 4) << 4;
			//printf("%u:\tbase_addr = %X\n", line_num, base_addr);
			c += 4;
		}

		uint8_t checksum = read_base16(c, 2);
		// todo: check checksum

		if (res != true)
			break;
	}

	//quiet_printf("Firmware size:\t%u bytes (0x%X)\n", firmware_size, firmware_size);

exit:
	fclose(fp);
	return res;
}