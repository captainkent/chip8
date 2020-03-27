#pragma once
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <stdint.h>
#include <string>
#include <array>
#include <vector>
#include <atomic>
#include "CMemory.h"
#include "CRegisters.h"


class CCPU
{
public:
	CCPU(CMemory* a_memory, CRegisters* a_register/*, CGraphics a_graphics, CKeyboard a_keyboard*/);
	~CCPU();

	void		initialize();
	bool		load_game(std::string sname);
	void		cpu_cycle(boost::system::error_code const& e, boost::asio::steady_timer* t);
	void		parse_opcode(uint16_t an_opcode);
	void		start();
	void		stop();

private:
	CMemory*					the_memory;
	CRegisters*					the_V_registers;

	
	uint16_t					the_opcode;
	uint16_t					the_pc;
	uint16_t					the_sp;
	uint8_t						the_I_register;
	uint8_t						the_sound_timer;
	uint8_t						the_delay_timer;
	bool						the_drawflag;
	
	std::array<uint8_t, 2048>	the_gfx;
	std::array<uint16_t, 16>	the_stack;
	std::array<uint8_t, 16>		the_key;
	

	// for timing...
	std::atomic<bool>			the_start_flag;
	boost::asio::io_context		the_context;
	boost::asio::steady_timer	the_timer;

	//// debug stuff
	//int							the_count;
	//boost::posix_time::ptime	the_start_time;
	//boost::posix_time::ptime	the_end_time;
	std::vector<int>			the_pixels;

};

