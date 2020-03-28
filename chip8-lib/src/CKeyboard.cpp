#include "CKeyboard.h"

CKeyboard::CKeyboard()
{
	the_keyboard = {};
}

void
CKeyboard::set_key_state(int a_key, int a_state)
{
	the_keyboard[a_key] = a_state;
}

uint8_t
CKeyboard::get_key_state(int a_key)
{
	return the_keyboard[a_key];
}

size_t
CKeyboard::get_size()
{
	return the_keyboard.size();
}