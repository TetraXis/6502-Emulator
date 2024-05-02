#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>

#include "vm_6502.h"
#include "timer.h"

struct source_line
{
	u64 number = 0;
	std::string text = "";
	std::string parsed_op = "";
};

struct byte_line
{
	u8 size = 0;
	u8 bytes[3] = {};
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
	source_line					active_source_line = {};
	byte_line					active_byte_line = {};
	instruction					active_operator = {};
	std::vector<source_line>	source_lines = {};
	std::vector<byte_line>		byte_lines = {};

	std::vector<std::string>	warnings = {};
	std::vector<std::string>	errors = {};
	//std::pair<std::string, std::string> defines = {};
	static std::map<std::string, instruction> instructions_map;

	compiler();

	bool compile(const std::string& in, const std::string& out);
	bool compile(const std::string& in, ram& out);

	bool read_txt(const std::string& path);
	void write_txt(const std::string& path) const;

	// removes commentaries and empty strings
	void remove_comments();

	void resolve_defines();

	// false if compilation error
	bool parse_line(const source_line& line);

	bool parse_operator(const std::string& op);
	bool parse_addressing(const std::string& addr);

	void print_errors() const;
	void print_warnings() const;

	u16 parse_number(const std::string& num);

	void build_binary(const std::string& out) const;
	void build_ram(ram& out) const;
};

namespace mask
{
	static std::regex EMPTY("^\\s*(;.*)?$");
	static std::regex COMMENT("\\s*;.*$");
	static std::regex EXTRA_SPACES("\\s+");
	static std::regex BEGIN_SPACES("^\\s*");
	static std::regex END_SPACES("\\s*$");
	static std::regex DEFINE("^\\s*DEFINE\\s+([a-zA-Z_]\\w*)\\s+(.*)$");

	// numbers
	static std::regex HEX("^\\$([\\da-fA-F]+)$");
	static std::regex DEC("^(\\d+)$");
	static std::regex BIN("^\\%([01]+)$");

	static std::regex CORRECT_LINE("^((\\s*[a-zA-Z_]\\w*?)\\s+)?([A-Za-z]{3})((\\s+)(.*?))?(\\s*)(;.*)?$");

	// addressings
	static std::regex IM("^#(\\$?%?[\\da-fA-F]+)$"); // immediate
	// absolute addressing is changed into zero page if address is <= 0xFF
	static std::regex ABS("^(\\$?%?[\\da-fA-F]+)$"); // absolute or zero page or relative
	static std::regex ABS_X("^(\\$?%?[\\da-fA-F]+)\\s*,\\s*[Xx]$"); // absolute, x or zero page, x
	static std::regex ABS_Y("^(\\$?%?[\\da-fA-F]+)\\s*,\\s*[Yy]$"); // absolute, y or zero page, y
	static std::regex ABS_IN("^\\((\\$?%?[\\da-fA-F]+)\\s*\\)$"); // absolute indirect
	static std::regex IN_X("^\\(\\s*(\\$?%?[\\da-fA-F]+)\\s*,\\s*[Xx]\\s*\\)$"); // inderect, x
	static std::regex IN_Y("^\\(\\s*(\\$?%?[\\da-fA-F]+)\\s*\\)\\s*,\\s*[Yy]$"); // indirect, y
	// if no zero page addressing found checks for relative
	static std::regex LABEL("^[a-zA-Z_]\\w*$");
}