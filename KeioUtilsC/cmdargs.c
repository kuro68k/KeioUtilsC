/*	Handle command line arguments
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef enum
{
	ARGTYPE_INT8,
	ARGTYPE_INT16,
	ARGTYPE_INT32,
	ARGTYPE_INT64,
	ARGTYPE_UINT8,
	ARGTYPE_UINT16,
	ARGTYPE_UINT32,
	ARGTYPE_UINT64,
	ARGTYPE_FLOAT,
	ARGTYPE_DOUBLE,
	ARGTYPE_CHAR,
	ARGTYPE_STRING,
	ARGTYPE_BOOL,
} ARGTYPE_t;

typedef struct
{
	void		*target;
	ARGTYPE_t	type;
	char		*short_description;
	char		*help;
	bool		required;
} CMDARGUMENT_t;

typedef struct
{
	CMDARGUMENT_t	arg;
	char	short_opt;		// single character option name
	char	*long_opt;		// string option name
} CMDOPTION_t;

int32_t int32_test;
char	*string;

const CMDARGUMENT_t arg_list[] = {
	{ &int32_test, ARGTYPE_INT32, "int_arg", "integer argument", true },
	{ &string, ARGTYPE_STRING, "string_arg", "string argument", false },
};
#define	NUM_ARGS	(sizeof(arg_list) / sizeof(arg_list[0]))


bool parse(char *arg, uint8_t type, void *target)
{
	uint64_t vu64 = 0;
	int64_t v64 = 0;
	double vd = 0;
	switch (type)
	{
		case ARGTYPE_INT8:
		case ARGTYPE_INT16:
		case ARGTYPE_INT32:
		case ARGTYPE_INT64:
			errno = 0;
			v64 = strtoll(arg, NULL, 0);
			if (errno != 0)
				return false;
			break;

		case ARGTYPE_UINT8:
		case ARGTYPE_UINT16:
		case ARGTYPE_UINT32:
		case ARGTYPE_UINT64:
			errno = 0;
			vu64 = strtoull(arg, NULL, 0);
			if (errno != 0)
				return false;
			break;

		case ARGTYPE_FLOAT:
		case ARGTYPE_DOUBLE:
			errno = 0;
			vd = strtod(arg, NULL);
			if (errno != 0)
				return false;
			break;
	}
	
	switch (type)
	{
		case ARGTYPE_INT8:
			if ((v64 < INT8_MIN) || (v64 > INT8_MAX))
				return false;
			*(int8_t *)target = (int8_t)v64;
			break;

		case ARGTYPE_INT16:
			if ((v64 < INT16_MIN) || (v64 > INT16_MAX))
				return false;
			*(int16_t *)target = (int16_t)v64;
			break;

		case ARGTYPE_INT32:
			if ((v64 < INT32_MIN) || (v64 > INT32_MAX))
				return false;
			*(int32_t *)target = (int32_t)v64;
			break;

		case ARGTYPE_INT64:
			*(int64_t *)target = v64;
			break;

		case ARGTYPE_UINT8:
			if (vu64 > UINT8_MAX)
				return false;
			*(uint8_t *)target = (uint8_t)vu64;
			break;

		case ARGTYPE_UINT16:
			if (vu64 > UINT16_MAX)
				return false;
			*(uint16_t *)target = (uint16_t)vu64;
			break;

		case ARGTYPE_UINT32:
			if (vu64 > UINT32_MAX)
				return false;
			*(uint32_t *)target = (uint32_t)vu64;
			break;

		case ARGTYPE_UINT64:
			*(uint64_t *)target = vu64;
			break;

		case ARGTYPE_FLOAT:
			*(float *)target = (float)vd;
			break;

		case ARGTYPE_DOUBLE:
			*(double *)target = vd;
			break;

		case ARGTYPE_STRING:
			*(char **)target = arg;
			break;

		default:
			return false;
	}
	
	return true;
}

bool cmdargs_parse(int argc, char *argv[])
{
	bool consumed[256] = { false };
	bool found[NUM_ARGS];

	int i;
	printf("argc: %d\n", argc);

	int count = 0;
	for (int i = 1; i < argc; i++)
	{
		if (consumed[i])		// already consumed
			continue;
		printf("arg: \"%s\"\n", argv[i]);

		if (!parse(argv[i], arg_list[count].type, arg_list[count].target))
		{
			printf("Unable to parse \"%s\".\n", argv[i]);
			return false;
		}

		found[count] = true;
		count++;
	}

	// check if all required arguments were found
	for (i = 0; i < NUM_ARGS; i++)
	{
		if (arg_list[i].required && !found[i])
		{
			printf("Argument \"%s\" is required.\n", arg_list[i].short_description);
			return false;
		}
	}

	printf("int32_test = %d\n", int32_test);
	printf("string = \"%s\"\n", string);

	return true;
}
