#pragma once
#include <cstring>
#include <iostream>
#include <cassert>
#include <map>
#include <functional>

// http://www.6502.org/users/obelisk/6502/index.html
// http://www.6502.org/source/
// https://skilldrick.github.io/easy6502/

// There are "set to" and "set if", which logically different, but now done as the same.

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using i32 = int;

constexpr u32 BIT_SIZE = 8;
constexpr u32 MAX_RAM_BYTES = 65'536;

struct ram
{
	u8* data = new u8[MAX_RAM_BYTES];

	ram();

	void clear();

	u8 operator [] (u32 idx) const;

	u8& operator [] (u32 idx);
};


struct cpu
{
	ram& mem;	// attached ram
	static std::map<u8, std::function<void(cpu& cpu_ref)>> op_map;	// operations ap

	u16 pc;		// program counter
	u8 sp;		// stack pointer

	u8 a;		// accumulator register
	u8 x, y;	// x and y registers

	u8 c : 1;	// carry flag
	u8 z : 1;	// zero flag
	u8 i : 1;	// interrupt disable flag
	u8 d : 1;	// decimal mode flag
	u8 b : 1;	// break command flag
	u8 v : 1;	// overflow flag
	u8 n : 1;	// negative flag
	u8 k : 1;	// kill flag - unofficial

	cpu(ram& mem_ref);
	cpu(const cpu&) = delete;
	cpu(cpu&&) = delete;
	cpu& operator = (const cpu&) = delete;

	void reset();

	// fetches next byte, increments program counter
	u8 next_byte();

	// automatically offset for stack
	void push(u8 bt);

	// automatically offset for stack
	u8 pull();

	void exe_op(u8 op);

	void start(i32 operators = 0x7FFFFFFF);
};

// namespace for operator codes
namespace op
{
	// load/store operations
	constexpr u8 LDA_IM		= 0xA9;	// load accumulator - immediate
	constexpr u8 LDA_ZP		= 0xA5;	// load accumulator - zero page
	constexpr u8 LDA_ZP_X	= 0xB5;	// load accumulator - zero page, x
	constexpr u8 LDA_ABS	= 0xAD;	// load accumulator - absolute
	constexpr u8 LDA_ABS_X	= 0xBD;	// load accumulator - absolute, x
	constexpr u8 LDA_ABS_Y	= 0xB9;	// load accumulator - absolute, y
	constexpr u8 LDA_IN_X	= 0xA1;	// load accumulator - indirect, x
	constexpr u8 LDA_IN_Y	= 0xB1;	// load accumulator - indirect, y
	constexpr u8 STA_ZP		= 0x85; // store accumulator - zero page
	constexpr u8 STA_ZP_X	= 0x95; // store accumulator - zero page, x
	constexpr u8 STA_ABS	= 0x8D;	// store accumulator - absolute
	constexpr u8 STA_ABS_X	= 0x9D;	// store accumulator - absolute, x
	constexpr u8 STA_ABS_Y	= 0x99;	// store accumulator - absolute, y
	constexpr u8 STA_IN_X	= 0x81;	// store accumulator - indirect, x
	constexpr u8 STA_IN_Y	= 0x91;	// store accumulator - indirect, y

	constexpr u8 LDX_IM		= 0xA2; // load x register - immediate
	constexpr u8 LDX_ZP		= 0xA6; // load x register - zero page
	constexpr u8 LDX_ZP_Y	= 0xB6; // load x register - zero page, y
	constexpr u8 LDX_ABS	= 0xAE; // load x register - absolute
	constexpr u8 LDX_ABS_Y	= 0xBE; // load x register - absolute, y
	constexpr u8 STX_ZP		= 0x86; // store x register - zero page
	constexpr u8 STX_ZP_Y	= 0x96; // store x register - zero page, y
	constexpr u8 STX_ABS	= 0x8E; // store x register - absolute

	constexpr u8 LDY_IM		= 0xA0; // load y register - immediate
	constexpr u8 LDY_ZP		= 0xA4; // load y register - zero page
	constexpr u8 LDY_ZP_X	= 0xB4; // load y register - zero page, x
	constexpr u8 LDY_ABS	= 0xAC; // load y register - absolute
	constexpr u8 LDY_ABS_X	= 0xBC; // load y register - absolute, x
	constexpr u8 STY_ZP		= 0x84; // store y register - zero page
	constexpr u8 STY_ZP_X	= 0x94; // store y register - zero page, x
	constexpr u8 STY_ABS	= 0x8C; // store y register - absolute

	// register transfers
	constexpr u8 TAX		= 0xAA; // transfer accumulator to x
	constexpr u8 TAY		= 0xA8; // transfer accumulator to y
	constexpr u8 TXA		= 0x8A; // transfer x to accumulator
	constexpr u8 TYA		= 0x98; // transfer y to accumulator

	// stack operations
	constexpr u8 TSX		= 0xBA; // transfer stack pointer to x
	constexpr u8 TXS		= 0x9A; // transfer x to stack pointer
	constexpr u8 PHA		= 0x48; // push accumulator
	constexpr u8 PHP		= 0x08; // push processor status
	constexpr u8 PLA		= 0x68; // pull accumulator
	constexpr u8 PLP		= 0x28; // pull process status

	// logical
	constexpr u8 AND_IM		= 0x29;	// logical and - immediate
	constexpr u8 AND_ZP		= 0x25;	// logical and - zero page
	constexpr u8 AND_ZP_X	= 0x35;	// logical and - zero page, x
	constexpr u8 AND_ABS	= 0x2D;	// logical and - absolute
	constexpr u8 AND_ABS_X	= 0x3D;	// logical and - absolute, x
	constexpr u8 AND_ABS_Y	= 0x39;	// logical and - absolute, y
	constexpr u8 AND_IN_X	= 0x21;	// logical and - indirect, x
	constexpr u8 AND_IN_Y	= 0x31;	// logical and - indirect, y

	// arithmetic

	// increments & decrements

	// shifts
	constexpr u8 ASL_A		= 0x0A;	// arithmetic shift left - accumulator

	// jumps & calls
	constexpr u8 JMP_ABS	= 0x4C; // jump - absolute
	constexpr u8 JMP_IN		= 0x6C; // jump - indirect
	constexpr u8 JSR		= 0x20;	// jump to subroutine
	constexpr u8 RTS		= 0x60;	// return from subroutine

	// branches

	// status flag changes

	// system functions
	constexpr u8 NOP		= 0xEA; // no operator
	constexpr u8 KIL		= 0xFF;	// kill process - unofficial
}