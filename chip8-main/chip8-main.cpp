// chip8-main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\chip8-lib\src\CMemory.h"
#include "..\chip8-lib\src\CRegisters.h"

int main()
{
    CMemory my_memory;

    std::vector<uint8_t> my_data;

    my_data.push_back(0x05);
    my_data.push_back(0x06);

    my_memory.load_data(my_data);

    uint16_t my_opcode = my_memory.get_opcode(0x200);

    CRegisters my_registers;

    my_registers.set_register_value(6, 100);

    std::cout << my_registers.get_register_value(6);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
