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

void Chip8::opcode_unhandled(void)
{
    cout << "Unhandled instruction: " << hex << current_opcode << endl;
}

void Chip8::opcode_0nnn(void)
{
    cout << "Need to implement opcode_0nnn. Ignored by most modern interrpreters" << endl;
}

void Chip8::opcode_00E0(void)
{
    // Clear the display
    cout << "Size of video: " << sizeof(video) << endl;
    memset(video, 0, sizeof(video));
}

void Chip8::opcode_00EE(void)
{
    // Return from subroutine
    registers.stack_pointer -= 1;
    registers.program_counter = stack[registers.stack_pointer];
}

void Chip8::opcode_1nnn(void)
{
    // Jump to location nnn
    registers.program_counter = get_lowest_12_bits(current_opcode);
}

void Chip8::opcode_2nnn(void)
{
    // Call subroutine at nnn
    
    stack[registers.stack_pointer] = registers.program_counter;
    registers.stack_pointer += 1;
    registers.program_counter = get_lowest_12_bits(current_opcode);
}

void Chip8::opcode_3xkk(void)
{
    // Skip next instruction if Vx = kk

    if(registers.general_purpose[get_nibble2(current_opcode)] == (current_opcode & byte0))
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_4xkk(void)
{
    // Skip next instruction if Vx != kk

    if(registers.general_purpose[get_nibble2(current_opcode)] != (current_opcode & byte0))
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_5xy0(void)
{
    // Skip next instruction if Vx = Vy

    if(registers.general_purpose[get_nibble2(current_opcode)] != registers.general_purpose[get_nibble1(current_opcode)])
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_6xkk(void)
{
    // Set Vx = kk

    registers.general_purpose[get_nibble2(current_opcode)] = (current_opcode & byte0);
}

void Chip8::opcode_7xkk(void)
{
    // Set Vx = Vx + kk

    registers.general_purpose[get_nibble2(current_opcode)] += (current_opcode & byte0);
}

void Chip8::opcode_8xy0(void)
{
    // Set Vx = Vy

    registers.general_purpose[get_nibble2(current_opcode)] = registers.general_purpose[get_nibble1(current_opcode)];
}

void Chip8::opcode_8xy1(void)
{
    // Set Vx = Vx OR Vy

    registers.general_purpose[get_nibble2(current_opcode)] |= registers.general_purpose[get_nibble1(current_opcode)];
}

void Chip8::opcode_8xy2(void)
{
    // Set Vx = Vx AND Vy

    registers.general_purpose[get_nibble2(current_opcode)] &= registers.general_purpose[get_nibble1(current_opcode)];
}

void Chip8::opcode_8xy3(void)
{
    // Set Vx = Vx XOR Vy

    registers.general_purpose[get_nibble2(current_opcode)] ^= registers.general_purpose[get_nibble1(current_opcode)];
}

void Chip8::opcode_8xy4(void)
{
    // Set Vx = Vx + Vy, set VF = carry if overflow
    uint8_t *reg_x = &registers.general_purpose[get_nibble2(current_opcode)];
    uint8_t *reg_y = &registers.general_purpose[get_nibble1(current_opcode)];

    uint16_t temp = *reg_x;
    *reg_x += *reg_y;

    // if there is an overflow, previous value will be greater than new value
    registers.general_purpose[15] = temp > *reg_x ? 1 : 0;
}

void Chip8::opcode_8xy5(void)
{
    // Set Vx = Vx - Vy, set VF = NOT borrow

    uint8_t *reg_x = &registers.general_purpose[get_nibble2(current_opcode)];
    uint8_t *reg_y = &registers.general_purpose[get_nibble1(current_opcode)];

    registers.general_purpose[15] = *reg_x > *reg_y ? 1 : 0;

    *reg_x -= *reg_y;
}

void Chip8::opcode_8xy6(void)
{
    // Set Vx = Vx SHR 1

    uint8_t *reg_x = &registers.general_purpose[get_nibble2(current_opcode)];

    registers.general_purpose[15] = (*reg_x & 0x1 == 0x1) ? 1 : 0;

    *reg_x /= 2;
}

void Chip8::opcode_8xy7(void)
{
    // Set Vx = Vy - Vx, set VF = NOT borrow

    uint8_t *reg_x = &registers.general_purpose[get_nibble2(current_opcode)];
    uint8_t *reg_y = &registers.general_purpose[get_nibble1(current_opcode)];

    registers.general_purpose[15] = *reg_y > *reg_x ? 1 : 0;

    *reg_x = *reg_y - *reg_x;
}

void Chip8::opcode_8xyE(void)
{
    // Set Vx = Vx SHL 1 

    uint8_t *reg_x = &registers.general_purpose[get_nibble2(current_opcode)];

    registers.general_purpose[15] = (*reg_x & 0x80 == 0x80) ? 1 : 0;
    
    *reg_x *= 2;
}

void Chip8::opcode_9xy0(void)
{
    // Skip next instruction if Vx != Vy

    if(registers.general_purpose[get_nibble2(current_opcode)] != registers.general_purpose[get_nibble1(current_opcode)] )
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_Annn(void)
{
    // Set I = nnn

    registers.index = get_lowest_12_bits(current_opcode);
}

void Chip8::opcode_Bnnn(void)
{
    // Jump to location nnn + V0

    registers.program_counter = get_lowest_12_bits(current_opcode) + registers.general_purpose[0];
}

void Chip8::opcode_Cxkk(void)
{
    // Set Vx = random byte AND kk

    registers.general_purpose[get_nibble2(current_opcode)] = (rand() % 256) & (current_opcode & byte0);
}

void Chip8::opcode_Dxyn(void)
{
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision

    cout << "Need to implement opcode_Dxyn" << endl;
}

void Chip8::opcode_Ex9E(void)
{
    // Skip next instruction if key with the value of Vx is pressed

    uint8_t *reg_x = &registers.general_purpose[get_nibble2(current_opcode)];

    if(keyboard[*reg_x])
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_ExA1(void)
{
    // Skip next instruction if key with the value of Vx is not pressed

    uint8_t *reg_x = &registers.general_purpose[get_nibble2(current_opcode)];

    if(!keyboard[*reg_x])
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_Fx07(void)
{
    cout << "Need to implement opcode_Fx07" << endl;
}

void Chip8::opcode_Fx0A(void)
{
    cout << "Need to implement opcode_Fx0A" << endl;
}

void Chip8::opcode_Fx15(void)
{
    cout << "Need to implement opcode_Fx15" << endl;
}

void Chip8::opcode_Fx18(void)
{
    cout << "Need to implement opcode_Fx18" << endl;
}

void Chip8::opcode_Fx1E(void)
{
    cout << "Need to implement opcode_Fx1E" << endl;
}

void Chip8::opcode_Fx29(void)
{
    cout << "Need to implement opcode_Fx29" << endl;
}

void Chip8::opcode_Fx33(void)
{
    cout << "Need to implement opcode_Fx33" << endl;
}

void Chip8::opcode_Fx55(void)
{
    cout << "Need to implement opcode_Fx55" << endl;
}

void Chip8::opcode_Fx65(void)
{
    cout << "Need to implement opcode_Fx65" << endl;
}









