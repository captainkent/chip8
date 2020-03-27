#include "CMemory.h"

CMemory::CMemory()
{
	// Initialise our memory to 0.
	the_memory = {};
}

void
CMemory::load_data(std::vector<uint8_t> a_data)
{
	for (int i = 0; i < a_data.size(); i++)
		set_byte(0x200 + i, a_data[i]);
}

uint8_t
CMemory::get_byte(int an_index)
{
	return the_memory[an_index];
}

void
CMemory::set_byte(int an_index, uint8_t a_value)
{
	the_memory[an_index] = a_value;
}

uint16_t
CMemory::get_opcode(int a_program_counter)
{
	// Fetch the opcode.
	return the_memory[a_program_counter] << 8 | the_memory[a_program_counter + 1];
}

size_t
CMemory::get_size()
{
	return the_memory.size();
}

