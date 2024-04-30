//#include "no_sillywarnings_please.h"
#include <iostream>

#include "vm_6502.h"
#include "compiler_6502.h"

#define REND_ADDR 0x8F00

int main()
{
	ram rm;
	compiler cmplr;
	std::cout << cmplr.compile("input.txt", rm) << '\n';

	cpu cpu_0(rm);
	cpu_0.start();

	std::cout << (int)cpu_0.a << '\n';

	return 0;




	ram mem;
	renderer ren(mem, REND_ADDR, GetDC(GetConsoleWindow()), 128, 128);

	mem[0xFFFF] = op::KIL;
	mem[0xFFFC] = op::JSR_ABS;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x02;
	mem[0x0200] = op::LDA_IM;
	mem[0x0201] = 0xFF;
	mem[0x0202] = op::NOP;
	mem[0x0203] = op::LSR_A;
	mem[0x0204] = op::RTS;

	mem[REND_ADDR]		= 0b11100000;
	mem[REND_ADDR + 1]	= 0b11100000;
	mem[REND_ADDR + 2]	= 0b11100000;
	mem[REND_ADDR + 3]	= 0b00011100;
	mem[REND_ADDR + 4]	= 0b00011100;
	mem[REND_ADDR + 5]	= 0b11100011;

	cpu cpu1(mem);

	//ren.draw();

	cpu1.start();

	std::cout << (int)cpu1.a << '\n';
	std::cout << (int)cpu1.c << '\n';
	std::cout << (int)cpu1.z << '\n';
	std::cout << (int)cpu1.v << '\n';
	std::cout << (int)cpu1.n << '\n';

	system("pause");
	return 0;
}
