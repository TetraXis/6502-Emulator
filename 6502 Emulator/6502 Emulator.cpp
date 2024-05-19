//#include "no_sillywarnings_please.h"
#include <iostream>

#include "vm_6502.h"
#include "compiler_6502.h"

#define REND_ADDR 0x8F00

int main()
{
	ram rm;
	cpu cpu_0(rm);
	compiler cmplr;
	if (cmplr.compile_and_build("input.txt", cpu_0))
	{
		//cmplr.write_txt("output.txt");
		cpu_0.start();
		rm.write_to("memory_shapshot.bin");
	}

	std::cout << "a: " << (int)cpu_0.a << '\n';
	std::cout << "x: " << (int)cpu_0.x << '\n';
	std::cout << "y: " << (int)cpu_0.y << '\n';

	system("pause");
	return 0;
}
