/* KeioUtilsC.cpp : Defines the entry point for the console application.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "SIUnits.h"
#include "crc.h"
#include "intel_hex.h"
#include "cmdargs.h"

int32_t int32_test;
char	*string;
float float_test;
char char_test;


int main(int argc, char *argv[])
{
	// CRC tests
	//printf("123456789: %04X\n", crc_ccitt("123456789", 9));
	crc_init();
	printf("123456789: %02X\n", crc8("123456789", 9));
	printf("123456789: %04X\n", crc16("123456789", 9));
	printf("123456789: %08X\n", crc32("123456789", 9));
	printf("\n");

    
	// SI unit tests
	char temp[256];
	double margin = 0.999999999;

	ToSIUnitsDouble(temp, 0, "%.2f", margin, 'u');
	printf("%s\n", temp);
	ToSIUnitsDouble(temp, 0.1, "%.2f", margin, 0);
	printf("%s\n", temp);
	ToSIUnitsDouble(temp, 0.01, "%.2f", margin, 0);
	printf("%s\n", temp);
	ToSIUnitsDouble(temp, 0.001, "%.2f", margin, 0);
	printf("%s\n", temp);
	ToSIUnitsDouble(temp, 0.0001, "%.2f", margin, 0);
	printf("%s\n", temp);

	ToSIUnitsDouble(temp, 0, "%.2f", margin, 0);
	printf("%s\n", temp);

	ToSIUnitsDouble(temp, 1000, "%.2f", margin, 0);
	printf("%s\n", temp);
	ToSIUnitsDouble(temp, 10000000000000000000, "%.2f", margin, 0);
	printf("%s\n", temp);
	printf("\n");


	// Intel HEX tests
	uint32_t image_size;
	if (!ihex_read_file("test.hex", NULL, 0xFFFFFFFF, &image_size))
	{
		printf("Unable to read test.hex.\n");
		printf("Last error: %s\n", ihex_get_last_error());
		return -1;
	}
	printf("Image size: %u bytes\n", image_size);
	uint8_t *buffer = malloc(image_size);
	if (!ihex_read_file("test.hex", buffer, image_size, NULL))
	{
		printf("Unable to read test.hex (2).\n");
		printf("Last error: %s\n", ihex_get_last_error());
		return -1;
	}
	printf("HEX file read OK.\n");
	printf("Last error: %s\n", ihex_get_last_error());
	free(buffer);
	printf("\n");


	// command line arguments
	cmdargs_parse(argc, argv);
	printf("int32_test = %d\n", int32_test);
	printf("string = \"%s\"\n", string);
	printf("float_test = \"%f\"\n", float_test);
	printf("char_test = \"%c\"\n", char_test);
	printf("\n");

	cmdargs_print_help("KeioUtilsC");

	return 0;
}

