#include <iostream>

#include "vm_6502.h"
#include "compiler_6502.h"

#define REND_ADDR 0x8F00
#define AMOUNT 12

int main()
{
	ram ram_0;
	cpu cpu_0(ram_0);
	compiler cmplr;

	if (cmplr.compile_and_build("input.txt", cpu_0))
	{
		timer tm;
		tm.start();
		cmplr.write_txt("output.txt");
		cpu_0.start();
		tm.stop();
		std::cout << "\nCPU running time: " << tm.elapsed_milliseconds() << "ms.\n";
		ram_0.write_to("memory_shapshot.bin");
	}

	std::cout << "\nRegisters:\n";
	std::cout << "a: " << (int)cpu_0.a << '\n';
	std::cout << "x: " << (int)cpu_0.x << '\n';
	std::cout << "y: " << (int)cpu_0.y << '\n';

	std::cout << "\nZero page:\n";
	for (u64 i = 0; i < AMOUNT; i++)
	{
		std::cout << i << ":\t" << (int)ram_0[i] << '\n';
	}

	system("pause");
	return 0;
}
