#include <stdio.h>
#include <iostream>

#include "vm_6502.h"

int main()
{
	ram mem;

	mem[0xFFFF] = op::KIL;
	mem[0xFFFC] = op::JSR;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x02;
	mem[0x0200] = op::LDA_IM;
	mem[0x0201] = 0x10;
	mem[0x0202] = op::ASL_A;
	mem[0x0203] = op::ASL_A;
	mem[0x0204] = op::RTS;

	cpu cpu1(mem);

	cpu1.start();

	std::cout << (int)cpu1.a << '\n';

	return 0;
}
