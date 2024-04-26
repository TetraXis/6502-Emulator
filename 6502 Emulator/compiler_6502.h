#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

#include "vm_6502.h"

struct source_line
{
	u64 number = 0;
	std::string text = "";
};

constexpr u8 ABSENT = 0x00; // for indicating that addressing variant is absent
// BRK abd JMP should be handled individually

struct instruction
{
	//std::string name = "";
	// opcodes variants for addressing
	u8 imp = {};	// implied
	u8 im = {};		// immediate
	u8 zp = {};		// zero page or relative
	u8 zp_x = {};	// zero page, x
	u8 zp_y = {};	// zero page, y
	u8 abs = {};	// absolute
	u8 abs_x = {};	// absolute, x
	u8 abs_y = {};	// absolute, y
	// u8 abs_in = {};	// absolute indirect JMP is the only one
	u8 in_x = {};	// indirect, x
	u8 in_y = {};	// indirect, y
	//u8 rel = {};	// relative
};


struct compiler
{
	std::vector<source_line> lines = {};
	static std::map<std::string, instruction> instructions_map;

	compiler();

	bool compile(const std::string in, const std::string out);

	bool read_txt(const std::string path);
	void write_txt(const std::string path) const;

	// removes commentaries and empty strings
	void remove_comments();

	// false if compilation error
	bool parse_line(const source_line& line) const;
};

namespace masks
{
	static std::regex EMPTY("^\\s*(;.*)?$");
	static std::regex COMMENT("\\s*;.*");

	// numbers
	static std::regex HEX("^\\$([\\da-fA-F]+)$");
	static std::regex DEC("^(\\d+)$");
	static std::regex BIN("^\\%([01]+)$");

	static std::regex INST("^(\\s*)([A-Z]{3})(\\s+)(.*?)(\\s*)(;.*)?$");

	// addressings
	static std::regex IM("^(#\\$?%?[\\da-fA-F]+)$"); // immediate
	// absolute addressing is changed into zero page if address is <= 0xFF
	static std::regex ABS("^(\\$?%?[\\da-fA-F]+)$"); // absolute or zero page or relative
	static std::regex ABS_X("^(#?\\$?%?[\\da-fA-F]+\\s*,\\s*X)$"); // absolute, x or zero page, x
	static std::regex ABS_Y("^(#?\\$?%?[\\da-fA-F]+\\s*,\\s*Y)$"); // absolute, y or zero page, y
	static std::regex ABS_IN("^(\\(#?\\$?%?[\\da-fA-F]+\\s*,\\s*Y\\))$"); // absolute indirect
	static std::regex IN_X("^(\\(\\s*\\$?%?[\\da-fA-F]+\\s*,\\s*X\\s*\\))$"); // inderect, x
	static std::regex IN_Y("^(\\(\\s*\\$?%?[\\da-fA-F]+\\s*\\)\\s*,\\s*Y)$"); // indirect, y
	// if no zero page addressing found checks for relative
	static std::regex LABEL("^[a-zA-Z_]\\w*$");
}