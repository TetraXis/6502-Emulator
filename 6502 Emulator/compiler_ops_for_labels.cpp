#include "compiler_6502.h"

std::unordered_map<std::string, u8> compiler::ops_that_can_use_labels =
{
	{
		"JSR",	// op name
		3		// byte size
	},
	{
		"JMP",	// op name
		3	  	// byte size
	},
	{
		"BCC",	// op name
		2	  	// byte size
	},
	{
		"BCS",	// op name
		2	  	// byte size
	},
	{
		"BEQ",	// op name
		2	  	// byte size
	},
	{
		"BMI",	// op name
		2	  	// byte size
	},
	{
		"BNE",	// op name
		2	  	// byte size
	},
	{
		"BPL",	// op name
		2	  	// byte size
	},
	{
		"BVC",	// op name
		2	  	// byte size
	},
	{
		"BVS",	// op name
		2	  	// byte size
	}
};