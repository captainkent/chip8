#pragma once
#include <stdint.h>
#include <array>

class CKeyboard
{
	public:
		CKeyboard();
		~CKeyboard() = default;

		void	set_key_state(int a_key, int a_state);
		uint8_t get_key_state(int a_key);
		size_t	get_size();

	private:
		std::array<uint8_t, 16> the_keyboard;
};

