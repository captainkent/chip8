#pragma once
#include <stdint.h>
#include <stack>

class CStack
{
	public:
		CStack();
		~CStack() = default;

		void		push(uint16_t a_data);
		uint16_t	top();
		void		pop();

	private:
		std::stack<uint16_t> the_stack;
};

