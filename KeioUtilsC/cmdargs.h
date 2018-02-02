#pragma once

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


/*	Define your arguments here
*/
extern float float_test;
extern char char_test;
extern int32_t int32_test;
extern char	*string;

#define	ARGUMENT_TABLE \
	{ &int32_test, ARGTYPE_INT32, "int_arg", "integer argument", true }, \
	{ &string, ARGTYPE_STRING, "string_arg", "string argument", false },

#define	OPTION_TABLE \
	{ { &float_test, ARGTYPE_FLOAT, "float_opt", "optional float", true }, 'f', "float" }, \
	{ { &char_test, ARGTYPE_CHAR, "char_opt", "optional char", false }, 'c', "char" },


extern bool cmdargs_parse(int argc, char *argv[]);
