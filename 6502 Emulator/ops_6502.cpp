#include "vm_6502.h"

// NOTE: now expecting that evaluation order inside an expression is sequenced
// NOTE: jmp sets pc to addr - 1, because counter will automaticaly increment

#define NEXT_WORD (c.next_byte() | (c.next_byte() << BIT_SIZE))
#define POP_WORD ((c.pull() << BIT_SIZE) | c.pull())
#define PROCESS_STATUS ((c.n << 7) | (c.v << 6) | (c.k << 5) | (c.b << 4) | (c.d << 3) | (c.i << 2) | (c.z << 1) | (c.c))

#define GET_INDIRECT_ADDR_X u8 addr = u8(c.next_byte() + c.x);
#define GET_INDIRECT_ADDR_Y u8 addr = u8(c.next_byte());
#define CONCAT_ADDR (c.mem[addr] | ((c.mem[addr + 1]) << BIT_SIZE))

std::map<u8, std::function<void(cpu& cpu_ref)>> cpu::op_map =
{
	{
		op::LDA_IM,
		[](cpu& c)
		{
			c.a = c.next_byte();
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LDA_ZP,
		[](cpu& c)
		{
			c.a = c.mem[c.next_byte()];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LDA_ZP_X,
		[](cpu& c)
		{
			c.a = c.mem[u8(c.next_byte() + c.x)];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LDA_ABS,
		[](cpu& c)
		{
			c.a = c.mem[NEXT_WORD];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LDA_ABS_X,
		[](cpu& c)
		{
			c.a = c.mem[NEXT_WORD + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LDA_ABS_Y,
		[](cpu& c)
		{
			c.a = c.mem[NEXT_WORD + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LDA_IN_X,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_X;
			c.a = c.mem[CONCAT_ADDR];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LDA_IN_Y,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_Y;
			c.a = c.mem[CONCAT_ADDR + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::STA_ZP,
		[](cpu& c)
		{
			c.mem[c.next_byte()] = c.a;
			return;
		}
	},
	{
		op::STA_ZP_X,
		[](cpu& c)
		{
			c.mem[c.next_byte() + c.x] = c.a;
			return;
		}
	},
	{
		op::STA_ABS,
		[](cpu& c)
		{
			c.mem[NEXT_WORD] = c.a;
			return;
		}
	},
	{
		op::STA_ABS_X,
		[](cpu& c)
		{
			c.mem[NEXT_WORD + c.x] = c.a;
			return;
		}
	},
	{
		op::STA_ABS_Y,
		[](cpu& c)
		{
			c.mem[NEXT_WORD + c.y] = c.a;
			return;
		}
	},
	{
		op::STA_IN_X,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_X;
			c.mem[CONCAT_ADDR] = c.a;
			return;
		}
	},
	{
		op::STA_IN_Y,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_Y;
			c.mem[CONCAT_ADDR + c.y] = c.a;
			return;
		}
	},	
	{
		op::LDX_IM,
		[](cpu& c)
		{
			c.x = c.next_byte();
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::LDX_ZP,
		[](cpu& c)
		{
			c.x = c.mem[c.next_byte()];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::LDX_ZP_Y,
		[](cpu& c)
		{
			c.x = c.mem[c.next_byte() + c.y];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::LDX_ABS,
		[](cpu& c)
		{
			c.x = c.mem[NEXT_WORD];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::LDX_ABS_Y,
		[](cpu& c)
		{
			c.x = c.mem[NEXT_WORD + c.y];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::STX_ZP,
		[](cpu& c)
		{
			c.mem[c.next_byte()] = c.x;
			return;
		}
	},
	{
		op::STX_ZP_Y,
		[](cpu& c)
		{
			c.mem[c.next_byte() + c.y] = c.x;
			return;
		}
	},
	{
		op::STX_ABS,
		[](cpu& c)
		{
			c.mem[NEXT_WORD] = c.x;
			return;
		}
	},
	{
		op::LDY_IM,
		[](cpu& c)
		{
			c.y = c.next_byte();
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::LDY_ZP,
		[](cpu& c)
		{
			c.y = c.mem[c.next_byte()];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::LDY_ZP_X,
		[](cpu& c)
		{
			c.y = c.mem[c.next_byte() + c.x];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::LDY_ABS,
		[](cpu& c)
		{
			c.y = c.mem[NEXT_WORD];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::LDY_ABS_X,
		[](cpu& c)
		{
			c.y = c.mem[NEXT_WORD + c.x];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::STY_ZP,
		[](cpu& c)
		{
			c.mem[c.next_byte()] = c.y;
			return;
		}
	},
	{
		op::STY_ZP_X,
		[](cpu& c)
		{
			c.mem[c.next_byte() + c.x] = c.y;
			return;
		}
	},
	{
		op::STY_ABS,
		[](cpu& c)
		{
			c.mem[NEXT_WORD] = c.y;
			return;
		}
	},
	{
		op::TAX,
		[](cpu& c)
		{
			c.x = c.a;
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::TAY,
		[](cpu& c)
		{
			c.y = c.a;
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::TXA,
		[](cpu& c)
		{
			c.a = c.x;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::TYA,
		[](cpu& c)
		{
			c.a = c.y;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::TSX,
		[](cpu& c)
		{
			c.x = c.sp;
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::TXS,
		[](cpu& c)
		{
			c.sp = c.x;
			return;
		}
	},
	{
		op::PHA,
		[](cpu& c)
		{
			c.push(c.a);
			return;
		}
	},
	{
		op::PHP,
		[](cpu& c)
		{
			c.push(PROCESS_STATUS);
			return;
		}
	},
	{
		op::PLA,
		[](cpu& c)
		{
			c.a = c.pull();
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::PLP,
		[](cpu& c)
		{
			u8 temp = c.pull();
			c.n = (temp & (1 << 7)) != 0;
			c.v = (temp & (1 << 6)) != 0;
			c.k = (temp & (1 << 5)) != 0;
			c.b = (temp & (1 << 4)) != 0;
			c.d = (temp & (1 << 3)) != 0;
			c.i = (temp & (1 << 2)) != 0;
			c.z = (temp & (1 << 1)) != 0;
			c.c = (temp & (1)) != 0;
			return;
		}
	},
	{
		op::AND_IM,
		[](cpu& c)
		{
			c.a &= c.next_byte();
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::AND_ZP,
		[](cpu& c)
		{
			c.a &= c.mem[c.next_byte()];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::AND_ZP_X,
		[](cpu& c)
		{
			c.a &= c.mem[c.next_byte() + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::AND_ABS,
		[](cpu& c)
		{
			c.a &= c.mem[NEXT_WORD];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::AND_ABS_X,
		[](cpu& c)
		{
			c.a &= c.mem[NEXT_WORD + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::AND_ABS_Y,
		[](cpu& c)
		{
			c.a &= c.mem[NEXT_WORD + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::AND_IN_X,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_X;
			c.a &= c.mem[CONCAT_ADDR];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::AND_IN_Y,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_Y;
			c.a &= c.mem[CONCAT_ADDR + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_IM,
		[](cpu& c)
		{
			c.a ^= c.next_byte();
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_ZP,
		[](cpu& c)
		{
			c.a ^= c.mem[c.next_byte()];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_ZP_X,
		[](cpu& c)
		{
			c.a ^= c.mem[c.next_byte() + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_ABS,
		[](cpu& c)
		{
			c.a ^= c.mem[NEXT_WORD];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_ABS_X,
		[](cpu& c)
		{
			c.a ^= c.mem[NEXT_WORD + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_ABS_Y,
		[](cpu& c)
		{
			c.a ^= c.mem[NEXT_WORD + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_IN_X,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_X;
			c.a ^= c.mem[CONCAT_ADDR];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::EOR_IN_Y,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_Y;
			c.a ^= c.mem[CONCAT_ADDR + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_IM,
		[](cpu& c)
		{
			c.a |= c.next_byte();
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_ZP,
		[](cpu& c)
		{
			c.a |= c.mem[c.next_byte()];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_ZP_X,
		[](cpu& c)
		{
			c.a |= c.mem[c.next_byte() + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_ABS,
		[](cpu& c)
		{
			c.a |= c.mem[NEXT_WORD];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_ABS_X,
		[](cpu& c)
		{
			c.a |= c.mem[NEXT_WORD + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_ABS_Y,
		[](cpu& c)
		{
			c.a |= c.mem[NEXT_WORD + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_IN_X,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_X;
			c.a |= c.mem[CONCAT_ADDR];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ORA_IN_Y,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_Y;
			c.a |= c.mem[CONCAT_ADDR + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::BIT_ZP,
		[](cpu& c)
		{
			u8 temp = c.mem[c.next_byte()];
			c.z = ((c.a & temp) == 0);
			c.v = (temp & 0b01000000) != 0;
			c.n = (temp & 0b10000000) != 0;
			return;
		}
	},
	{
		op::BIT_ABS,
		[](cpu& c)
		{
			u8 temp = c.mem[NEXT_WORD];
			c.z = ((c.a & temp) == 0);
			c.v = (temp & 0b01000000) != 0;
			c.n = (temp & 0b10000000) != 0;
			return;
		}
	},
	{
		op::ADC_IM,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.next_byte();
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ADC_ZP,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[c.next_byte()];
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ADC_ZP_X,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[c.next_byte() + c.x];
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ADC_ABS,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[NEXT_WORD];
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ADC_ABS_X,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[NEXT_WORD + c.x];
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ADC_ABS_Y,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[NEXT_WORD + c.y];
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ADC_IN_X,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			GET_INDIRECT_ADDR_X;
			u8 value = c.mem[CONCAT_ADDR];
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ADC_IN_Y,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			GET_INDIRECT_ADDR_Y;
			u8 value = c.mem[CONCAT_ADDR + c.y];
			c.a += value + c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ((u16(prev_a) + value + c.c) & 0xFF00) != 0;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_IM,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.next_byte();
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_ZP,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[c.next_byte()];
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_ZP_X,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[c.next_byte() + c.x];
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_ABS,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[NEXT_WORD];
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_ABS_X,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[NEXT_WORD + c.x];
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_ABS_Y,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			u8 value = c.mem[NEXT_WORD + c.y];
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_IN_X,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			GET_INDIRECT_ADDR_X;
			u8 value = c.mem[CONCAT_ADDR];
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::SBC_IN_Y,
		[](cpu& c)
		{
			u8 prev_a = c.a;
			GET_INDIRECT_ADDR_Y;
			u8 value = c.mem[CONCAT_ADDR + c.y];
			c.a -= value + !c.c;
			c.v = ( (prev_a ^ c.a) & (value ^ c.a) & 0x80 ) != 0;
			c.c = ~c.v;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_IM,
		[](cpu& c)
		{
			u8 value = c.next_byte();
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_ZP,
		[](cpu& c)
		{
			u8 value = c.mem[c.next_byte()];
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_ZP_X,
		[](cpu& c)
		{
			u8 value = c.mem[c.next_byte() + c.x];
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_ABS,
		[](cpu& c)
		{
			u8 value = c.mem[NEXT_WORD];
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_ABS_X,
		[](cpu& c)
		{
			u8 value = c.mem[NEXT_WORD + c.x];
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_ABS_Y,
		[](cpu& c)
		{
			u8 value = c.mem[NEXT_WORD + c.y];
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_IN_X,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_X;
			u8 value = c.mem[CONCAT_ADDR];
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CMP_IN_Y,
		[](cpu& c)
		{
			GET_INDIRECT_ADDR_Y;
			u8 value = c.mem[CONCAT_ADDR + c.y];
			c.c = (c.a >= value);
			c.z = (c.a == value);
			c.n = ((c.a - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CPX_IM,
		[](cpu& c)
		{
			u8 value = c.next_byte();
			c.c = (c.x >= value);
			c.z = (c.x == value);
			c.n = ((c.x - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CPX_ZP,
		[](cpu& c)
		{
			u8 value = c.mem[c.next_byte()];
			c.c = (c.x >= value);
			c.z = (c.x == value);
			c.n = ((c.x - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CPX_ABS,
		[](cpu& c)
		{
			u8 value = c.mem[NEXT_WORD];
			c.c = (c.x >= value);
			c.z = (c.x == value);
			c.n = ((c.x - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CPY_IM,
		[](cpu& c)
		{
			u8 value = c.next_byte();
			c.c = (c.y >= value);
			c.z = (c.y == value);
			c.n = ((c.y - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CPY_ZP,
		[](cpu& c)
		{
			u8 value = c.mem[c.next_byte()];
			c.c = (c.y >= value);
			c.z = (c.y == value);
			c.n = ((c.y - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::CPY_ABS,
		[](cpu& c)
		{
			u8 value = c.mem[NEXT_WORD];
			c.c = (c.y >= value);
			c.z = (c.y == value);
			c.n = ((c.y - value) & 0x80) != 0;
			return;
		}
	},
	{
		op::INC_ZP,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte()];
			++addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::INC_ZP_X,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte() + c.x];
			++addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::INC_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD];
			++addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::INC_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD + c.x];
			++addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::INX,
		[](cpu& c)
		{
			++c.x;
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::INY,
		[](cpu& c)
		{
			++c.y;
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::DEC_ZP,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte()];
			--addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::DEC_ZP_X,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte() + c.x];
			--addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::DEC_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD];
			--addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::DEC_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD + c.x];
			--addr;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::DEX,
		[](cpu& c)
		{
			--c.x;
			c.z = (c.x == 0);
			c.n = (c.x & 0x80) != 0;
			return;
		}
	},
	{
		op::DEY,
		[](cpu& c)
		{
			--c.y;
			c.z = (c.y == 0);
			c.n = (c.y & 0x80) != 0;
			return;
		}
	},
	{
		op::ASL_A,
		[](cpu& c)
		{
			c.c = (c.a & 0x80) != 0;
			c.a <<= 1;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ASL_ZP,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte()];
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ASL_ZP_X,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte() + c.x];
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ASL_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD];
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ASL_ABS_X,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD + c.x];
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::LSR_A,
		[](cpu& c)
		{
			c.c = (c.a & 0x01) != 0;
			c.a >>= 1;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::LSR_ZP,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte()];
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::LSR_ZP_X,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte() + c.x];
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::LSR_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD];
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::LSR_ABS_X,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD + c.x];
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROL_A,
		[](cpu& c)
		{
			u8 old_c = c.c;
			c.c = (c.a & 0x80) != 0;
			c.a <<= 1;
			c.a |= old_c;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ROL_ZP,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte()];
			u8 old_c = c.c;
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			addr |= old_c;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROL_ZP_X,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte() + c.x];
			u8 old_c = c.c;
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			addr |= old_c;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROL_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD];
			u8 old_c = c.c;
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			addr |= old_c;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROL_ABS_X,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD + c.x];
			u8 old_c = c.c;
			c.c = (addr & 0x80) != 0;
			addr <<= 1;
			addr |= old_c;
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROR_A,
		[](cpu& c)
		{
			u8 old_c = c.c;
			c.c = (c.a & 0x01) != 0;
			c.a >>= 1;
			c.a |= (old_c << (BIT_SIZE - 1));
			c.z = (c.a == 0);
			c.n = (c.a & 0x80) != 0;
			return;
		}
	},
	{
		op::ROR_ZP,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte()];
			u8 old_c = c.c;
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			addr |= (old_c << (BIT_SIZE - 1));
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROR_ZP_X,
		[](cpu& c)
		{
			u8& addr = c.mem[c.next_byte() + c.x];
			u8 old_c = c.c;
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			addr |= (old_c << (BIT_SIZE - 1));
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROR_ABS,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD];
			u8 old_c = c.c;
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			addr |= (old_c << (BIT_SIZE - 1));
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::ROR_ABS_X,
		[](cpu& c)
		{
			u8& addr = c.mem[NEXT_WORD + c.x];
			u8 old_c = c.c;
			c.c = (addr & 0x01) != 0;
			addr >>= 1;
			addr |= (old_c << (BIT_SIZE - 1));
			c.z = (addr == 0);
			c.n = (addr & 0x80) != 0;
			return;
		}
	},
	{
		op::JMP_ABS,
		[](cpu& c)
		{
			c.pc = NEXT_WORD - 1;
			return;
		}
	},
	{
		op::JMP_IN,
		[](cpu& c)
		{
			u16 addr = u16(c.mem[c.next_byte()]) | (c.mem[c.next_byte()] << BIT_SIZE);
			c.pc = (u16(c.mem[addr]) + (u16(c.mem[addr + 1]) << BIT_SIZE)) - 1;
			return;
		}
	},
	{
		op::JSR_ABS,
		[](cpu& c)
		{
			//     address of return point - 1
			//     \________/
			c.push((c.pc + 2) & u16(0x00FF));
			c.push((c.pc + 2) >> BIT_SIZE);
			c.pc = NEXT_WORD - 1;
			return;
		}
	},
	{
		op::RTS,
		[](cpu& c)
		{
			c.pc = POP_WORD;
			return;
		}
	},
	{
		op::BCC,
		[](cpu& c)
		{
			if (c.c == 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::BCS,
		[](cpu& c)
		{
			if (c.c != 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::BEQ,
		[](cpu& c)
		{
			if (c.z != 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::BMI,
		[](cpu& c)
		{
			if (c.n != 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::BNE,
		[](cpu& c)
		{
			if (c.z == 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::BPL,
		[](cpu& c)
		{
			if (c.n == 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::BVC,
		[](cpu& c)
		{
			if (c.v == 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::BVS,
		[](cpu& c)
		{
			if (c.v != 0)
			{
				c.pc += c.next_byte();
			}
			return;
		}
	},
	{
		op::CLC,
		[](cpu& c)
		{
			c.c = false;
			return;
		}
	},
	{
		op::CLI,
		[](cpu& c)
		{
			c.i = false;
			return;
		}
	},
	{
		op::CLV,
		[](cpu& c)
		{
			c.v = false;
			return;
		}
	},
	{
		op::SEC,
		[](cpu& c)
		{
			c.c = true;
			return;
		}
	},
	{
		op::SEI,
		[](cpu& c)
		{
			c.i = true;
			return;
		}
	},
	{
		op::BRK,
		[](cpu& c)
		{
			c.push(c.pc & 0xFF);
			c.push((c.pc & 0xFF00) >> BIT_SIZE);
			c.push(PROCESS_STATUS);
			c.pc = c.mem[0xFFFE];
			c.pc |= (c.mem[0xFFFF] << BIT_SIZE);
			c.b = true;
			return;
		}
	},
	{
		op::RTI,
		[](cpu& c)
		{
			u8 temp = c.pull();
			c.n = (temp & (1 << 7)) != 0;
			c.v = (temp & (1 << 6)) != 0;
			c.k = (temp & (1 << 5)) != 0;
			c.b = (temp & (1 << 4)) != 0;
			c.d = (temp & (1 << 3)) != 0;
			c.i = (temp & (1 << 2)) != 0;
			c.z = (temp & (1 << 1)) != 0;
			c.c = (temp & (1)) != 0;
			c.pc = ((c.pull() << BIT_SIZE) | c.pull());
			return;
		}
	},
	{
		op::NOP,
		[](cpu& c)
		{
			(void*)(&c);
			return;
		}
	},
	{
		op::KIL,
		[](cpu& c)
		{
			c.k = true;
			return;
		}
	}
};