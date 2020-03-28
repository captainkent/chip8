#include "pch.h"
#include "../chip8-lib/src/CPU.h"
#include "../chip8-lib/src/CMemory.h"
#include "../chip8-lib/src/CRegisters.h"
#include "../chip8-lib/src/CGraphics.h"
#include "../chip8-lib/src/CKeyboard.h"

class opcode_parser : public testing::Test {
public:
	CMemory*		the_memory;
	CRegisters*		the_registers;
	CGraphics*		the_graphics;
	CKeyboard*		the_keyboard;
	CCPU*			the_cpu;

	void SetUp() {
		the_memory		= new CMemory;
		the_registers	= new CRegisters;
		the_graphics	= new CGraphics;
		the_keyboard	= new CKeyboard;

		the_cpu = new CCPU(the_memory, the_registers, the_graphics, the_keyboard);
	}

	void TearDown() {
		delete the_memory;
		delete the_registers;
		delete the_graphics;
		delete the_keyboard;
		delete the_cpu;
	}
};

/**
	Test to see if the memory is filled as intended.
*/
TEST_F(opcode_parser, test_memory)
{
	// Check if memory is empty.
	for (int i = 0; i < the_memory->get_size(); i++)
		EXPECT_EQ(the_memory->get_byte(i), 0);
	
	// Store some data in the memory.
	std::vector<uint8_t> my_data_buffer;

	for(int i = 0; i < 10; i++)
		my_data_buffer.push_back(0x10 + i);

	the_memory->load_data(my_data_buffer);

	// Verify the content of the memory.
	for (int i = 0; i < 10; i++)
		EXPECT_EQ(the_memory->get_byte(i + 0x200), 0x10 + i);
}


/**
	Test to see if the registers are filled as intended.
*/
TEST_F(opcode_parser, test_registers)
{
	// Set up a register.
	the_registers->set_register_value(2, 0x20);

	EXPECT_EQ(the_registers->get_register_value(2), 0x20);
}

/**
	Test to see if the graphics register works as intended.
*/
TEST_F(opcode_parser, test_graphics)
{
	// Make sure it's empty.
	EXPECT_EQ(the_graphics->get_pixel_state(6), 0);
	
	// Set up some stuff in the graphics register.
	the_graphics->flip_pixel(6);

	EXPECT_EQ(the_graphics->get_pixel_state(6), 1);

	// Flip it again.
	the_graphics->flip_pixel(6);

	EXPECT_EQ(the_graphics->get_pixel_state(6), 0);

}

/**
	Test to see if the keyboard register works as intended.
*/
TEST_F(opcode_parser, test_keyboard)
{

}

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
	for (int i = 0; i < the_graphics->get_size(); i++)
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
	EXPECT_EQ(0, 1);
}

/**
	1nnn - JP addr

	Jump to location nnn.

	The interpreter sets the program counter to nnn.
*/
TEST_F(opcode_parser, test_JP_addr)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x1123;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 0x123);
}

/**
	2nnn - CALL addr

	Call subroutine at nnn.

	The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
*/
TEST_F(opcode_parser, test_CALL_addr)
{
	EXPECT_EQ(0, 1);
}

/**
	3xkk - SE Vx, byte

	Skip next instruction if Vx = kk.

	The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
*/
TEST_F(opcode_parser, test_SE_Vx_byte)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x3466;

	// Set up the registers.
	the_registers->set_register_value(4, 0x66);

	// Parse the opcode, this time it should skip, means program counter goes up by 4.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 4);

	// Now set the registers so it won't skip.
	the_registers->set_register_value(4, 0x65);

	// Parse the opcode, this time it shouldn't skip, means program counter goes up by 2.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 6);
}


/**
	4xkk - SNE Vx, byte

	Skip next instruction if Vx != kk.

	The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
*/
TEST_F(opcode_parser, test_SNE_Vx_byte)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x4466;

	// Set up the registers.
	the_registers->set_register_value(4, 0x66);

	// Parse the opcode, this time it shouldn't skip, means program counter goes up by 2.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 2);

	// Now set the registers so it won't skip.
	the_registers->set_register_value(4, 0x65);

	// Parse the opcode, this time it shouldn skip, means program counter goes up by 4.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 6);
}

/**
	5xy0 - SE Vx, Vy

	Skip next instruction if Vx = Vy.

	The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
*/
TEST_F(opcode_parser, test_SE_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x5450;

	// Set up the registers.
	the_registers->set_register_value(4, 0x66);
	the_registers->set_register_value(5, 0x66);

	// Parse the opcode, this time it should skip, means program counter goes up by 4.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 4);

	// Now set the registers so it won't skip.
	the_registers->set_register_value(4, 0x65);

	// Parse the opcode, this time it shouldn skip, means program counter goes up by 2.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 6);
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
	// Set up the opcode.
	uint16_t my_opcode = 0x7244;

	// Set up something in the register to add.
	the_registers->set_register_value(2, 0x20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(2), 0x64);
}

/**
	8xy0 - LD Vx, Vy

	Set Vx = Vy.

	Stores the value of register Vy in register Vx.
*/
TEST_F(opcode_parser, test_LD_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x8340;

	// Set up the registers.
	the_registers->set_register_value(3, 0x20);
	the_registers->set_register_value(4, 0x50);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(3), 0x50);
}

/**
	8xy1 - OR Vx, Vy
	Set Vx = Vx OR Vy.

	Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
	A bitwise OR compares the corresponding bits from two values, and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.
*/
TEST_F(opcode_parser, test_OR_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x8631;

	// Set up the registers.
	the_registers->set_register_value(6, 0x20);
	the_registers->set_register_value(3, 0x50);

	// Comparison value.
	uint8_t my_result = 0x20 | 0x50;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), my_result);
}

/**
	8xy2 - AND Vx, Vy
	Set Vx = Vx AND Vy.

	Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
	A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0.
*/
TEST_F(opcode_parser, test_AND_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x8632;

	// Set up the registers.
	the_registers->set_register_value(6, 0x20);
	the_registers->set_register_value(3, 0x50);

	// Comparison value.
	uint8_t my_result = 0x20 & 0x50;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), my_result);
}

/**
	8xy3 - XOR Vx, Vy
	Set Vx = Vx XOR Vy.

	Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
	An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0.
*/
TEST_F(opcode_parser, test_XOR_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x8633;

	// Set up the registers.
	the_registers->set_register_value(6, 0x20);
	the_registers->set_register_value(3, 0x50);

	// Comparison value.
	uint8_t my_result = 0x20 ^ 0x50;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), my_result);
}

/**
	8xy4 - ADD Vx, Vy
	Set Vx = Vx + Vy, set VF = carry.

	The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
*/
TEST_F(opcode_parser, test_ADD_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x8634;

	// Set up the registers, this ADD should have no carry bit set.
	the_registers->set_register_value(6, 0x20);
	the_registers->set_register_value(3, 0x50);

	// Comparison value.
	uint8_t my_result = 0x20 + 0x50;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), my_result);
	EXPECT_EQ(the_registers->get_register_value(0xf), 0);
	
	// Now do some ADD where a carry bit will be set.
	the_registers->set_register_value(6, 0xff);
	the_registers->set_register_value(3, 0x20);

	// Comparison value.
	my_result = 0xff + 0x20;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), my_result);
	EXPECT_EQ(the_registers->get_register_value(0xf), 1);
}

/**
	8xy5 - SUB Vx, Vy
	Set Vx = Vx - Vy, set VF = NOT borrow.

	If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
*/
TEST_F(opcode_parser, test_SUB_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x8695;

	// Set up the registers, to cause a carry bit.
	the_registers->set_register_value(6, 5);
	the_registers->set_register_value(9, 2);

	// Comparison value.
	uint8_t my_result = 5 - 2;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), my_result);
	EXPECT_EQ(the_registers->get_register_value(0xf), 1);

	//////////////////////////////////////////////////////

	// Comparison value.
	my_result = 3 - 9;

	// Set up the registers, to this case no carry bit.
	the_registers->set_register_value(6, 3);
	the_registers->set_register_value(9, 9);

	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(6), my_result);
	EXPECT_EQ(the_registers->get_register_value(0xf), 0);
}

/**
	8xy6 - SHR Vx {, Vy}
	Set Vx = Vy SHR 1.

	Store the value of register VY shifted right one bit in register VX. Set register VF to the least significant bit of VY prior to the shift. VY is unchanged.
*/
TEST_F(opcode_parser, test_SHR_Vx_Vy)
{
	// Set up the opcode.
	uint8_t my_opcode = 0x8436;

	// Set up the registers.
	the_registers->set_register_value(4, 0x10);
	the_registers->set_register_value(3, 0x20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// First check the LSB of Vy.
	uint8_t my_lsb = 0x20 & 0b0001;

	EXPECT_EQ(the_registers->get_register_value(0xf), my_lsb);

	// Now check the shifted value.
	uint8_t my_shifted_value = 0x20 >> 1;

	EXPECT_EQ(the_registers->get_register_value(4), my_shifted_value);
}

/**
	8xy7 - SUBN Vx, Vy
	Set Vx = Vy - Vx, set VF = NOT borrow.

	If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
*/
TEST_F(opcode_parser, test_SUBN_Vx_Vy)
{
	// Set up the opcode.
	uint8_t my_opcode = 0x8437;

	// Set up the registers, with carry bit.
	the_registers->set_register_value(4, 0x10);
	the_registers->set_register_value(3, 0x20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// First check the value.
	uint8_t my_value = 0x20 - 0x10;

	EXPECT_EQ(the_registers->get_register_value(4), my_value);
	EXPECT_EQ(the_registers->get_register_value(0xf), 1);

	// Set up the registers, without carry bit.
	the_registers->set_register_value(4, 0x20);
	the_registers->set_register_value(3, 0x10);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// First check the value.
	my_value = 0x10 - 0x20;

	EXPECT_EQ(the_registers->get_register_value(4), my_value);
	EXPECT_EQ(the_registers->get_register_value(0xf), 0);
}

/**
	8xyE - SHL Vx {, Vy}
	Set Vx = Vy SHL 1.

	Store the value of register VY shifted left one bit in register VX. Set register VF to the most significant bit of VY prior to the shift. VY is unchanged.
*/
TEST_F(opcode_parser, test_SHL_Vx_Vy)
{
	// Set up the opcode.
	uint8_t my_opcode = 0x843e;

	// Set up the registers.
	the_registers->set_register_value(4, 0x10);
	the_registers->set_register_value(3, 0x20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// First check the MSB of Vy.
	uint8_t my_msb = 0x20 & 0b1000;

	EXPECT_EQ(the_registers->get_register_value(0xf), my_msb);

	// Now check the shifted value.
	uint8_t my_shifted_value = 0x20 << 1;

	EXPECT_EQ(the_registers->get_register_value(4), my_shifted_value);
}

/**
	9xy0 - SNE Vx, Vy
	Skip next instruction if Vx != Vy.

	The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
*/
TEST_F(opcode_parser, test_SNE_Vx_Vy)
{
	// Set up the opcode.
	uint16_t my_opcode = 0x9470;

	// Set up the registers to NOT skip.
	the_registers->set_register_value(4, 0x10);
	the_registers->set_register_value(7, 0x10);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Program counter should be 2.
	EXPECT_EQ(the_cpu->get_pc(), 2);

	// Now we do want to skip.
	the_registers->set_register_value(4, 0x10);
	the_registers->set_register_value(7, 0x20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Program counter should be 6, the previous program counter was 2, and this time we skip.
	EXPECT_EQ(the_cpu->get_pc(), 6);
}

/**
	Annn - LD I, addr
	Set I = nnn.

	The value of register I is set to nnn.
*/
TEST_F(opcode_parser, test_LD_I_addr)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xa123;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_I_reg(), 0x123);
}

/**
	Bnnn - JP V0, addr
	Jump to location nnn + V0.

	The program counter is set to nnn plus the value of V0.
*/
TEST_F(opcode_parser, test_JP_V0_addr)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xb123;

	// Set up the registers.
	the_registers->set_register_value(0, 0x5);

	// The address value.
	uint16_t my_value = 0x123 + 0x5;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), my_value);
}

/**
	Cxkk - RND Vx, byte
	Set Vx = random byte AND kk.

	The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
*/
TEST_F(opcode_parser, test_RND_Vx_byte)
{
	// Per definition of rand(), if we don't pass a seed to it using srand() before calling rand(), it will behave as if srand(1) is called.
	// Seeing as we only set the seed in initialize(), we can abuse this for testing! 
	// Therefore our implementation of rand() % 255 will always return 41.

	// Set up the opcode.
	uint16_t my_opcode = 0xc521;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// My value to check, 41 is what rand() % 255 returns.
	uint8_t my_value = 0x21 & 41;

	EXPECT_EQ(the_registers->get_register_value(5), my_value);
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
	EXPECT_EQ(0, 1);
}

/**
	Ex9E - SKP Vx
	Skip next instruction if key with the value of Vx is pressed.

	Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
*/
TEST_F(opcode_parser, test_SKP_Vx)
{
	// Set up the opcode.
	uint8_t my_opcode = 0xe59e;

	// Set up something in the keystate to active.
	the_keyboard->set_key_state(5, 1);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Key state is active, so the program counter will be skipped (+4).
	EXPECT_EQ(the_cpu->get_pc(), 4);

	// Now set keystate to inactive.
	the_keyboard->set_key_state(5, 0);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Key state is active, so the program counter will not be skipped, just incremented.
	EXPECT_EQ(the_cpu->get_pc(), 6);

}

/**
	ExA1 - SKNP Vx
	Skip next instruction if key with the value of Vx is not pressed.

	Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
*/
TEST_F(opcode_parser, test_SKNP_Vx)
{
	// Set up the opcode.
	uint8_t my_opcode = 0xe5a1;

	// Set up something in the keystate to active.
	the_keyboard->set_key_state(5, 1);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Key state is active, so the program counter will NOT be skipped.
	EXPECT_EQ(the_cpu->get_pc(), 2);

	// Now set keystate to active.
	the_keyboard->set_key_state(5, 1);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Key state is inactive, so the program counter will be skipped.
	EXPECT_EQ(the_cpu->get_pc(), 6);
}

/**
	Fx07 - LD Vx, DT
	Set Vx = delay timer value.

	The value of DT is placed into Vx.
*/
TEST_F(opcode_parser, test_LD_Vx_DT)
{
	// At startup, our delay timer is 0x0. Therefore, our register will be set to 0.
	uint16_t my_opcode = 0xf507;

	// To check this, load a value in it, determine that value.
	the_registers->set_register_value(5, 20);

	EXPECT_EQ(the_registers->get_register_value(5), 20);

	// Now parse the opcode (which then resets it to 0).
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_registers->get_register_value(5), 0);
}

/**
	Fx0A - LD Vx, K
	Wait for a key press, store the value of the key in Vx.

	All execution stops until a key is pressed, then the value of that key is stored in Vx.
*/
TEST_F(opcode_parser, test_LD_Vx_K)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xf50a;

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Verify the program counter doesn't increase because no key is active.
	EXPECT_EQ(the_cpu->get_pc(), 0);

	// Now set a keystate to active.
	the_keyboard->set_key_state(2, 1);

	// Re-parse the opcode, and verify the program counter does increase.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_pc(), 2);

	// Verify the Vx register gets the keystate value.
	EXPECT_EQ(the_registers->get_register_value(5), 1);
}

/**
	Fx15 - LD DT, Vx
	Set delay timer = Vx.

	DT is set equal to the value of Vx.
*/
TEST_F(opcode_parser, test_LD_DT_Vx)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xf615;

	// Set up the registers.
	the_registers->set_register_value(6, 20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_delay_timer(), 20);
}

/**
	Fx18 - LD ST, Vx
	Set sound timer = Vx.

	ST is set equal to the value of Vx.
*/
TEST_F(opcode_parser, test_LD_ST_Vx)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xf618;

	// Set up the registers.
	the_registers->set_register_value(6, 20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	EXPECT_EQ(the_cpu->get_sound_timer(), 20);
}

/**
	Fx1E - ADD I, Vx
	Set I = I + Vx.

	The values of I and Vx are added, and the results are stored in I.
*/
TEST_F(opcode_parser, test_ADD_I_Vx)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xf61e;

	// Set up the registers.
	the_registers->set_register_value(6, 20);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Seeing as the I register is 0, it'll will just add 20 to it.
	EXPECT_EQ(the_cpu->get_I_reg(), 20);
}

/**
	Fx29 - LD F, Vx
	Set I = location of sprite for digit Vx.

	The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
*/
TEST_F(opcode_parser, test_LD_F_Vx)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xf529;

	// Set up the registers to look for the hexadecimal sprite of 'A'.
	the_registers->set_register_value(0x5, 0xa);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Each sprite consists out of 5 8bit values, so the 'A' sprite will be located at 0xa * 5.
	uint8_t my_value = 0xa * 0x5;

	EXPECT_EQ(the_cpu->get_I_reg(), my_value);
}

/**
	Fx33 - LD B, Vx
	Store BCD representation of Vx in memory locations I, I+1, and I+2.

	The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
*/
TEST_F(opcode_parser, test_LD_B_Vx)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xf933;

	// Set up the registers.
	the_registers->set_register_value(9, 0xf4);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// 0xf4 = 244 in decimal, this means 2 100 digits, 4 10 digits and 4 1 digits.
	EXPECT_EQ(the_cpu->get_I_reg(), 2);
	EXPECT_EQ(the_cpu->get_I_reg() + 1, 4);
	EXPECT_EQ(the_cpu->get_I_reg() + 2, 4);
}

/**
	Fx55 - LD [I], Vx
	Store registers V0 through Vx in memory starting at location I.

	The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
*/
TEST_F(opcode_parser, test_LD_I_Vx)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xff55;

	// Set up the registers.
	for (int i = 0; i < 0xf; i++)
		the_registers->set_register_value(i, i + 1);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Now check to see if the memory is loaded correctly, starting at I.
	for (int i = 0; i < 0xf; i++)
	{
		uint8_t my_value = i + 1;
		uint8_t my_address = the_cpu->get_I_reg() + i;

		EXPECT_EQ(the_memory->get_byte(my_address), my_value);
	}
}

/**
	Fx65 - LD Vx, [I]
	Read registers V0 through Vx from memory starting at location I.

	The interpreter reads values from memory starting at location I into registers V0 through Vx.
*/
TEST_F(opcode_parser, test_LD_VX_I)
{
	// Set up the opcode.
	uint16_t my_opcode = 0xff65;

	// Set up the memory.
	uint8_t my_address = the_cpu->get_I_reg();

	for (int i = 0; i < 0xf; i++)
		the_memory->set_byte(my_address, i + 3);

	// Parse the opcode.
	the_cpu->parse_opcode(my_opcode);

	// Now check to see if the memory is loaded into the registers.
	for (int i = 0; i < 0xf; i++)
	{
		uint8_t my_value = i + 3;

		EXPECT_EQ(the_registers->get_register_value(i), my_value);
	}
}