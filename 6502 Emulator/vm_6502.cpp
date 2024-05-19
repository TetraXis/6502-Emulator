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

void ram::write_to(const std::string& path)
{
	std::ofstream fout(path, std::ios::binary);
	for (u32 i = 0; i <= 0xFFFF; i++)
	{
		fout << data[i];
	}
	fout.close();
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
	mem[u16(0x0100) + (sp--)] = bt;
}

u8 cpu::pull()
{
	return mem[u16(0x0100) + (++sp)];
}

void cpu::exe_op(u8 op)
{
#ifdef DEBUG_6502
	std::cout << std::hex << "executing " << u16(op) << '\n';
#endif //DEBUG_6502
	if (op_map.find(op) == op_map.end())
	{
		std::cerr << std::hex << "Unknown operator \'" << u16(op) << "\' at \'" << pc << "\'\n";
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

renderer::renderer(ram& mem_ref, u16 new_addr, HDC new_hdc, u16 new_size_x, u16 new_size_y, u8 new_scale_x, u8 new_scale_y)
	: mem(mem_ref), addr(new_addr), hdc(new_hdc), size_x(new_size_x), size_y(new_size_y), scale_x(new_scale_x), scale_y(new_scale_y)
{
}

void renderer::draw()
{
	for (u16 i = 0; i < size_x * size_y; i++)
	{
		//Rectangle(hdc, (i % size_x) * scale_x, (i / size_x) * scale_y, i % size_x, i / size_x, )
		/*SetPixel(hdc, i % size_x, i / size_x,
			RGB
			(
				((mem[addr + i] & 0b11100000) >> 5)	* 256 / 8,
				((mem[addr + i] & 0b00011100) >> 2)	* 256 / 8,
				(mem[addr + i] & 0b00000011)		* 256 / 4
			)
		);*/
	}
}
