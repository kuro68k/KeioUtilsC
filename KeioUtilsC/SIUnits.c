// SIUnits.c

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

const char Prefixes[] = "yzafpnum kMGTPEZY";
char SetFormatF[32] = "%.2f";
char SetMinimumUnit = 'u';
double SetMargin = 1.0;

void ToSIUnitsDouble(char *s, double v, char *format, double margin, char minimum_unit)
{
	if (format == NULL)
		format = "%2f";

	int min_unit_idx = 0;
	if (minimum_unit != '\0')
	{
		char *min = strchr(Prefixes, minimum_unit);		// returns 0 if not found
		if (min != NULL)
			min_unit_idx = min - Prefixes;
	}
	char min_unit_char = Prefixes[min_unit_idx];

	// handle negative numbers by storing a -ve flag and processing as positive
	bool negative = false;
	if (v < 0)
	{
		negative = true;
		v = 0 - v;
	}

	// find best prefix
	char unit_char = min_unit_char;
	int exp = -21 + (3 * min_unit_idx);
	double m;

	if (v != 0)
	{
		bool minimum_found = false;
		int idx = 0;
		for (; exp <= 24; exp += 3)
		{
			// skip over prefixes if they are below the selected minimum
			if (Prefixes[idx] == min_unit_char)
				minimum_found = true;
			if (!minimum_found)
			{
				idx++;
				continue;
			}

			unit_char = Prefixes[idx];
			m = pow(10, exp) * margin;			// calculate every time to avoid precision/rounding errors
			if (v <= m)
			{
				if (negative)
					v = 0 - v;
				break;
			}
			idx++;
		}
	}

	m = pow(10, exp - 3);
	sprintf(s, format, v / m);
	char unit_str[2] = "\0";
	unit_str[0] = unit_char;
	strcat(s, unit_str);
}
/*
void ToSIUnitsLong(char *s, long v, long margin, char minimum_unit)
{
//	ToSIUnitsDouble(s, (double)v, format, (double)margin, minimum_unit);

	int min_unit_idx = 0;
	if (minimum_unit != '\0')
	{
		char *min = strchr(Prefixes, minimum_unit);		// returns 0 if not found
		if (min != NULL)
			min_unit_idx = min - Prefixes;
	}
	char min_unit_char = Prefixes[min_unit_idx];

	// handle negative numbers by storing a -ve flag and processing as positive
	bool negative = false;
	if (v < 0)
	{
		negative = true;
		v = 0 - v;
	}

	// find best prefix
	char unit_char = min_unit_char;
	int exp = -21 + (3 * min_unit_idx);
	long m;

	if (v != 0)
	{
		bool minimum_found = false;
		int idx = 0;
		for (; exp <= 24; exp += 3)
		{
			// skip over prefixes if they are below the selected minimum
			if (Prefixes[idx] == min_unit_char)
				minimum_found = true;
			if (!minimum_found)
			{
				idx++;
				continue;
			}

			unit_char = Prefixes[idx];
			m = pow(10, exp) * margin;			// calculate every time to avoid precision/rounding errors
			if (v <= m)
			{
				if (negative)
					v = 0 - v;
				break;
			}
			idx++;
		}
	}

	m = pow(10, exp - 3);
	sprintf(s, format, v / m);
	char unit_str[2] = "\0";
	unit_str[0] = unit_char;
	strcat(s, unit_str);
}

void ToSIUnitsLongS(char *s, long v)
{
	ToSIUnitsDouble(s, (double)v, "%lf", (double)SetMargin, SetMinimumUnit);
}
*/
bool SetSIFormat(char *format, char minimum_unit, double margin)
{
	if (strlen(format) > sizeof(SetFormatF))
		return false;
	strncpy(SetFormatF, format, sizeof(SetFormatF));
	SetMinimumUnit = minimum_unit;
	SetMargin = margin;
	return true;
}
