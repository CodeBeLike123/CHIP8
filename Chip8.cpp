#include "Chip8.h"
#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include  <array>
#include  <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip> 
#include <string>
#include <sstream>
#include <vector>
using namespace std;
Chip8::Chip8() {
	Initialize();
}

Chip8::~Chip8() {

}
void  Chip8::EmulateCycle(){
    //fetch 
    uint16_t opcode = (memory[pc] << 8) | (memory[pc + 1]);
    //decode + execute
    ExecuteOpcode(opcode);
}

void Chip8::ExecuteOpcode(uint16_t opcode) {
    switch (opcode & 0xF000) {
    case 0x0000:
    {
        switch (opcode & 0x00FF)
        {
        case 0x00E0: //00E0
        {
            memset(screen_pixels, 0, sizeof(screen_pixels));
            pc += 2;
            drawFlag = true;
            break;
        }
        case 0x00EE:  //00EE
        {
            --sp;
            pc = stack[sp];
            pc += 2;
            break;
        }

        }
        break;
    }
    case  0x1000:  //1NNN
    {
        pc = opcode & 0x0FFF;
        break;
    }
    case 0x2000: // 2NNN
    {
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
        break;
    }
    case 0x6000: // 6XNN
    {
        int register_index = (opcode & 0x0F00) >> 8;
        int address = opcode & 0x00FF;
        V[register_index] = address;
        break;
    }
    case 0x7000: //7XNN
    {
        int register_index = (opcode & 0x0F00) >> 8;
        int value = opcode & 0x00FF;
        V[register_index] += value;
        break;
    }
    case 0xA000: //ANNN
    {
        int register_index_value = (opcode & 0x0FFF);
        I = register_index_value;
        break;
    }
    case 0xD000: //DXYN
    {
        int x_register = (opcode & 0x0F00) >> 8;  //VX
        int y_register = (opcode & 0x00F0) >> 4;  //VY
        int height = opcode & 0x000F;
        int  width = 8; //default chip8 sprite width =  8 pixel 
        V[0x0F] = 0;

        int x = V[x_register] % 64;
        int y = V[y_register] % 32;

        for (int i = 0; i < height; i++) {
            int  pixel = memory[I + i];
            for (int j = 0; j < width; j++) {
                if ((pixel & (0x80 >> j)) != 0) {
                    int index = ((x + j) + ((y + i) * 64)) % 2048;
                    if (screen_pixels[index] == 1)
                    {
                        V[0x0F] = 1;
                    }

                    screen_pixels[index] ^= 1;
                }
            }
        }
        pc += 2;
        drawFlag = true;

    }
    }
}

void Chip8::Initialize() {
	I = 0;
	pc = 0x200;
	opcode = 0;
	sp = 0;

	delay_timer = 0;
	sound_timer = 0;

	
	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset (screen_pixels, 0, sizeof(screen_pixels));
	memset(key, 0, sizeof(key));
	memset(stack, 0, sizeof(stack));

	for (int i = 0; i < 80; i++) {
		memory[0x050 + i] = chip8_fontset[i];
	}

}

bool Chip8::LoadRom(const char* filename) {

    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open())
        return false;


    std::streamsize size = file.tellg();
    if (size > (4096 - 512)) {
        return false;
    }


    file.seekg(0, std::ios::beg);


    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        return false;
    }


    for (int i = 0; i < size; ++i) {
        memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
    }

    return true;

}
    