#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>

#include "vm_6502.h"
#include "timer.h"

// #define DEBUG_6502_COMP

// NOTE: Labels only can be used by standard branch ops

constexpr u8 ABSENT_OP = 0x00; // for indicating that addressing variant is absent
// BRK abd JMP should be handled individually
constexpr u16 ABSENT_LABEL = 0xFFFF; // for label that was declared, but its address was not yet calculated

struct op_modes
{
	u8 imp		= ABSENT_OP;	// implied
	u8 im		= ABSENT_OP;	// immediate
	u8 zp		= ABSENT_OP;	// zero page or relative
	u8 zp_x		= ABSENT_OP;	// zero page, x
	u8 zp_y		= ABSENT_OP;	// zero page, y
	u8 abs		= ABSENT_OP;	// absolute
	u8 abs_x	= ABSENT_OP;	// absolute, x
	u8 abs_y	= ABSENT_OP;	// absolute, y
	u8 in_x		= ABSENT_OP;	// indirect, x
	u8 in_y		= ABSENT_OP;	// indirect, y
};

enum struct addr_mode : u8
{
	none, imp, im, zp, zp_x, zp_y, abs, abs_x, abs_y, in_x, in_y
};

struct source_line
{
	bool		parsed = false;
	bool		unresolved_label = false;
	u64			number = 0;
	std::string	text = "";
	op_modes	parsed_op = {};
	u8			byte_size = 0;
	u8			bytes[3] = {};

	source_line();
	source_line(u64 num, const std::string& new_text);
};

struct msg
{
	std::string text = "";

	// create as error
	static msg err(const source_line& line, const std::string& type, const std::string& add_msg);
	// create as warning
	static msg wrn(const source_line& line, const std::string& type, const std::string& add_msg);
};

constexpr u64 MAX_PARSE_PASSES = 16;

struct compiler
{
	static std::unordered_map<std::string, op_modes>		op_map;
	//              op           byte size
	static std::unordered_map<std::string, u8>			ops_that_can_use_labels;

	//       name         addr
	std::unordered_map<std::string, u16>	labels = {};
	std::vector<source_line>	source_lines = {};
	source_line*				active_line = nullptr;
	// counts bytes before encoutering a label
	u16							parsed_bytes = 0;
	
	std::vector<msg>			warnings = {};
	std::vector<msg>			errors = {};
	timer						internal_timer = {};

	compiler();

	bool read_txt(const std::string& path);
	void write_txt(const std::string& path) const;

	void print_errors() const;
	void print_warnings() const;

	u16 parse_number(const std::string& num);
	u8 convert_to_byte(u64 number);

	bool compile(const std::string& path);
	bool compile_and_build(const std::string& path, cpu& cpu_ref);

	void resolve_defines();
	void clean_up();
	void read_labels();
	bool parse_active_line();

	// all methods below rely on shared active_line
	bool parse_op(const std::string& op);
	bool resolve_addr_for_op(std::string addr, const std::string& op);
};

namespace mask
{
	// preprocessor
	static std::regex STRONG_DEFINE("^\\s*#DEFINE\\s+\"(.*)\"\\s+\"(.*)\"\\s*$");
	// group 1: name
	// group 2: value
	static std::regex WEAK_DEFINE("^\\s*#DEFINE\\s+([_a-zA-Z]\\w*)\\s+(.+)\\s*$");
	// group 1: name
	// group 2: value
	static std::regex WORD("[_a-zA-Z]\\w*");
	static std::regex EMPTY("^\\s*(;.*)?$");
	static std::regex COMMENT("\\s*;.*$");
	static std::regex EXTRA_SPACES("\\s+");
	static std::regex BEGIN_SPACES("^\\s*");
	static std::regex END_SPACES("\\s*$");

	// numbers
	// group 1: value
	static std::regex HEX("^\\$([\\da-fA-F]+)$");
	static std::regex DEC("^(\\d+)$");
	static std::regex BIN("^\\%([01]+)$");
	static std::regex COMPLEX("#?([$%]?[\\da-fA-F]+)\\s*([+-])\\s*([$%]?[\\da-fA-F]+)$");
	// group 1: left number
	// group 2: operator
	// group 3: right number

	// main
	static std::regex CORRECT_OP("^(([_a-zA-Z]\\w*):?\\s+)?([_a-zA-Z]\\w*)(\\s+(.*))?$");
	// group 2: label
	// group 3: op
	// group 5: addr

	// labels
	static std::regex LABEL_DECL("^\\s*([_a-zA-Z]\\w*)\\s*:?\\s*$");
	// group 1: label
	static std::regex LABEL_IN_USE("^([_a-zA-Z]\\w*)$");
	// group 1: label

	// address modes
	// group 1: addr value
	static std::regex IM("^#([%$]?[\\da-fA-F]+)$");
	static std::regex ABS("^([%$]?[\\da-fA-F]+)$"); // absolute, zero page or relative
	static std::regex ABS_X("^([%$]?[\\da-fA-F]+)\\s*,\\s*[Xx]$"); // absolute, x or zero page, x
	static std::regex ABS_Y("^([%$]?[\\da-fA-F]+)\\s*,\\s*[Yy]$"); // absolute, y or zero page, y
	static std::regex IN_X("^\\s*\\(\\s*([%$]?[\\da-fA-F]+)\\s*,\\s*[Xx]\\s*\\)$"); // indirect, x
	static std::regex IN_Y("^\\s*\\(\\s*([%$]?[\\da-fA-F]+)\\s*\\)\\s*,\\s*[Yy]$"); // indirect, y
	// BRK and JMP are edge cases
}