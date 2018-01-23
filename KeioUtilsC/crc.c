// crc.cpp

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "crc.h"

#define USE_LOOKUP_TABLES

uint8_t crc8_initial = 0;
uint8_t crc8_poly = 0x1D;

uint16_t crc16_initial = 0xFFFF;
uint16_t crc16_poly = 0x1021;
uint16_t crc16_final_xor = 0x0000;
bool crc16_reverse_input = false;
bool crc16_reverse_output = false;

uint32_t crc32_initial = 0xFFFFFFFF;
uint32_t crc32_poly = 0x04C11DB7;
uint32_t crc32_final_xor = 0xFFFFFFFF;
bool crc32_reverse_input = true;
bool crc32_reverse_output = true;

/**************************************************************************************************
* Set CRC16 parameters
*/
void crc16_set(uint16_t initial_value, uint16_t polynomial, uint16_t final_xor, bool reverse_input, bool reverse_output)
{
	crc16_initial = initial_value;
	crc16_poly = polynomial;
	crc16_final_xor = final_xor;
	crc16_reverse_input = reverse_input;
	crc16_reverse_output = reverse_output;
}

/**************************************************************************************************
* Reverse bits in words
*/
#ifndef USE_LOOKUP_TABLES
uint8_t reverse8(uint8_t x)
{
	x = ((x >> 1) & 0x55) | ((x & 0x55) << 1);	// swap odd/even bits
	x = ((x >> 2) & 0x33) | ((x & 0x33) << 2);	// swap consecutive pairs
	x = ((x >> 4) & 0x0F) | ((x & 0x0F) << 4);	// swap nibbles
	return x;
}

uint16_t reverse16(uint16_t x)
{
	x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1);	// swap odd/even bits
	x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2);	// swap consecutive pairs
	x = ((x >> 4) & 0x0F0F) | ((x & 0x0F0F) << 4);	// swap nibbles
	x = ((x >> 8) & 0x00FF) | ((x & 0x00FF) << 8);	// swap bytes
	return x;
}

uint32_t reverse32(uint32_t x)
{
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);	// swap odd/even bits
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);	// swap consecutive pairs
	x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);	// swap nibbles
	x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);	// swap bytes
	x = (x >> 16) | (x << 16);						// swap 16 bit words
	return x;
}

/**************************************************************************************************
* CRC8
*/
uint8_t crc8(const void *buffer, unsigned int buffer_length)
{
	uint8_t crc = 0;
	uint8_t *ptr = (uint8_t *)buffer;

	while (buffer_length--)
	{
		crc ^= *ptr++;

		for (uint8_t i = 0; i < 8; i++)
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ crc8_poly;
			else
				crc <<= 1;
		}
	}

	return crc;
}

/**************************************************************************************************
* CRC16
*/
uint16_t crc16(const void *buffer, unsigned int buffer_length)
{
	uint16_t crc = crc16_initial;
	uint8_t *ptr = (uint8_t *)buffer;

	while (buffer_length--)
	{
		uint8_t b = crc16_reverse_input ? reverse8(*ptr++) : *ptr++;
		crc ^= (uint16_t)b << 8;

		for (int i = 0; i < 8; i++)
		{
			if ((crc & 0x8000) != 0)
				crc = (crc << 1) ^ crc16_poly;
			else
				crc <<= 1;
		}
	}

	return crc16_final_xor ^ (crc16_reverse_output ? reverse16(crc) : crc);
}

/**************************************************************************************************
* CRC32
*/
uint32_t crc32(const void *buffer, unsigned int buffer_length)
{
	uint32_t crc = crc32_initial;
	uint8_t *ptr = (uint8_t *)buffer;

	while (buffer_length--)
	{
		crc ^= (uint32_t)(crc32_reverse_input ? reverse8(*ptr++) : *ptr++) << 24;

		for (uint8_t i = 0; i < 8; i++)
		{
			if (crc & 0x80000000)
				crc = (crc << 1) ^ crc32_poly;
			else
				crc <<= 1;
		}
	}

	return crc32_final_xor ^ (crc32_reverse_output ? reverse32(crc) : crc);
}

/**************************************************************************************************
* Not required for non-lookup table versions
*/
void crc_init(void)
{}

#else
const uint8_t bit_reversal_table[] =
{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

uint8_t crc8_lut[256];
uint16_t crc16_lut[256];
uint32_t crc32_lut[256];

inline uint8_t reverse8(uint8_t x)
{
	return bit_reversal_table[x];
}

inline uint16_t reverse16(uint16_t x)
{
	return bit_reversal_table[x >> 8] | (bit_reversal_table[x & 0xFF] << 8);
}

inline uint32_t reverse32(uint32_t x)
{
	const uint8_t *p = (uint8_t *)&x;
	return	bit_reversal_table[p[3]] |
			(bit_reversal_table[p[2]] << 8) |
			(bit_reversal_table[p[1]] << 16) |
			(bit_reversal_table[p[0]] << 24);
}

/**************************************************************************************************
* Generate lookup table for CRC8
*/
void crc8_calculate_table(void)
{
	uint8_t i = 0;
	do
	{
		uint8_t byte = i;
		for (uint8_t bit = 0; bit < 8; bit++)
		{
			if (byte & 0x80)
				byte = (byte << 1) ^ crc8_poly;
			else
				byte <<= 1;
		}
		crc8_lut[i] = byte;
	} while (++i);
}

/**************************************************************************************************
* CRC8
*/
uint8_t crc8(const void *buffer, unsigned int buffer_length)
{
	uint8_t crc = crc8_initial;
	uint8_t *ptr = (uint8_t *)buffer;

	while (buffer_length--)
		crc = crc8_lut[*ptr++ ^ crc];
	return crc;
}

/**************************************************************************************************
* Generate lookup table for CRC16
*/
void crc16_calculate_table(void)
{
	uint8_t i = 0;
	do
	{
		uint16_t word = (uint16_t)i << 8;
		for (uint8_t bit = 0; bit < 8; bit++)
		{
			if (word & 0x8000)
				word = (word << 1) ^ crc16_poly;
			else
				word <<= 1;
		}
		crc16_lut[i] = word;
	} while (++i);
}

/**************************************************************************************************
* CRC16
*/
uint16_t crc16(const void *buffer, unsigned int buffer_length)
{
	uint16_t crc = crc16_initial;
	uint8_t *ptr = (uint8_t *)buffer;

	if (crc16_reverse_input)
	{
		while (buffer_length--)
			crc = (crc << 8) ^ (crc16_lut[(crc >> 8) ^ reverse8(*ptr++)]);
	}
	else
	{
		while (buffer_length--)
			crc = (crc << 8) ^ (crc16_lut[(crc >> 8) ^ *ptr++]);
	}
	return crc16_reverse_output ? reverse16(crc) : crc;
}

/**************************************************************************************************
* Generate lookup table for CRC32
*/
void crc32_calculate_table(void)
{
	uint8_t i = 0;
	do
	{
		uint32_t word = (uint32_t)i << 24;
		for (uint8_t bit = 0; bit < 8; bit++)
		{
			if (word & 0x80000000)
				word = (word << 1) ^ crc32_poly;
			else
				word <<= 1;
		}
		crc32_lut[i] = word;
	} while (++i);
}

/**************************************************************************************************
* CRC32
*/
uint32_t crc32(const void *buffer, unsigned int buffer_length)
{
	uint32_t crc = crc32_initial;
	uint8_t *ptr = (uint8_t *)buffer;

	if (crc32_reverse_input)
	{
		while (buffer_length--)
			crc = (crc << 8) ^ (crc32_lut[(crc >> 24) ^ reverse8(*ptr++)]);
	}
	else
	{
		while (buffer_length--)
			crc = (crc << 8) ^ (crc32_lut[(crc >> 24) ^ *ptr++]);
	}
	return crc32_final_xor ^ (crc32_reverse_output ? reverse32(crc) : crc);
}

/**************************************************************************************************
* Generate lookup tables
*/
void crc_init(void)
{
	crc8_calculate_table();
	crc16_calculate_table();
	crc32_calculate_table();
}

#endif


/*
// lookup table for CRC32
const uint32_t crc32_table[256] = {
0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

uint32_t crc32(uint8_t *buffer, uint32_t buffer_length)
{
uint32_t crc = 0xFFFFFFFF;

for (uint32_t i = 0; i < buffer_length; i++)
crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32_table[(crc ^ buffer[i]) & 0xFF];

return (crc ^ 0xFFFFFFFF);
}
*/

/**************************************************************************************************
* XMEGA NVM compatible CRC32
*/
#define XMEGA_CRC32_POLY	0x0080001B	// Polynomial for use with Xmega devices

uint32_t xmega_nvm_crc32(uint8_t *buffer, unsigned int buffer_length)
{
	unsigned int	address;
	uint32_t		data_reg, help_a, help_b;
	uint32_t		crc_reg = 0;

	for (address = 0; address < buffer_length; address += 2)
	{
		help_a = crc_reg << 1;
		help_a &= 0x00FFFFFE;
		help_b = crc_reg & (1 << 23);

		if (help_b > 0)
			help_b = 0x00FFFFFF;

		data_reg = (uint32_t)((buffer[address + 1] << 8) | buffer[address] & 0xFFFF);

		crc_reg = (help_a ^ data_reg) ^ (help_b & XMEGA_CRC32_POLY);
		crc_reg = crc_reg & 0x00FFFFFF;
	}

	return crc_reg;
}
