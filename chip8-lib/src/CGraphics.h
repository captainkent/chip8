#pragma once
#include <array>
#include <stdint.h>

class CGraphics
{
	public:
		CGraphics();
		~CGraphics() = default;

		int		get_pixel_state(int a_pixel);
		void	flip_pixel(int a_pixel);
		void	clear();
		size_t	get_size();

	private:
		std::array<uint8_t, 2048>	the_graphics;

};

