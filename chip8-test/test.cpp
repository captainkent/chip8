#include "pch.h"
#include "../chip8-lib/src/CPU.h"
#include "../chip8-lib/src/CMemory.h"
#include "../chip8-lib/src/CRegisters.h"
#include "../chip8-lib/src/CGraphics.h"

class opcode_parser : public testing::Test {
public:
	CMemory*		the_memory;
	CRegisters*		the_registers;
	CGraphics*		the_graphics;
	CCPU*			the_cpu;

	void SetUp() {
		the_memory		= new CMemory;
		the_registers	= new CRegisters;
		the_graphics	= new CGraphics;
		
		the_cpu = new CCPU(the_memory, the_registers, the_graphics);
	}

	void TearDown() {
		delete the_cpu;
		delete the_registers;
		delete the_graphics;
	}
};


/**
	00E0 - CLS

	Clear the display.
*/
TEST_F(opcode_parser, test_CLS)
{
	// Manually set some pixels.
	for (int i = 0; i < 5; i++)
	{
		the_graphics->flip_pixel(i);
		EXPECT_EQ(the_graphics->get_pixel_state(i), 1);
	}
	
	// Set and parse the opcode.
	uint16_t my_opcode = 0x00e0;

	the_cpu->parse_opcode(my_opcode);

	// Verify the graphics are cleared.
	for (int i = 0; i < the_graphics->size(); i++)
	{
		EXPECT_EQ(the_graphics->get_pixel_state(i), 0);
	}
}

/**
	00EE - RET

	Return from a subroutine.

	The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
*/
TEST_F(opcode_parser, test_RET)
{

}

/**
	1nnn - JP addr

	Jump to location nnn.

	The interpreter sets the program counter to nnn.
*/
TEST_F(opcode_parser, test_JP_addr)
{

}

/**
	2nnn - CALL addr

	Call subroutine at nnn.

	The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
*/
TEST_F(opcode_parser, test_CALL_addr)
{

}

/**
	3xkk - SE Vx, byte

	Skip next instruction if Vx = kk.

	The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
*/
TEST_F(opcode_parser, test_SE_Vx_byte)
{

}


/**
	4xkk - SNE Vx, byte

	Skip next instruction if Vx != kk.

	The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
*/
TEST_F(opcode_parser, test_SNE_Vx_byte)
{

}

/**
	5xy0 - SE Vx, Vy

	Skip next instruction if Vx = Vy.

	The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
*/
TEST_F(opcode_parser, test_SE_Vx_Vy)
{

}

/**
	6xkk - LD Vx, byte

	Set Vx = kk.

	The interpreter puts the value kk into register Vx.
*/
TEST_F(opcode_parser, test_LD_Vx_byte)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x6128;

	the_cpu->parse_opcode(my_opcode);

	// Verify the contents of the register.
	EXPECT_EQ(the_registers->get_register_value(1), 0x28);
}

/**
	7xkk - ADD Vx, byte
	Set Vx = Vx + kk.

	Adds the value kk to the value of register Vx, then stores the result in Vx.
*/
TEST_F(opcode_parser, test_ADD_Vx_byte)
{

}

/**
	8xy0 - LD Vx, Vy

	Set Vx = Vy.

	Stores the value of register Vy in register Vx.
*/
TEST_F(opcode_parser, test_LD_Vx_Vy)
{

}

/**
	8xy1 - OR Vx, Vy
	Set Vx = Vx OR Vy.

	Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
	A bitwise OR compares the corresponding bits from two values, and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.
*/
TEST_F(opcode_parser, test_OR_Vx_Vy)
{

}

/**
	8xy2 - AND Vx, Vy
	Set Vx = Vx AND Vy.

	Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
	A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0.
*/
TEST_F(opcode_parser, test_AND_Vx_Vy)
{

}

/**
	8xy3 - XOR Vx, Vy
	Set Vx = Vx XOR Vy.

	Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
	An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0.
*/
TEST_F(opcode_parser, test_XOR_Vx_Vy)
{

}

/**
	8xy4 - ADD Vx, Vy
	Set Vx = Vx + Vy, set VF = carry.

	The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
*/
TEST_F(opcode_parser, test_ADD_Vx_Vy)
{

}

/**
	8xy5 - SUB Vx, Vy
	Set Vx = Vx - Vy, set VF = NOT borrow.

	If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
*/
TEST_F(opcode_parser, test_SUB_Vx_Vy)
{
	uint16_t my_opcode = 0x8695;

	the_registers->set_register_value(6, 5);
	the_registers->set_register_value(9, 2);

	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), 3);
	EXPECT_EQ(the_registers->get_register_value(0xf), 1);

	//////////////////////////////////////////////////////

	my_opcode = 0x8755;

	the_registers->set_register_value(7, 3);
	the_registers->set_register_value(5, 9);

	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(7), 250);
	EXPECT_EQ(the_registers->get_register_value(0xf), 0);
}

/**
	8xy6 - SHR Vx {, Vy}
	Set Vx = Vx SHR 1.

	If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
*/
TEST_F(opcode_parser, test_SHR_Vx_Vy)
{

}

/**
	8xy7 - SUBN Vx, Vy
	Set Vx = Vy - Vx, set VF = NOT borrow.

	If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
*/
TEST_F(opcode_parser, test_SUBN_Vx_Vy)
{

}

/**
	8xyE - SHL Vx {, Vy}
	Set Vx = Vx SHL 1.

	If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
*/
TEST_F(opcode_parser, test_SHL_Vx_Vy)
{

}

/**
	9xy0 - SNE Vx, Vy
	Skip next instruction if Vx != Vy.

	The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
*/
TEST_F(opcode_parser, test_SNE_Vx_Vy)
{

}

/**
	Annn - LD I, addr
	Set I = nnn.

	The value of register I is set to nnn.
*/
TEST_F(opcode_parser, test_LD_I_addr)
{

}

/**
	Bnnn - JP V0, addr
	Jump to location nnn + V0.

	The program counter is set to nnn plus the value of V0.
*/
TEST_F(opcode_parser, test_JP_V0_addr)
{

}

/**
	Cxkk - RND Vx, byte
	Set Vx = random byte AND kk.

	The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
*/
TEST_F(opcode_parser, test_RND_Vx_byte)
{

}

/**
	Dxyn - DRW Vx, Vy, nibble
	Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

	The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
	Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
	If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen.
	See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
*/
TEST_F(opcode_parser, test_DRW_Vx_Vy_nibble)
{

}

/**
	Ex9E - SKP Vx
	Skip next instruction if key with the value of Vx is pressed.

	Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
*/
TEST_F(opcode_parser, test_SKP_Vx)
{

}

/**
	ExA1 - SKNP Vx
	Skip next instruction if key with the value of Vx is not pressed.

	Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
*/
TEST_F(opcode_parser, test_SKNP_Vx)
{

}

/**
	Fx07 - LD Vx, DT
	Set Vx = delay timer value.

	The value of DT is placed into Vx.
*/
TEST_F(opcode_parser, test_LD_Vx_DT)
{

}

/**
	Fx0A - LD Vx, K
	Wait for a key press, store the value of the key in Vx.

	All execution stops until a key is pressed, then the value of that key is stored in Vx.
*/
TEST_F(opcode_parser, test_LD_Vx_K)
{

}

/**
	Fx15 - LD DT, Vx
	Set delay timer = Vx.

	DT is set equal to the value of Vx.
*/
TEST_F(opcode_parser, test_LD_DT_Vx)
{

}

/**
	Fx18 - LD ST, Vx
	Set sound timer = Vx.

	ST is set equal to the value of Vx.
*/
TEST_F(opcode_parser, test_LD_ST_Vx)
{

}

/**
	Fx1E - ADD I, Vx
	Set I = I + Vx.

	The values of I and Vx are added, and the results are stored in I.
*/
TEST_F(opcode_parser, test_ADD_I_Vx)
{

}

/**
	Fx29 - LD F, Vx
	Set I = location of sprite for digit Vx.

	The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
*/
TEST_F(opcode_parser, test_LD_F_Vx)
{

}

/**
	Fx33 - LD B, Vx
	Store BCD representation of Vx in memory locations I, I+1, and I+2.

	The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
*/
TEST_F(opcode_parser, test_LD_B_Vx)
{

}

/**
	Fx55 - LD [I], Vx
	Store registers V0 through Vx in memory starting at location I.

	The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
*/
TEST_F(opcode_parser, test_LD_I_Vx)
{

}

/**
	Fx65 - LD Vx, [I]
	Read registers V0 through Vx from memory starting at location I.

	The interpreter reads values from memory starting at location I into registers V0 through Vx.
*/
TEST_F(opcode_parser, test_LD_VX_I)
{

}