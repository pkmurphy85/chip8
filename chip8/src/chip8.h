#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>

#define ROM_START_ADDRESS 0x200
#define FONTS_START_ADDRESS 0x50

typedef struct {
    uint8_t general_purpose[16];        /**< 16 8-bit General Purpose registers */
    uint16_t index;                     /**< 16-bit Index register */
    uint16_t program_counter;           /**< 16-bit Program Counter */
    uint8_t stack_pointer;              /**< 8-bit Stack Pointer - up to 16 levels */
    uint8_t delay_timer;                /**< 8-bit Delay Timer register */
    uint8_t sound_timer;                /**< 8-bit Sound Timer register */
    
} register_set;

uint8_t fonts[] =
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

class Chip8 
{
    private:
        register_set registers;           /**< Registers */
        uint8_t memory[4096];             /**< 4K memory */
        uint16_t current_instruction;     /**< The current intruction */
        uint16_t stack[16];
        uint8_t keypad[16]{};
	    uint32_t video[64 * 32]{};

        uint16_t get_lowest_12_bits(uint16_t opcode);
        uint8_t get_nibble3(uint16_t opcode);
        uint8_t get_nibble2(uint16_t opcode);
        uint8_t get_nibble1(uint16_t opcode);
        uint8_t get_nibble0(uint16_t opcode);

        void opcode_unhandled(uint16_t opcode);
        void opcode_0nnn(uint16_t address);
        void opcode_00E0(uint16_t address);
        void opcode_00EE(uint16_t address);
        void opcode_1nnn(uint16_t address);
        void opcode_2nnn(uint16_t address);
        void opcode_3xkk(uint16_t opcode);
        void opcode_4xkk(uint16_t opcode);
        void opcode_5xy0(uint16_t opcode);
        void opcode_6xkk(uint16_t opcode);
        void opcode_7xkk(uint16_t opcode);
        
        void opcode_8xy0(uint16_t opcode);
        void opcode_8xy1(uint16_t opcode);
        void opcode_8xy2(uint16_t opcode);
        void opcode_8xy3(uint16_t opcode);
        void opcode_8xy4(uint16_t opcode);
        void opcode_8xy5(uint16_t opcode);
        void opcode_8xy6(uint16_t opcode);
        void opcode_8xy7(uint16_t opcode);
        void opcode_8xyE(uint16_t opcode);

        void opcode_9xy0(uint16_t opcode);
        void opcode_Annn(uint16_t opcode);
        void opcode_Bnnn(uint16_t opcode);
        void opcode_Cxkk(uint16_t opcode);
        void opcode_Dxyn(uint16_t opcode);
        void opcode_Ex9E(uint16_t opcode);
        void opcode_ExA1(uint16_t opcode);

        void opcode_Fx07(uint16_t opcode);
        void opcode_Fx0A(uint16_t opcode);
        void opcode_Fx15(uint16_t opcode);
        void opcode_Fx18(uint16_t opcode);
        void opcode_Fx1E(uint16_t opcode);
        void opcode_Fx29(uint16_t opcode);
        void opcode_Fx33(uint16_t opcode);
        void opcode_Fx55(uint16_t opcode);
        void opcode_Fx65(uint16_t opcode);

        
    public: 
        Chip8();
        ~Chip8();
        void initialize(void);
        uint16_t get_program_counter(void);
        void fetch(void);
};

#endif // CHIP8_H