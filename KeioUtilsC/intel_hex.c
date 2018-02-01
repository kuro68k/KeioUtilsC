/*	intel_hex.cpp
	Read Intel HEX format images into memory.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "intel_hex.h"


char last_error[128] = "None";

char* ihex_get_last_error(void)
{
	return last_error;
}

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

/*	Load an Intel hex file into buffer. If buffer is NULL nothing is written, allowing you to check
	the size via *image_size before allocating enough memory to hold it. buffer_size is always checked
	even if buffer is NULL.
*/
bool ihex_read_file(char *filename, uint8_t *buffer, unsigned int buffer_size, uint32_t *image_size)
{
	FILE *fp;
	bool res = true;

	if (image_size != NULL)
		*image_size = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		snprintf(last_error, sizeof(last_error), "Unable to open %s.\n", filename);
		return false;
	}
	//quiet_printf("Loading %s...\n", filename);

	if (buffer != NULL)
		memset(buffer, 0xFF, buffer_size);
	uint32_t	base_addr = 0;

	int line_num = 0;
	while (feof(fp) != EOF)
	{
		uint8_t checksum = 0;
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

		checksum = len + (addr >> 8) + (addr & 0xFF) + type;
		//printf("%u\t%X\t%u\n", len, addr, type);

		switch (type)
		{
		case 0:		// data record
			for (uint16_t i = 0; i < len; i++)
			{
				uint32_t absadr = base_addr + (addr++);
				if (absadr >= buffer_size)
				{
					snprintf(last_error, sizeof(last_error), "Firmware image too large for buffer (%X).\n", absadr);
					res = false;
					goto exit;
				}
				uint8_t b = read_base16(c, 2);
				checksum += b;
				if (buffer != NULL)
					buffer[absadr] = b;
				c += 2;
				if (image_size != NULL)
				{
					if (absadr > *image_size)
						*image_size = absadr;
				}
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
			checksum += (base_addr >> 8) + (base_addr & 0xFF);
			//printf("%u:\tbase_addr = %X\n", line_num, base_addr);
			c += 4;
		}

		uint8_t cc = read_base16(c, 2);
		checksum = ~checksum + 1;
		if (cc != checksum)
		{
			snprintf(last_error, sizeof(last_error),
					 "Checksum mismatch on line %d (read %02X, calculated %02X)\n",
					 line_num, cc, checksum);
			res = false;
			break;
		}

		if (res != true)
			break;
	}

	if (image_size != NULL)
		(*image_size)++;	// size is highest address + 1
	//quiet_printf("Firmware size:\t%u bytes (0x%X)\n", firmware_size, firmware_size);

exit:
	fclose(fp);
	return res;
}
