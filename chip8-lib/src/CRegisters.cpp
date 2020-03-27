#include "CRegisters.h"

CRegisters::CRegisters()
{
	the_registers = {};
}

void
CRegisters::set_register_value(int a_register, uint8_t a_value)
{
	the_registers[a_register] = a_value;
}

uint8_t
CRegisters::get_register_value(int a_register)
{
	return the_registers[a_register];
}