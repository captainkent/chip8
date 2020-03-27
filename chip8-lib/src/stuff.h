#pragma once
#include <stdint.h>
#include <iostream>

template<class T, size_t n>
class CRegister
{
public:
	CRegister() : the_register({}) {}
	~CRegister() = default;

	//void	set_byte(T a_byte)	{ the_register = a_byte	};
	//T		get_byte()			{ return the_register	};

	void	set_register_value(int a_register, T a_value) { the_register[a_register] = a_value; };
	T		get_register_value(int a_register) { return the_register[a_register]; };

	void	set_byte(T a_byte) { if (n == 1) { the_register[0] = a_byte; } };
	T		get_byte() { if (n == 1) { return the_register[0]; } };

private:
	std::array<T, n>  the_register;
};

namespace stuff {

	void DissassembleChip8OpCode(uint8_t* codebuffer, int pc);
}