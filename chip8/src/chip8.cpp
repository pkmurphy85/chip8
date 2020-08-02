#include "chip8.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cstring>    /* memset */
#include <iomanip>      // setfill, setw

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

uint8_t fontset[] =
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



Chip8::Chip8(void)
{
    cout << "CPU created" << endl;
    registers.program_counter = ROM_START_ADDRESS;
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
    std::memset(registers.general_purpose, 0, sizeof(registers.general_purpose)/sizeof(registers.general_purpose[0]));

    //srand (time(NULL));

    std::memset(keyboard, 0, sizeof(keyboard)/sizeof(keyboard[0]));
    std::memset(video, 0, sizeof(video)/sizeof(video[0]));
    std::memset(memory, 0, sizeof(memory)/sizeof(memory[0]));
    std::memset(stack, 0, sizeof(stack)/sizeof(stack[0]));

    load_function_tables();
    load_fonts();
    load_rom();

    cout << "Initialized" << endl;
    //cout << "Program counter" << registers.program_counter << endl;
}

uint8_t Chip8::current_opcode_nibble3(void)
{
    //cout << "current_opcode_nibble3 " << hex << (uint16_t) (current_opcode) << endl;
    //cout << "current_opcode_nibble3 " << hex << (uint16_t) (current_opcode & nibble3) << endl;
    //cout << "current_opcode_nibble3 " << hex << (uint16_t) ((current_opcode & nibble3) >> 12) << endl;
    
    return (((current_opcode & nibble3) >> 12) & 0x0F);
}

uint8_t Chip8::current_opcode_nibble2(void)
{
    return (((current_opcode & nibble2) >> 8) & 0x0F);
}

uint8_t Chip8::current_opcode_nibble1(void)
{
    return (((current_opcode & nibble1) >> 4) & 0x0F);
}

uint8_t Chip8::current_opcode_nibble0(void)
{
    return ((current_opcode & nibble0) & 0x0F);
}

uint16_t Chip8::current_opcode_lowest_12_bits()
{
    return (current_opcode & 0x0FFF);   
}

uint8_t Chip8::current_opcode_byte0(void)
{
    return (current_opcode & byte0);
}

uint8_t Chip8::current_opcode_byte1(void)
{
    return (current_opcode & byte1);
}


void Chip8::load_function_tables(void)
{
    table[0] = &Chip8::Table0;
    table[1] = &Chip8::opcode_1nnn;
    table[2] = &Chip8::opcode_2nnn;
    table[3] = &Chip8::opcode_3xkk;
    table[4] = &Chip8::opcode_4xkk;
    table[5] = &Chip8::opcode_5xy0;
    table[6] = &Chip8::opcode_6xkk;
    table[7] = &Chip8::opcode_7xkk;
    table[8] = &Chip8::Table8;
    table[9] = &Chip8::opcode_9xy0;
    table[10] = &Chip8::opcode_Annn;
    table[11] = &Chip8::opcode_Bnnn;
    table[12] = &Chip8::opcode_Cxkk;
    table[13] = &Chip8::opcode_Dxyn;
    table[14] = &Chip8::TableE;
    table[15] = &Chip8::TableF;

    table0[0] = &Chip8::opcode_00E0;
    table0[0xE] = &Chip8::opcode_00EE;

    table8[0] = &Chip8::opcode_8xy0;
    table8[1] = &Chip8::opcode_8xy1;
    table8[2] = &Chip8::opcode_8xy2;
    table8[3] = &Chip8::opcode_8xy3;
    table8[4] = &Chip8::opcode_8xy4;
    table8[5] = &Chip8::opcode_8xy5;
    table8[6] = &Chip8::opcode_8xy6;
    table8[7] = &Chip8::opcode_8xy7;
    //table8[8] = &Chip8::opcode_8xyE;

    tableE[0] = &Chip8::opcode_ExA1;
    tableE[1] = &Chip8::opcode_Ex9E;

    
    tableF[0] = &Chip8::opcode_Fx07;
    tableF[1] = &Chip8::opcode_Fx0A;
    tableF[2] = &Chip8::opcode_Fx15;   
    tableF[3] = &Chip8::opcode_Fx18;
    tableF[4] = &Chip8::opcode_Fx1E;
    tableF[5] = &Chip8::opcode_Fx29;
    tableF[6] = &Chip8::opcode_Fx33;
    tableF[7] = &Chip8::opcode_Fx55;
    tableF[8] = &Chip8::opcode_Fx65;
    

}

void Chip8::execute(void)
{
    //cout << "execute: "<< hex << current_opcode << endl;
    //cout << "execute: "<< hex << (uint16_t)current_opcode_nibble3() << endl;
    ((*this).*(table[current_opcode_nibble3()]))();
}

void Chip8::Table0(void)
{
    cout << "Table 0" << endl;
    ((*this).*(table0[current_opcode_nibble0()]))();
    //(table0[current_opcode_nibble0()])();
}

void Chip8::Table8(void)
{
    cout << "Table 8" << endl;
    ((*this).*(table8[current_opcode_nibble0()]))();
}

void Chip8::TableE(void)
{
    cout << "Table E" << endl;
    ((*this).*(tableE[current_opcode_nibble0()]))();
}

void Chip8::TableF(void)
{
    cout << "Table F" << endl;
    ((*this).*(tableF[current_opcode_nibble0()]))();
}

void Chip8::load_fonts()
{
    cout << "load_fonts" << endl;

    std::memcpy(&memory[FONTS_START_ADDRESS], fontset, sizeof(fontset));
}

void Chip8::load_rom()
{
    cout << "load_rom" << endl;
    
    std::ifstream rom;
    
    rom.open("../../../roms/IBM_Logo.rom", std::ios::binary);

    uint16_t count  = 0;    
    while (!rom.eof()) 
	{
        memory[ROM_START_ADDRESS + count] = rom.get();
        //cout << hex << (uint16_t)memory[ROM_START_ADDRESS + count] << " ";
        count += 1;
	}

    rom.close();
}


void Chip8::fetch(void)
{                   
    // Memory is composed of 4096 bytes. ins
    // MSB of instruction first byte, LSB of instruction is second byte
    //current_opcode = (uint16_t)(memory[registers.program_counter] | 
    //                                  (memory[registers.program_counter + 1] << 8));

    current_opcode = (uint16_t)((memory[registers.program_counter] << 8)| 
                                      memory[registers.program_counter + 1]);
    registers.program_counter += 2;

    /*if(current_opcode != 0x1228 )
    {
        cout << "FETCH" << endl;
        cout << "current opcode: " << hex << std::setw(4) << (uint16_t) current_opcode << endl;
        cout <<  "program counter: " << hex << std::setw(4) << (uint16_t)registers.program_counter << endl;
        cout <<  "stack pointer: " << hex << std::setw(4) << (uint16_t)registers.stack_pointer << endl;
        cout <<  "index register:  " << hex << std::setw(4) << (uint16_t)registers.index << endl;
        for(uint16_t i = 0; i<16; i++)
        {
            cout <<  "general_purpose[" << i << "]: " << hex << std::setw(2) << (uint16_t)registers.general_purpose[i] << endl;;
        }
    }
*/
    
}

void Chip8::opcode_unhandled(void)
{
    cout << "Unhandled instruction: " << hex << current_opcode << endl;
}

void Chip8::opcode_00E0(void)
{
    // Clear the display

    cout << "opcode_00E0" << endl;
    memset(video, 0, sizeof(video)/sizeof(video[0]));
}

void Chip8::opcode_00EE(void)
{
    // Return from subroutine

    cout << "opcode_00EE" << endl;

    registers.stack_pointer -= 1;
    registers.program_counter = stack[registers.stack_pointer];
}

void Chip8::opcode_1nnn(void)
{
    // Jump to location nnn
    //cout << "opcode_1nnn" << endl;

    registers.program_counter = current_opcode_lowest_12_bits();
}

void Chip8::opcode_2nnn(void)
{
    // Call subroutine at nnn
    
    cout << "opcode_2nnn" << endl;

    stack[registers.stack_pointer] = registers.program_counter;
    registers.stack_pointer += 1;
    registers.program_counter = current_opcode_lowest_12_bits();
}

void Chip8::opcode_3xkk(void)
{
    // Skip next instruction if Vx = kk

    cout << "opcode_3xkk" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t byte = current_opcode_byte0();

    if(registers.general_purpose[Vx] == byte)
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_4xkk(void)
{
    // Skip next instruction if Vx != kk

    cout << "opcode_4xkk" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t byte = current_opcode_byte0();

    if(registers.general_purpose[Vx] != byte)
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_5xy0(void)
{
    // Skip next instruction if Vx = Vy

    cout << "opcode_5xy0" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    if(registers.general_purpose[Vx] != registers.general_purpose[Vy])
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_6xkk(void)
{
    // Set Vx = kk

    cout << "opcode_6xkk" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t byte = current_opcode_byte0();

    registers.general_purpose[Vx] = byte;
}

void Chip8::opcode_7xkk(void)
{
    // Set Vx = Vx + kk

    cout << "opcode_7xkk" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t byte = current_opcode_byte0();

    registers.general_purpose[Vx] += byte;
}

void Chip8::opcode_8xy0(void)
{
    // Set Vx = Vy

    cout << "opcode_8xy0" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    registers.general_purpose[Vx] = registers.general_purpose[Vy];
}

void Chip8::opcode_8xy1(void)
{
    // Set Vx = Vx OR Vy

    cout << "opcode_8xy1" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    registers.general_purpose[Vx] |= registers.general_purpose[Vy];
}

void Chip8::opcode_8xy2(void)
{
    // Set Vx = Vx AND Vy

    cout << "opcode_8xy2" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    registers.general_purpose[Vx] &= registers.general_purpose[Vy];
}

void Chip8::opcode_8xy3(void)
{
    // Set Vx = Vx XOR Vy

    cout << "opcode_8xy3" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    registers.general_purpose[Vx] ^= registers.general_purpose[Vy];
}

void Chip8::opcode_8xy4(void)
{
    // Set Vx = Vx + Vy, set VF = carry if overflow
    
    cout << "opcode_8xy4" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    uint16_t temp = registers.general_purpose[Vx];
    registers.general_purpose[Vx]  += registers.general_purpose[Vy];

    // if there is an overflow, previous value will be greater than new value
    registers.general_purpose[0xF] = temp > registers.general_purpose[Vx] ? 1 : 0;
}

void Chip8::opcode_8xy5(void)
{
    // Set Vx = Vx - Vy, set VF = NOT borrow

    cout << "opcode_8xy5" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    registers.general_purpose[0xF] = registers.general_purpose[Vx] > registers.general_purpose[Vy] ? 1 : 0;

    registers.general_purpose[Vx] -= registers.general_purpose[Vy];
}

void Chip8::opcode_8xy6(void)
{
    // Set Vx = Vx SHR 1

    cout << "opcode_8xy6" << endl;

    uint8_t Vx = current_opcode_nibble2();

    registers.general_purpose[0xF] = (registers.general_purpose[Vx] & 0x1) == 0x1 ? 1 : 0;

    registers.general_purpose[Vx] /= 2;
}

void Chip8::opcode_8xy7(void)
{
    // Set Vx = Vy - Vx, set VF = NOT borrow

    cout << "opcode_8xy7" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    registers.general_purpose[0xF] = registers.general_purpose[Vx] > registers.general_purpose[Vy] ? 1 : 0;

    registers.general_purpose[Vx] = registers.general_purpose[Vy] - registers.general_purpose[Vx];
}

void Chip8::opcode_8xyE(void)
{
    // Set Vx = Vx SHL 1 

    cout << "opcode_8xyE" << endl;

    uint8_t Vx = current_opcode_nibble2();

    registers.general_purpose[0xF] = (registers.general_purpose[Vx] & 0x80) == 0x80 ? 1 : 0;
    
    registers.general_purpose[Vx] *= 2;
}

void Chip8::opcode_9xy0(void)
{
    // Skip next instruction if Vx != Vy

    cout << "opcode_9xy0" << endl;

    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();

    if(registers.general_purpose[Vx] != registers.general_purpose[Vy] )
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_Annn(void)
{
    // Set I = nnn

    cout << "opcode_Annn" << endl;

    registers.index = current_opcode_lowest_12_bits();
}

void Chip8::opcode_Bnnn(void)
{
    // Jump to location nnn + V0

    cout << "opcode_Bnnn" << endl;

    registers.program_counter = current_opcode_lowest_12_bits() + registers.general_purpose[0];
}

void Chip8::opcode_Cxkk(void)
{
    // Set Vx = random byte AND kk

    cout << "opcode_Cxkk" << endl;
    
    uint8_t Vx = current_opcode_nibble2();

    registers.general_purpose[Vx] = (rand() % 256) & current_opcode_byte0();
}



void Chip8::opcode_Dxyn(void)
{
    cout << "opcode_Dxyn" << endl;
    
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    uint8_t Vx = current_opcode_nibble2();
    uint8_t Vy = current_opcode_nibble1();
    
    uint8_t x_loc = registers.general_purpose[Vx] % VIDEO_WIDTH;
    uint8_t y_loc = registers.general_purpose[Vy] % VIDEO_HEIGHT;
    registers.general_purpose[0xF] = 0;
    uint8_t num_rows = current_opcode_nibble0();

    for(int8_t row = 0; row<num_rows; row++)
    {
        uint8_t sprite_data = memory[registers.index + row]; 

        for(int8_t col = 0; col < 8; col++)
        {
            uint8_t sprite_pixel = sprite_data & (0x80u >> col);
            uint8_t* screen_pixel = &video[(x_loc + col) + VIDEO_WIDTH*(y_loc + row)];

            if(sprite_pixel)
            {
                if(*screen_pixel == 0xFF)
                {
                    registers.general_purpose[0xF] = 1;
                }
                *screen_pixel ^= 0xFF;
            }
        }
    }
}

void Chip8::opcode_Ex9E(void)
{
    // Skip next instruction if key with the value of Vx is pressed

    cout << "opcode_Ex9E" << endl;

    uint8_t Vx = current_opcode_nibble2();

    if(keyboard[Vx])
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_ExA1(void)
{
    // Skip next instruction if key with the value of Vx is not pressed

    cout << "opcode_ExA1" << endl;

    uint8_t Vx = current_opcode_nibble2();

    if(!keyboard[Vx])
    {
        registers.program_counter += 2;
    }
}

void Chip8::opcode_Fx07(void)
{
    // Set Vx = delay timer value

    cout << "opcode_Fx07" << endl;

     uint8_t Vx = current_opcode_nibble2();

     registers.general_purpose[Vx] = registers.delay_timer;
}

void Chip8::opcode_Fx0A(void)
{
    // Wait for a key press, store the value of the key in Vx

    cout << "opcode_Fx0A" << endl;

    uint8_t Vx = current_opcode_nibble2();

    bool key_pressed = false;
    for(int i = 0; i < sizeof(keyboard); i++)
    {
        if(keyboard[i] == 1)
        {
            key_pressed = true;
            registers.general_purpose[Vx] = i;
        }
    }

    if(!key_pressed)
    {
        registers.program_counter -= 2; // will cause this function to run again until key pressed
    }
}

void Chip8::opcode_Fx15(void)
{
    // Set delay timer = Vx

    cout << "opcode_Fx15" << endl;

    uint8_t Vx = current_opcode_nibble2();

    registers.delay_timer = registers.general_purpose[Vx];
}

void Chip8::opcode_Fx18(void)
{
    // Set sound timer = Vx

    cout << "opcode_Fx18" << endl;

    uint8_t Vx = current_opcode_nibble2();

    registers.sound_timer = registers.general_purpose[Vx];
}

void Chip8::opcode_Fx1E(void)
{
    // Set I = I + Vx

    cout << "opcode_Fx1E" << endl;

    uint8_t Vx = current_opcode_nibble2();

    registers.index += registers.general_purpose[Vx];
}

void Chip8::opcode_Fx29(void)
{
    cout << "opcode_Fx29" << endl;
    uint8_t Vx = current_opcode_nibble2();
    uint8_t digit = registers.general_purpose[Vx] & 0x0F; // just use bottom nibble
    registers.index = FONTS_START_ADDRESS + (digit * 5);
}

void Chip8::opcode_Fx33(void)
{
    cout << "opcode_Fx33" << endl;
    uint8_t Vx = current_opcode_nibble2();
    uint8_t digit = registers.general_purpose[Vx];
    uint8_t bcd_hundreds = digit/100;
    uint8_t bcd_tens = (digit - 100 * bcd_hundreds)/10;
    uint8_t bcd_ones = ((digit - 100 * bcd_hundreds) - 10 * bcd_tens);

    memory[registers.index] = bcd_hundreds;
    memory[registers.index+1] = bcd_tens;
    memory[registers.index+2] = bcd_ones;
}

void Chip8::opcode_Fx55(void)
{
    // Store registers V0 through Vx in memory starting at location I

    cout << "opcode_Fx55" << endl;

    uint8_t offset = current_opcode_nibble2();

    std::memcpy(&memory[registers.index], registers.general_purpose, offset);
}

void Chip8::opcode_Fx65(void)
{
    // Read registers V0 through Vx from memory starting at location I

    cout << "opcode_Fx65" << endl;

    uint8_t offset = current_opcode_nibble2();

    std::memcpy(registers.general_purpose, &memory[registers.index], offset);
}

uint8_t* Chip8::get_video(void)
{
    return video;
}








