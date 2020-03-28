#include "CGraphics.h"

CGraphics::CGraphics() :
	the_graphics({})
{
}

int
CGraphics::get_pixel_state(int a_pixel)
{
	return the_graphics[a_pixel];
}

void
CGraphics::flip_pixel(int a_pixel)
{
	the_graphics[a_pixel] ^= 1;
}

void
CGraphics::clear()
{
	the_graphics = {};
}

size_t
CGraphics::get_size()
{
	return the_graphics.size();
}