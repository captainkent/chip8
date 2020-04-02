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

bool
CGraphics::init()
{
	bool my_return_flag;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		my_return_flag = false;
	}
	else
	{
		// Create the window at the desired size.
		//SDL_CreateWindowAndRenderer(WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE, SDL_WINDOW_SHOWN, &the_window, &the_renderer);
		the_window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
		the_renderer = SDL_CreateRenderer(the_window, -1, SDL_RENDERER_SOFTWARE);

		if (the_window == nullptr || the_renderer == nullptr)
		{
			my_return_flag = false;
		}
		else
		{
			// Create the texture at the size of the pixel buffer.
			the_texture = SDL_CreateTexture(the_renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, 64, 32);

			// Set the colour to black, copy texture to render and render it.
			/*SDL_SetTextureColorMod(the_texture, 0xff, 0xff, 0xff);
			SDL_RenderCopy(the_renderer, the_texture, NULL, NULL);
			SDL_RenderPresent(the_renderer);*/

			my_return_flag = true;
		}
	}

	return my_return_flag;
}

void
CGraphics::draw()
{
	// Before we draw, we need to convert our array to an array suitable for drawing with RGB values. This means convert each uint_8 which 
	// has a 1 to an 0xff = black.

	std::array<uint8_t, 2048> my_graphics = the_graphics;

	for (int i = 0; i < my_graphics.size(); i++)
	{
		if (my_graphics[i] == (uint8_t)(1))
			my_graphics[i] = 0xff;
		else
			my_graphics[i] = 0x0;
	}

	// Update texture.
	SDL_UpdateTexture(the_texture, NULL, &my_graphics, 32 * sizeof(uint8_t));

	// Rect set-up for auto scaling.
	SDL_Rect my_rect;
	my_rect.w = 640;
	my_rect.h = 320;
	my_rect.x = 0;
	my_rect.y = 0;

	// Copy texture to renderer. In this case, the texture will be automatically scaled to the render size.
	//SDL_RenderCopyEx(the_renderer, the_texture, NULL, &my_rect, 180, &my_point, SDL_FLIP_HORIZONTAL);
	SDL_RenderCopy(the_renderer, the_texture, NULL, &my_rect);
	SDL_RenderPresent(the_renderer);
}