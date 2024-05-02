#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <Windows.h>

// http://www.6502.org/users/obelisk/6502/index.html
// http://www.6502.org/source/
// https://skilldrick.github.io/easy6502/

// There are "set to" and "set if", which logically different, but now done as the same.
// Decimal mode is depricated:
//   1. CLD and SED are removed
//   2. ADC and SBC are always in binary

using u8	= unsigned char;
using u16	= unsigned short;
using u32	= unsigned int;
using u64	= unsigned long long;
using i8	= char;
using i16	= short;
using i32	= int;
using i64	= long long;

constexpr u32 BIT_SIZE = 8;
constexpr u32 MAX_RAM_BYTES = 65'536;

struct ram
{
	u8* data = new u8[MAX_RAM_BYTES];

	ram();

	~ram();

	void clear();

	u8 operator [] (u32 idx) const;

	u8& operator [] (u32 idx);

	void write_to(const std::string& path);
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

	// automatically offsets for stack
	void push(u8 bt);

	// automatically offsets for stack
	u8 pull();

	void exe_op(u8 op);

	void start(i32 operators = 0x7FFFFFFF);
};

/// <summary>
/// Looks at bytes in memory from 'mem_addr' to draw pixels.
/// One byte - one pixel.
/// </summary>
struct renderer
{
	ram& mem;
	u16 addr;

	u16 size_x = 0;
	u16 size_y = 0;
	u8 scale_x = 1;
	u8 scale_y = 1;

	HDC hdc;

	renderer(ram& mem_ref, u16 new_addr, HDC new_hdc, u16 new_size_x = 0, u16 new_size_y = 0, u8 new_scale_x = 1, u8 new_scale_y = 1);

	void draw();
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

	constexpr u8 EOR_IM		= 0x49;	// logical xor - immediate
	constexpr u8 EOR_ZP		= 0x45;	// logical xor - zero page
	constexpr u8 EOR_ZP_X	= 0x55;	// logical xor - zero page, x
	constexpr u8 EOR_ABS	= 0x4D;	// logical xor - absolute
	constexpr u8 EOR_ABS_X	= 0x5D;	// logical xor - absolute, x
	constexpr u8 EOR_ABS_Y	= 0x59;	// logical xor - absolute, y
	constexpr u8 EOR_IN_X	= 0x41;	// logical xor - indirect, x
	constexpr u8 EOR_IN_Y	= 0x51;	// logical xor - indirect, y

	constexpr u8 ORA_IM		= 0x09;	// logical or - immediate
	constexpr u8 ORA_ZP		= 0x05;	// logical or - zero page
	constexpr u8 ORA_ZP_X	= 0x15;	// logical or - zero page, x
	constexpr u8 ORA_ABS	= 0x0D;	// logical or - absolute
	constexpr u8 ORA_ABS_X	= 0x1D;	// logical or - absolute, x
	constexpr u8 ORA_ABS_Y	= 0x19;	// logical or - absolute, y
	constexpr u8 ORA_IN_X	= 0x01;	// logical or - indirect, x
	constexpr u8 ORA_IN_Y	= 0x11;	// logical or - indirect, y

	constexpr u8 BIT_ZP		= 0x24; // bit test - zero page
	constexpr u8 BIT_ABS	= 0x2C; // bit test - absolute

	// arithmetic
	constexpr u8 ADC_IM		= 0x69; // add with carry - immediate
	constexpr u8 ADC_ZP		= 0x65; // add with carry - zero page
	constexpr u8 ADC_ZP_X	= 0x75; // add with carry - zero page, x
	constexpr u8 ADC_ABS	= 0x6D; // add with carry - absolute
	constexpr u8 ADC_ABS_X	= 0x7D; // add with carry - absolute, x
	constexpr u8 ADC_ABS_Y	= 0x79; // add with carry - absolute, y
	constexpr u8 ADC_IN_X	= 0x61; // add with carry - indirect, x
	constexpr u8 ADC_IN_Y	= 0x71; // add with carry - indirect, y

	constexpr u8 SBC_IM		= 0xE9; // subtract with carry - immidiate
	constexpr u8 SBC_ZP		= 0xE5; // subtract with carry - zero page
	constexpr u8 SBC_ZP_X	= 0xF5; // subtract with carry - zero page, x
	constexpr u8 SBC_ABS	= 0xED; // subtract with carry - absolute
	constexpr u8 SBC_ABS_X	= 0xFD; // subtract with carry - absolute, x
	constexpr u8 SBC_ABS_Y	= 0xF9; // subtract with carry - absolute, y
	constexpr u8 SBC_IN_X	= 0xE1; // subtract with carry - indirect, x
	constexpr u8 SBC_IN_Y	= 0xF1; // subtract with carry - indirect, y

	constexpr u8 CMP_IM		= 0xC9; // compare accumulator - immidiate
	constexpr u8 CMP_ZP		= 0xC5; // compare accumulator - zero page
	constexpr u8 CMP_ZP_X	= 0xD5; // compare accumulator - zero page, x
	constexpr u8 CMP_ABS	= 0xCD; // compare accumulator - absolute
	constexpr u8 CMP_ABS_X	= 0xDD; // compare accumulator - absolute, x
	constexpr u8 CMP_ABS_Y	= 0xD9; // compare accumulator - absolute, y
	constexpr u8 CMP_IN_X	= 0xC1; // compare accumulator - indirect, x
	constexpr u8 CMP_IN_Y	= 0xD1; // compare accumulator - indirect, y

	constexpr u8 CPX_IM		= 0xE0; // compare x register - immidiate
	constexpr u8 CPX_ZP		= 0xE4; // compare x register - zero page
	constexpr u8 CPX_ABS	= 0xEC; // compare x register - absolute

	constexpr u8 CPY_IM		= 0xC0; // compare y register - immidiate
	constexpr u8 CPY_ZP		= 0xC4; // compare y register - zero page
	constexpr u8 CPY_ABS	= 0xCC; // compare y register - absolute

	// increments & decrements
	constexpr u8 INC_ZP		= 0xE6; // increment memory - zero page
	constexpr u8 INC_ZP_X	= 0xF6; // increment memory - zero page, x
	constexpr u8 INC_ABS	= 0xEE; // increment memory - absolute
	constexpr u8 INC_ABS_X	= 0xFE; // increment memory - absolute, x

	constexpr u8 INX		= 0xE8; // increment x register
	constexpr u8 INY		= 0xC8; // increment y register

	constexpr u8 DEC_ZP		= 0xC6; // deccrement memory - zero page
	constexpr u8 DEC_ZP_X	= 0xD6; // deccrement memory - zero page, x
	constexpr u8 DEC_ABS	= 0xCE; // deccrement memory - absolute
	constexpr u8 DEC_ABS_X	= 0xDE; // deccrement memory - absolute, x

	constexpr u8 DEX		= 0xCA; // decrement x register
	constexpr u8 DEY		= 0x88; // decrement y register

	// shifts
	constexpr u8 ASL_A		= 0x0A;	// arithmetic shift left - accumulator
	constexpr u8 ASL_ZP		= 0x06;	// arithmetic shift left - zero page
	constexpr u8 ASL_ZP_X	= 0x16;	// arithmetic shift left - zero page, x
	constexpr u8 ASL_ABS	= 0x0E;	// arithmetic shift left - absolute
	constexpr u8 ASL_ABS_X	= 0x1E;	// arithmetic shift left - absolute, x

	constexpr u8 LSR_A		= 0x4A;	// logical shift right - accumulator
	constexpr u8 LSR_ZP		= 0x46;	// logical shift right - zero page
	constexpr u8 LSR_ZP_X	= 0x56;	// logical shift right - zero page, x
	constexpr u8 LSR_ABS	= 0x4E;	// logical shift right - absolute
	constexpr u8 LSR_ABS_X	= 0x5E;	// logical shift right - absolute, x

	constexpr u8 ROL_A		= 0x2A;	// rotate left - accumulator
	constexpr u8 ROL_ZP		= 0x26;	// rotate left - zero page
	constexpr u8 ROL_ZP_X	= 0x36;	// rotate left - zero page, x
	constexpr u8 ROL_ABS	= 0x2E;	// rotate left - absolute
	constexpr u8 ROL_ABS_X	= 0x3E;	// rotate left - absolute, x

	constexpr u8 ROR_A		= 0x6A;	// rotate right - accumulator
	constexpr u8 ROR_ZP		= 0x66;	// rotate right - zero page
	constexpr u8 ROR_ZP_X	= 0x76;	// rotate right - zero page, x
	constexpr u8 ROR_ABS	= 0x6E;	// rotate right - absolute
	constexpr u8 ROR_ABS_X	= 0x7E;	// rotate right - absolute, x

	// jumps & calls
	constexpr u8 JMP_ABS	= 0x4C; // jump - absolute
	constexpr u8 JMP_IN		= 0x6C; // jump - indirect
	constexpr u8 JSR_ABS	= 0x20;	// jump to subroutine - absolute
	constexpr u8 RTS		= 0x60;	// return from subroutine

	// branches
	constexpr u8 BCC		= 0x90; // branch if carry clear
	constexpr u8 BCS		= 0xB0; // branch if carry set
	constexpr u8 BEQ		= 0xF0; // branch if zero set (branch if equal)
	constexpr u8 BMI		= 0x30; // branch if negative set (branch if minus)
	constexpr u8 BNE		= 0xD0; // branch if zero clear (branch if not equal)
	constexpr u8 BPL		= 0x10; // branch if negative clear (branch if positive)
	constexpr u8 BVC		= 0x50; // branch if overflow clear
	constexpr u8 BVS		= 0x70; // branch if overflow set

	// status flag changes
	constexpr u8 CLC		= 0x18; // clear carry flag
	constexpr u8 CLI		= 0x58; // clear interrupt disable flag
	constexpr u8 CLV		= 0xB8; // clear overflow flag
	constexpr u8 SEC		= 0x38; // set carry flag
	constexpr u8 SEI		= 0x78; // set interrupt disable flag

	// system functions
	constexpr u8 BRK		= 0x00; // force interrupt
	constexpr u8 RTI		= 0x40; // return from interrupt
	constexpr u8 NOP		= 0xEA; // no operator
	constexpr u8 KIL		= 0xFF;	// kill process - unofficial
}