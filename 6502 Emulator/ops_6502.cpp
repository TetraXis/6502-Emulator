#include "vm_6502.h"

// NOTE: now expecting that evaluation order inside an expression is sequenced
// NOTE: jmp sets pc to addr - 1, because counter will automaticaly increment

/* Inderect addressing examples

	u8 addr = c.mem[u8(c.next_byte() + c.x)]; // address of least significant byte
	c.a = c.mem[addr | ((addr + 1) << BIT_SIZE)];

	u8 addr = c.mem[u8(c.next_byte())]; // address of least significant byte
	c.a = c.mem[(addr | ((addr + 1) << BIT_SIZE)) + c.y];

*/



#define NEXT_WORD (c.next_byte() | (c.next_byte() << BIT_SIZE))
#define POP_WORD ((c.pull() << BIT_SIZE) | c.pull())
#define PROCESS_STATUS ((c.n << 7) | (c.v << 6) | (c.k << 5) | (c.b << 4) | (c.d << 3) | (c.i << 2) | (c.z << 1) | (c.c))

#define GET_INDIRECT_ADDR_X u8 addr = c.mem[u8(c.next_byte() + c.x)];
#define GET_INDIRECT_ADDR_Y u8 addr = c.mem[u8(c.next_byte())];
#define CONCAT_ADDR (addr | ((addr + 1) << BIT_SIZE))

std::map<u8, std::function<void(cpu& cpu_ref)>> cpu::op_map =
{
	{
		op::LDA_IM,
		[](cpu& c)
		{
			c.a = c.next_byte();
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::LDA_ZP,
		[](cpu& c)
		{
			c.a = c.mem[c.next_byte()];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::LDA_ZP_X,
		[](cpu& c)
		{
			c.a = c.mem[u8(c.next_byte() + c.x)];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::LDA_ABS,
		[](cpu& c)
		{
			c.a = c.mem[NEXT_WORD];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::LDA_ABS_X,
		[](cpu& c)
		{
			c.a = c.mem[NEXT_WORD + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::LDA_ABS_Y,
		[](cpu& c)
		{
			c.a = c.mem[NEXT_WORD + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
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
			c.n = (c.a & 0x80);
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
			c.n = (c.a & 0x80);
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
			c.n = (c.x & 0x80);
			return;
		}
	},
	{
		op::LDX_ZP,
		[](cpu& c)
		{
			c.x = c.mem[c.next_byte()];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80);
			return;
		}
	},
	{
		op::LDX_ZP_Y,
		[](cpu& c)
		{
			c.x = c.mem[c.next_byte() + c.y];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80);
			return;
		}
	},
	{
		op::LDX_ABS,
		[](cpu& c)
		{
			c.x = c.mem[NEXT_WORD];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80);
			return;
		}
	},
	{
		op::LDX_ABS_Y,
		[](cpu& c)
		{
			c.x = c.mem[NEXT_WORD + c.y];
			c.z = (c.x == 0);
			c.n = (c.x & 0x80);
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
			c.n = (c.y & 0x80);
			return;
		}
	},
	{
		op::LDY_ZP,
		[](cpu& c)
		{
			c.y = c.mem[c.next_byte()];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80);
			return;
		}
	},
	{
		op::LDY_ZP_X,
		[](cpu& c)
		{
			c.y = c.mem[c.next_byte() + c.x];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80);
			return;
		}
	},
	{
		op::LDY_ABS,
		[](cpu& c)
		{
			c.y = c.mem[NEXT_WORD];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80);
			return;
		}
	},
	{
		op::LDY_ABS_X,
		[](cpu& c)
		{
			c.y = c.mem[NEXT_WORD + c.x];
			c.z = (c.y == 0);
			c.n = (c.y & 0x80);
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
			c.n = (c.x & 0x80);
			return;
		}
	},
	{
		op::TAY,
		[](cpu& c)
		{
			c.y = c.a;
			c.z = (c.y == 0);
			c.n = (c.y & 0x80);
			return;
		}
	},
	{
		op::TXA,
		[](cpu& c)
		{
			c.a = c.x;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::TYA,
		[](cpu& c)
		{
			c.a = c.y;
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::TSX,
		[](cpu& c)
		{
			c.x = c.sp;
			c.z = (c.x == 0);
			c.n = (c.x & 0x80);
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
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::PLP,
		[](cpu& c)
		{
			u8 temp = c.pull();
			c.n = temp & (1 << 7);
			c.v = temp & (1 << 6);
			c.k = temp & (1 << 5);
			c.b = temp & (1 << 4);
			c.d = temp & (1 << 3);
			c.i = temp & (1 << 2);
			c.z = temp & (1 << 1);
			c.c = temp & (1);
			return;
		}
	},
	{
		op::AND_IM,
		[](cpu& c)
		{
			c.a &= c.next_byte();
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::AND_ZP,
		[](cpu& c)
		{
			c.a &= c.mem[c.next_byte()];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::AND_ZP_X,
		[](cpu& c)
		{
			c.a &= c.mem[c.next_byte() + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::AND_ABS,
		[](cpu& c)
		{
			c.a &= c.mem[NEXT_WORD];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::AND_ABS_X,
		[](cpu& c)
		{
			c.a &= c.mem[NEXT_WORD + c.x];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::AND_ABS_Y,
		[](cpu& c)
		{
			c.a &= c.mem[NEXT_WORD + c.y];
			c.z = (c.a == 0);
			c.n = (c.a & 0x80);
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
			c.n = (c.a & 0x80);
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
			c.n = (c.a & 0x80);
			return;
		}
	},
	{
		op::ASL_A,
		[](cpu& c)
		{
			c.c = c.a & 0x80;
			c.a <<= 1;
			c.z = (c.a == 0);
			c.n = c.a & 0x80;
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
			c.pc = (c.mem[c.next_byte()] | (c.mem[c.next_byte()] << BIT_SIZE)) - 1;
			return;
		}
	},
	{
		op::JSR,
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