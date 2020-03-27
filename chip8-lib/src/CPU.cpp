#pragma once
#include "CPU.h"
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <assert.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/bind.hpp>

CCPU::CCPU(CMemory* a_memory, CRegisters* a_register) : 
	the_memory(a_memory),
	the_V_registers(a_register),
	the_I_register(0x0),
	the_opcode(0x0),
	the_pc(0x0),
	the_gfx({}),
	the_delay_timer(0x0),
	the_sound_timer(0x0),
	the_stack({}),
	the_sp(0x0),
	the_key({}),
	the_drawflag(false),
	the_start_flag(false),
	the_timer(the_context, boost::asio::chrono::milliseconds(2))
{
}


CCPU::~CCPU()
{
}

void
CCPU::initialize()
{
	// Set program counter to 0x200, the rest to 0x0.
	// Everything before is system info, rom, etc.
	the_pc			= 0x200;
	the_opcode		= 0x0;
	the_I_register	= 0x0;
	the_sp			= 0x0;
	
	// Clear registers.
	the_gfx			= {};
	the_stack		= {};
	//the_V_reg		= {};
	//the_memory		= {};

	// For random number generation
	srand(time(NULL));

	// The fontset
	unsigned char chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	// Load the fontset into memory at 0x50.
	for (int i = 0; i < sizeof(chip8_fontset); i++)
	{
		the_memory->set_byte(i + 0x50, chip8_fontset[i]);
	}
}

bool
CCPU::load_game(std::string sname)
{
	bool my_return = false;
	
	// Load the file.
	std::ifstream my_file(sname, std::ios::binary | std::ios::ate);

	if (!my_file)
	{
		std::cerr << "Unable to load file..\n";
	}
	else
	{
		// Get file size.
		std::streamsize fsize = my_file.tellg();

		// Set back to beginning of stream.
		my_file.seekg(0, std::ios::beg);

		// Create buffer, load into container.
		std::vector<uint8_t> my_buffer(fsize);
		my_file.read(reinterpret_cast<char*>(&my_buffer[0]), fsize);

		// Copy buffer into memory.
		the_memory->load_data(my_buffer);

		my_return = true;
	}
		
	return my_return;
}

void
CCPU::cpu_cycle(boost::system::error_code const& e, boost::asio::steady_timer* t)
{	
	// Fetch the opcode.
	//the_opcode = the_memory[the_pc] << 8 | the_memory[the_pc + 1];
	the_opcode = the_memory->get_opcode(the_pc);

	// Parse the opcode.
	parse_opcode(the_opcode);

	// Update the timers.
	if (the_delay_timer > 0)
		--the_delay_timer;

	if (the_sound_timer > 0)
	{
		if (the_sound_timer == 1)
		{
			printf("BEEP\n");
			// beep noise here.
		}
		--the_sound_timer;
	}

	// If we're done with our timer, return to stop endlessly continuing. Make sure to do this after we've performed our last cycle!
	if (!the_start_flag)
		return;

	// For the timer to continue and not drift away.
	t->expires_at(t->expiry() + boost::asio::chrono::milliseconds(2));
	t->async_wait(boost::bind(&CCPU::cpu_cycle, this, boost::asio::placeholders::error, t));
	
}

void CCPU::parse_opcode(uint16_t an_opcode)
{
	// Split up for easy access later.
	uint8_t code[2];
	code[0] = (an_opcode & 0xff00) >> 8;
	code[1] = an_opcode & 0x00ff;

	printf("%04x %02x %02x ", the_pc, code[0], code[1]);

	switch (an_opcode & 0xf000)
	{
		case 0x0000:
		{
			switch (an_opcode & 0x00ff)
			{
			case 0xe0:
				{
					the_gfx = {};
					the_drawflag = true;
					the_pc += 2;

					printf("%-10s", "CLS\n");
					break;
				}
				case 0xee:
				{
					the_sp--;
					the_pc = the_stack[the_sp];
					the_pc += 2;

					printf("%-10s\n", "RET");
					break;
				}
				default:
				{
					printf("unknown opcode: 0x%04x\n", an_opcode);
				}
			}
			break;
		}
		case 0x1000:
		{
			the_pc = an_opcode & 0x0FFF;

			printf("%-10s #$%03x\n", "JP", the_pc);
			break;
		}
		case 0x2000:
		{
			uint16_t addr = ((code[0] & 0x0f) << 8) + code[1];

			the_stack[the_sp] = the_pc;
			the_sp++;
			the_pc = addr;

			printf("%-10s #$%03x\n", "CALL", addr);
			break;
		}
		case 0x3000:
		{
			uint8_t reg = code[0] & 0x0f;

			//if (the_V_reg[reg] == code[1])
			if(the_V_registers->get_register_value(reg) == code[1])
			{
				the_pc += 2;
			}

			the_pc += 2;

			printf("%-10s V%01X,#$%02x\n", "SE", reg, code[1]);
			break;
		}
		case 0x4000:
		{
			uint8_t reg = code[0] & 0x0f;

			//if (the_V_reg[reg] != code[1])
			if(the_V_registers->get_register_value(reg) != code[1])
			{
				the_pc += 2;
			}

			the_pc += 2;

			printf("%-10s V%01X,#$%02x\n", "SNE", reg, code[1]);
			break;
		}
		case 0x5000:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = code[1] & 0x0f;

			if (the_V_registers->get_register_value(regx) == the_V_registers->get_register_value(regy))
			{
				the_pc += 2;
			}

			the_pc += 2;

			printf("%-10s V%01X,V%01X\n", "SNE", regx, regy);
			break;
		}
		case 0x6000:
		{
			uint8_t reg = code[0] & 0x0f;
			//the_V_reg[reg] = code[1];
			the_V_registers->set_register_value(reg, code[1]);

			the_pc += 2;
			printf("%-10s V%01X,#$%02x\n", "MVI", reg, code[1]);

			break;
		}
		case 0x7000:
		{
			uint8_t reg = code[0] & 0x0f;
			uint8_t my_value = the_V_registers->get_register_value(reg) + code[1];

			the_V_registers->set_register_value(reg, my_value);
			//the_V_reg[reg] += code[1];

			the_pc += 2;
			printf("%-10s V%01X,#$%02x\n", "ADD", reg, code[1]);

			break;
		}
		case 0x8000:
		{
			switch (an_opcode & 0x000f)
			{
				case 0x00:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;
					uint8_t my_value = the_V_registers->get_register_value(regy);

					//the_V_reg[regx] = the_V_reg[regy];
					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "LD", regx, regy);
					break;
				}
				case 0x01:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;
					uint8_t my_value = the_V_registers->get_register_value(regx) | the_V_registers->get_register_value(regy);


					//the_V_reg[regx] = the_V_reg[regx] | the_V_reg[regy];
					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "OR", regx, regy);
					break;
				}
				case 0x02:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;
					uint8_t my_value = the_V_registers->get_register_value(regx) & the_V_registers->get_register_value(regy);

					//the_V_reg[regx] = the_V_reg[regx] & the_V_reg[regy];
					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "AND", regx, regy);
					break;
				}
				case 0x03:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;
					uint8_t my_value = the_V_registers->get_register_value(regx) ^ the_V_registers->get_register_value(regy);

					//the_V_reg[regx] = the_V_reg[regx] ^ the_V_reg[regy];
					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "XOR", regx, regy);
					break;
				}
				case 0x04:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;
					uint8_t my_value = the_V_registers->get_register_value(regx) + the_V_registers->get_register_value(regy);

					if (the_V_registers->get_register_value(regx) + the_V_registers->get_register_value(regy) <= 0xff)
					{
						the_V_registers->set_register_value(regx, my_value);
					}
					else
					{
						the_V_registers->set_register_value(regx, my_value);
						the_V_registers->set_register_value(0xf, 1);
					}

					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "ADD", regx, regy);
					break;
				}
				case 0x05:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;

					if (the_V_registers->get_register_value(regx) > the_V_registers->get_register_value(regy))
					{
						the_V_registers->set_register_value(0xf, 1);
					}
					else
					{
						the_V_registers->set_register_value(0xf, 0);
					}

					uint8_t my_value = the_V_registers->get_register_value(regx) - the_V_registers->get_register_value(regy);

					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "SUB", regx, regy);
					break;
				}
				case 0x06:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;

					the_V_registers->set_register_value(0xf, regx & 0x1);
					
					uint8_t my_value = the_V_registers->get_register_value(regx) / 2;

					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "SHR", regx, regy);
					break;
				}
				case 0x07:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;

					if (the_V_registers->get_register_value(regy) > the_V_registers->get_register_value(regx))
					{
						the_V_registers->set_register_value(0xf, 1);
					}
					else
					{
						the_V_registers->set_register_value(0xf, 0);
					}

					uint8_t my_value = the_V_registers->get_register_value(regy) - the_V_registers->get_register_value(regx);

					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "SUBN", regx, regy);
					break;
				}
				case 0x0E:
				{
					uint8_t regx = code[0] & 0x0f;
					uint8_t regy = (code[1] & 0xf0) >> 4;

					if ((the_V_registers->get_register_value(regx) & 0xf0) == 0x1)
					{
						the_V_registers->set_register_value(0xf, 1);
					}
					else
					{
						the_V_registers->set_register_value(0xf, 0);
					}
					
					uint8_t my_value = the_V_registers->get_register_value(regx) * 2;

					the_V_registers->set_register_value(regx, my_value);
					the_pc += 2;

					printf("%-10s V%01X,V%01X\n", "SHL", regx, regy);
					break;
				}
				default:
				{
					printf("unknown opcode: 0x%04x\n", an_opcode);
				}
				break;
			}

			break;
		}
		case 0x9000:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;

			if (the_V_registers->get_register_value(regx) != the_V_registers->get_register_value(regy))
			{
				the_pc += 2;
			}

			the_pc += 2;

			printf("%-10s V%01X,V%01X\n", "SNE", regx, regy);
			break;
		}
		case 0xA000:
		{
			//the_I_register.set_byte(an_opcode & 0x0FFF);
			the_I_register = an_opcode & 0x0FFF;
			the_pc += 2;

			//printf("%-10s #$%03x\n", "LD I", the_I_register.get_byte());
			printf("%-10s #$%03x\n", "LD I", the_I_register);
			break;
		}
		case 0xB000:
		{
			the_pc = (an_opcode & 0x0FFF) + the_V_registers->get_register_value(0x0);

			printf("%-10s #$%03x\n", "JP", the_pc);
			break;
		}
		case 0xC000:
		{
			uint8_t reg = code[0] & 0x0f;
			uint8_t num = rand() % 255;

			the_V_registers->set_register_value(reg, num & code[1]);
			the_pc += 2;

			printf("%-10s V%01X,#$%02x\n", "RND", reg, code[1]);
			break;
		}
		case 0xD000:
		{
			// DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
			// All sprites are 8bit wide, but n high.

			// Sprites are ALWAYS 8 pixels wide, and between 1 and 15 pixels high, where N is height.
			// height = rows = n
			// width = columns = 8

			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;

			// Get the co-ordinates and height.
			uint8_t x = the_V_registers->get_register_value(regx);
			uint8_t y = the_V_registers->get_register_value(regy);
			uint8_t height = an_opcode & 0x000F;
			uint8_t pixel;

			// Reset F register.
			the_V_registers->set_register_value(0xf, 0);

			// First check each row.
			for (int row = 0; row < height; row++)
			{
				// Retrieve the pixel state from memory.
				//pixel = the_memory.get_byte(the_I_register.get_byte() + row);
				pixel = the_memory->get_byte(the_I_register + row);

				// Then check each column,
				for (int col = 0; col < 8; col++)
				{
					// Check if the pixel is already set.
					if ((pixel & (0x80 >> col)) != 0)
					{
						int my_pixel = (x + row + ((y + col) * 64));

						the_pixels.push_back(my_pixel);

						if (the_gfx[(x + row + ((y + col) * 64))] == 1)
						{
							the_V_registers->set_register_value(0xf, 1);
						}
						the_gfx[(x + row + ((y + col) * 64))] ^= 1;
					}
				}
			}

			the_drawflag = true;
			the_pc += 2;

			printf("%-10s V%01X,V%01X,#$%01X\n", "DRW", regx, regy, height);
			break;
		}
		case 0xE000:
		{
			switch (an_opcode & 0x00FF)
			{
				case 0x9E:
				{
					uint8_t reg = code[0] & 0x0f;
					if (the_key[the_V_registers->get_register_value(reg)] == 1)
					{
						the_pc += 2;
					}

					the_pc += 2;

					printf("%-10s V%01X\n", "SKP", reg);
					break;
				}
				case 0xA1:
				{
					uint8_t reg = code[0] & 0x0f;
					if (the_key[the_V_registers->get_register_value(reg)] != 1)
					{
						the_pc += 2;
					}

					the_pc += 2;

					printf("%-10s V%01X\n", "SKNP", reg);
					break;
				}
				default:
				{
					printf("unknown opcode: 0x%04x\n", an_opcode);
				}
			}
			break;
		}
		case 0xF000:
		{
			switch (an_opcode & 0x00FF)
			{
				case 0x07:
				{
					uint8_t reg = code[0] & 0x0f;
					the_V_registers->set_register_value(reg, the_delay_timer);

					the_pc += 2;

					printf("%-10s V%01X,#DT\n", "LD Vx DT", reg);
					break;
				}
				case 0x0A:
				{
					uint8_t reg = code[0] & 0x0f;

					// Check status of all keys stored in key.
					for (auto i : the_key)
					{
						// If key state is active.
						if (the_key[i])
						{
							// Store key value in Vreg.
							the_V_registers->set_register_value(reg, i);
							the_pc += 2;
						}
					}

					printf("%-10s V%01X,#K\n", "LD Vx K", reg);
					break;
				}
				case 0x15:
				{
					uint8_t reg = code[0] & 0x0f;
					the_delay_timer = the_V_registers->get_register_value(reg);

					the_pc += 2;

					printf("%-10s V%01X\n", "LD DT Vx", reg);
					break;
				}
				case 0x18:
				{
					uint8_t reg = code[0] & 0x0f;
					the_sound_timer = the_V_registers->get_register_value(reg);

					the_pc += 2;

					printf("%-10s V%01X\n", "LD ST Vx", reg);
					break;
				}
				case 0x1E:
				{
					uint8_t reg = code[0] & 0x0f;
					//uint8_t my_value = the_I_register.get_byte() + the_V_registers.get_register_value(reg);
					uint8_t my_value = the_I_register + the_V_registers->get_register_value(reg);

					//the_I_register.set_byte(my_value);
					the_I_register = my_value;
					the_pc += 2;

					printf("%-10s V%01X\n", "ADD F Vx", reg);
					break;
				}
				case 0x29:
				{
					uint8_t reg = code[0] & 0x0f;
					uint8_t my_value = the_V_registers->get_register_value(reg) * 0x05;

					//the_I_register.set_byte(my_value);
					the_I_register = my_value;
					the_pc += 2;

					printf("%-10s V%01X\n", "LD F Vx", reg);
					break;
				}
				case 0x33:
				{
					uint8_t reg = code[0] & 0x0f;
					uint8_t bcd = the_V_registers->get_register_value(reg);

					//the_memory.set_byte(the_I_register.get_byte(),		bcd / 100);
					//the_memory.set_byte(the_I_register.get_byte(),		(bcd / 10) % 10);
					//the_memory.set_byte(the_I_register.get_byte() + 2,	bcd % 10);

					the_memory->set_byte(the_I_register,		bcd / 100);
					the_memory->set_byte(the_I_register + 1,	(bcd / 10) % 10);
					the_memory->set_byte(the_I_register + 2,	bcd % 10);

					the_pc += 2;

					printf("%-10s V%01X\n", "LD B Vx", reg);
					break;
				}
				case 0x55:
				{
					uint8_t reg = code[0] & 0x0f;
					for (int i = 0; i <= reg; i++)
					{
						//the_memory.set_byte(the_I_register.get_byte() + i, the_V_registers.get_register_value(i));
						the_memory->set_byte(the_I_register + i, the_V_registers->get_register_value(i));
					}

					the_pc += 2;

					printf("%-10s V%01X\n", "LD [I] Vx", reg);
					break;
				}
				case 0x65:
				{
					uint8_t reg = code[0] & 0x0f;
					for (int i = 0; i <= reg; i++)
					{
						//uint8_t my_value = the_memory.get_byte(the_I_register.get_byte() + 1);
						uint8_t my_value = the_memory->get_byte(the_I_register + 1);
						
						the_V_registers->set_register_value(i, my_value);
					}

					the_pc += 2;

					printf("%-10s V%01X\n", "LD Vx [I]", reg);
					break;
				}
				default:
				{
					printf("unknown opcode: 0x%04x\n", an_opcode);
				}
			}

			break;
		}
		default:
		{
			printf("unknown opcode: 0x%04x\n", an_opcode);
			break;
		}
	}
}

void CCPU::start()
{
	/*the_start_time = boost::posix_time::microsec_clock::local_time();*/
	
	the_start_flag = true;

	the_timer.async_wait(boost::bind(&CCPU::cpu_cycle, this, boost::asio::placeholders::error, &the_timer));
	the_context.run();
}

void CCPU::stop()
{
	the_start_flag = false;
}