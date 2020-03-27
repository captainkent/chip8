#pragma once
#include <array>
#include <vector>

class CMemory {
	public:
		CMemory();
		~CMemory() = default;

		void		load_data(std::vector<uint8_t> a_data);
		uint8_t		get_byte(int an_index);
		void		set_byte(int an_index, uint8_t a_value);
		uint16_t	get_opcode(int a_program_counter);
		size_t		get_size();
		
	private:
		std::array<uint8_t, 4096>	the_memory;
};