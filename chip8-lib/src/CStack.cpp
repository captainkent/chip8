#include "CStack.h"

CStack::CStack()
{
	the_stack = {};
}

void
CStack::push(uint16_t a_data)
{
	the_stack.push(a_data);
}

uint16_t
CStack::top()
{
	return the_stack.top();
}

void
CStack::pop()
{
	the_stack.pop();
}