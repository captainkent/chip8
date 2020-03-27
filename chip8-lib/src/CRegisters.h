#pragma once
#include <array>

class CRegisters
{
	public:
		CRegisters();
		~CRegisters() = default;
		
		void	set_register_value(int a_register, uint8_t a_value);
		uint8_t get_register_value(int a_register);

	private:
		std::array<uint8_t, 16>		the_registers;
};

