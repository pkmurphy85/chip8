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
        uint16_t stack[16];
        uint8_t keyboard[16]{};
	    uint32_t video[64 * 32]{};
        uint16_t current_opcode;          /**< The current intruction */

        uint16_t current_opcode_lowest_12_bits(void);
        uint8_t current_opcode_nibble3(void);
        uint8_t current_opcode_nibble2(void);
        uint8_t current_opcode_nibble1(void);
        uint8_t current_opcode_nibble0(void);
        uint8_t current_opcode_byte0(void);
        uint8_t current_opcode_byte1(void);


        void opcode_unhandled(void);
        void opcode_0nnn(void);
        void opcode_00E0(void);
        void opcode_00EE(void);
        void opcode_1nnn(void);
        void opcode_2nnn(void);
        void opcode_3xkk(void);
        void opcode_4xkk(void);
        void opcode_5xy0(void);
        void opcode_6xkk(void);
        void opcode_7xkk(void);
        
        void opcode_8xy0(void);
        void opcode_8xy1(void);
        void opcode_8xy2(void);
        void opcode_8xy3(void);
        void opcode_8xy4(void);
        void opcode_8xy5(void);
        void opcode_8xy6(void);
        void opcode_8xy7(void);
        void opcode_8xyE(void);

        void opcode_9xy0(void);
        void opcode_Annn(void);
        void opcode_Bnnn(void);
        void opcode_Cxkk(void);
        void opcode_Dxyn(void);
        void opcode_Ex9E(void);
        void opcode_ExA1(void);

        void opcode_Fx07(void);
        void opcode_Fx0A(void);
        void opcode_Fx15(void);
        void opcode_Fx18(void);
        void opcode_Fx1E(void);
        void opcode_Fx29(void);
        void opcode_Fx33(void);
        void opcode_Fx55(void);
        void opcode_Fx65(void);

        
    public: 
        Chip8();
        ~Chip8();
        void initialize(void);
        uint16_t get_program_counter(void);
        void fetch(void);
};

#endif // CHIP8_H
