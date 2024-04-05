#include "vm_6502.h"

ram::ram()
{
	clear();
}

ram::~ram()
{
	delete data;
}

void ram::clear()
{
	memset(data, 0, MAX_RAM_BYTES);
}

u8 ram::operator[](u32 idx) const
{
	if (idx >= MAX_RAM_BYTES)
	{
		std::cerr << "Memory accessed out of bounds." << '\n';
		system("pause");
		return data[MAX_RAM_BYTES - 1];
	}

	return data[idx];
}

u8& ram::operator[](u32 idx)
{
	if (idx >= MAX_RAM_BYTES)
	{
		std::cerr << "Memory accessed out of bounds." << '\n';
		system("pause");
		return data[MAX_RAM_BYTES - 1];
	}

	return data[idx];
}

cpu::cpu(ram& mem_ref) : mem(mem_ref)
{
	reset();
}

void cpu::reset()
{
	pc = u16(0xFFFC);
	sp = u8(0xFF);

	a = u8(0);
	x = u8(0);
	y = u8(0);

	c = false;
	z = false;
	i = false;
	d = false;
	b = false;
	v = false;
	n = false;
	k = false;
}

u8 cpu::next_byte()
{
	return mem[++pc];
}

void cpu::push(u8 bt)
{
	mem[0x0100 + (sp--)] = bt;
}

u8 cpu::pull()
{
	return mem[0x0100 + (++sp)];
}

void cpu::exe_op(u8 op)
{
	if (op_map.find(op) == op_map.end())
	{
		std::cerr << std::hex << "Unknown operator \'" << op << "\' at \'" << pc << "\'\n";
		system("pause");
		return;
	}
	op_map.at(op)(*this);
}

void cpu::start(i32 operators)
{
	for (u8 op = mem[pc]; operators > 0 && !k; op = next_byte(), operators--)
	{
		exe_op(op);
	}
}
