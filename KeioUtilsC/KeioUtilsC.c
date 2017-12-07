/* KeioUtilsC.cpp : Defines the entry point for the console application.
*/

#include <stdint.h>
#include <stdio.h>
#include "SIUnits.h"

int main()
{
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
	return 0;
}

