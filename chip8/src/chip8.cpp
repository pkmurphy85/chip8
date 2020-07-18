#include "chip8.h"
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cstring>    /* memset */

using std::cout;
using std::endl;
using std::hex;

typedef enum
{
    nibble0 = (0x000F),
    nibble1 = (0x00F0),
    nibble2 = (0x0F00),
    nibble3 = (0xF000),

    byte0 = (0x00FF),
    byte1 = (0xFF00),
} bytemask ;

Chip8::Chip8(void)
{
    cout << "CPU created" << endl;
    registers.program_counter = ROM_START_ADDRESS;
    //std::copy(fonts, fonts+sizeof(fonts), memory[FONTS_START_ADDRESS]);
}

Chip8::~Chip8(void)
{
    cout << "CPU deleted" << endl;
}

void Chip8::initialize(void)
{
    registers.program_counter = ROM_START_ADDRESS; // most programs start here
    registers.stack_pointer = 0;
    registers.index = 0;
    registers.delay_timer = 0;
    registers.sound_timer = 0;
    memset(registers.general_purpose, 0, sizeof(registers.general_purpose));

    srand (time(NULL));
    // clear display
    // clear stack
    // clear memory
    // load fontset
    //memset(memory, 0, sizeof(memory));
    cout << "Initialized" << endl;
}

uint8_t Chip8::get_nibble3(uint16_t opcode)
{
    return (((opcode & nibble3) >> 24) & 0x0F);
}

uint8_t Chip8::get_nibble2(uint16_t opcode)
{
    return (((opcode & nibble2) >> 16) & 0x0F);
}

uint8_t Chip8::get_nibble1(uint16_t opcode)
{
    return (((opcode & nibble1) >> 8) & 0x0F);
}

uint8_t Chip8::get_nibble0(uint16_t opcode)
{
    return ((opcode & nibble0) & 0x0F);
}



uint16_t Chip8::get_program_counter(void)
{
    return registers.program_counter;
}

void Chip8::fetch(void)
{                   
    // Memory is composed of 4096 bytes. ins
    // MSB of instruction first byte, LSB of instruction is second byte
    current_instruction = (uint16_t)((memory[registers.program_counter] << 8) | memory[registers.program_counter + 1]);
    registers.program_counter += 2;
}

uint16_t Chip8::get_lowest_12_bits(uint16_t opcode)
{
    return (opcode & 0x0FFF);   
}

void Chip8::opcode_unhandled(uint16_t address)
{
    cout << "Unhandled instruction: " << hex << address << endl;
}

void Chip8::opcode_0nnn(uint16_t opcode)
{
    cout << "Need to implement opcode_0nnn" << endl;
}

void Chip8::opcode_00E0(uint16_t opcode)
{
    cout << "Need to implement opcode_00E0" << endl;
}

void Chip8::opcode_00EE(uint16_t opcode)
{
    cout << "Need to implement opcode_00EE" << endl;
}

void Chip8::opcode_1nnn(uint16_t opcode)
{
    registers.program_counter = get_lowest_12_bits(opcode);
}

void Chip8::opcode_2nnn(uint16_t opcode)
{
    registers.stack_pointer += 1;
    stack[registers.stack_pointer] = registers.program_counter;
    registers.program_counter = get_lowest_12_bits(opcode);
}

void Chip8::opcode_3xkk(uint16_t opcode)
{
    if(registers.general_purpose[get_nibble2(opcode)] == (opcode & byte0))
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_4xkk(uint16_t opcode)
{
    if(registers.general_purpose[get_nibble2(opcode)] != (opcode & byte0))
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_5xy0(uint16_t opcode)
{
    if(registers.general_purpose[get_nibble2(opcode)] != (opcode & nibble1))
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_6xkk(uint16_t opcode)
{
    registers.general_purpose[get_nibble2(opcode)] = (opcode & byte0);
}

void Chip8::opcode_7xkk(uint16_t opcode)
{
    registers.general_purpose[get_nibble2(opcode)] += (opcode & byte0);
}

void Chip8::opcode_8xy0(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy0" << endl;
}

void Chip8::opcode_8xy1(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy1" << endl;
}

void Chip8::opcode_8xy2(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy2" << endl;
}

void Chip8::opcode_8xy3(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy3" << endl;    
}

void Chip8::opcode_8xy4(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy4" << endl;
}

void Chip8::opcode_8xy5(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy5" << endl;
}

void Chip8::opcode_8xy6(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy6" << endl;
}

void Chip8::opcode_8xy7(uint16_t opcode)
{
    cout << "Need to implement opcode_8xy7" <<endl;
}

void Chip8::opcode_8xyE(uint16_t opcode)
{
    cout << "Need to implement opcode_8xyE" << endl;
}

void Chip8::opcode_9xy0(uint16_t opcode)
{
    if(registers.general_purpose[get_nibble2(opcode)] != registers.general_purpose[get_nibble1(opcode)] )
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_Annn(uint16_t opcode)
{
    registers.index = get_lowest_12_bits(opcode);
}

void Chip8::opcode_Bnnn(uint16_t opcode)
{
    registers.program_counter = get_lowest_12_bits(opcode) + registers.general_purpose[0];
}

void Chip8::opcode_Cxkk(uint16_t opcode)
{
    registers.general_purpose[get_nibble2(opcode)] = (rand() % 256) & (opcode & byte0);
}

void Chip8::opcode_Dxyn(uint16_t opcode)
{
    cout << "Need to implement opcode_Dxyn" << endl;
}

void Chip8::opcode_Ex9E(uint16_t opcode)
{
    cout << "Need to implement opcode_Ex9E" << endl;
}

void Chip8::opcode_ExA1(uint16_t opcode)
{
    cout << "Need to implement opcode_ExA1" << endl;
}

void Chip8::opcode_Fx07(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx07" << endl;
}

void Chip8::opcode_Fx0A(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx0A" << endl;
}

void Chip8::opcode_Fx15(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx15" << endl;
}

void Chip8::opcode_Fx18(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx18" << endl;
}

void Chip8::opcode_Fx1E(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx1E" << endl;
}

void Chip8::opcode_Fx29(uint16_t opcode)
{
    cout << "Need to implement LD_F_Vx_Fx29" << endl;
}

void Chip8::opcode_Fx33(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx33" << endl;
}

void Chip8::opcode_Fx55(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx55" << endl;
}

void Chip8::opcode_Fx65(uint16_t opcode)
{
    cout << "Need to implement opcode_Fx65" << endl;
}









