#pragma once
#include <array>
#include <stdint.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

class CGraphics
{
	public:
		CGraphics();
		~CGraphics() = default;

		int		get_pixel_state(int a_pixel);
		void	flip_pixel(int a_pixel);
		void	clear();
		size_t	get_size();

		bool	init();
		void	draw();

	private:
		std::array<uint8_t, 2048>	the_graphics;

		// sdl stuff
		SDL_Window* the_window;
		SDL_Renderer* the_renderer;
		SDL_Surface* the_surface;
		SDL_Texture* the_texture;

};

