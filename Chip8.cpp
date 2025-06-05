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
    if (sound_timer > 0) {
        sound_timer--;
    }
    if (delay_timer > 0 ) {
        delay_timer--;
    }
    
    uint16_t opcode = (memory[pc] << 8) | (memory[pc + 1]);
    
    pc += 2;

    ExecuteOpcode(opcode);
    cout << "current opcode:" << opcode << endl;
}

void Chip8::ExecuteOpcode(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    switch (opcode & 0xF000) {
    case 0x0000:
    {
        switch (opcode & 0x00FF)
        {
        case 0x00E0: //00E0
        {
            memset(screen_pixels, 0, sizeof(screen_pixels));
            
            drawFlag = true;
            break;
        }
        case 0x00EE:  //00EE
        {
            --sp;
            pc = stack[sp];
            
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
    case 0x3000: //3XNN skip if VX==NN
    {
        unsigned short x = (opcode & 0x0F00) >> 8;
        unsigned short nn = opcode & 0x00FF;

        if (V[x] == nn) {
            pc += 2;
        }
            

        break;
    }
    case 0x4000: //4XNN skip if VX==NN
    {
        unsigned short x = (opcode & 0x0F00) >> 8;
        unsigned short nn = opcode & 0x00FF;

        if (V[x] != nn) {
            pc += 2;
        }
            

        break;
    }
    case 0x5000: //5XY0
    {
        int16_t x = (opcode & 0x0F00) >> 8;
        int16_t y = (opcode & 0x00F0) >> 4;

        if (V[x] == V[y]) {
            pc += 2;
        }
        break;

    }
    case 0x6000: //6XNN
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
    case 0x8000 : //8XYN
    {
        switch (opcode & 0x000F)
        {
        case 0x0: //8XY0
        {
            int16_t x = (opcode & 0x0F00) >> 8;
            int16_t y = (opcode & 0x00F0) >> 4;
            V[x] = V[y];
            
            break;
        }
        case 0x1: //8XY1
        {
            int8_t x = (opcode & 0x0F00) >> 8;
            int8_t y = (opcode & 0x00F0) >> 4;
            V[x] |= V[y];
            
            break;
        }
        case 0x2: //8xy2
        {
            int8_t x = (opcode & 0x0F00) >> 8;
            int8_t y = (opcode & 0x00F0) >> 4;
            V[x] &= V[y];
            
            break;
        }
        case 0x3: //8xy3
        {
            int8_t x = (opcode & 0x0F00) >> 8;
            int8_t y = (opcode & 0x00F0) >> 4;
            V[x] ^= V[y];
            
            break;
        }
        case 0x4: 
        {
            int16_t x = (opcode & 0x0F00) >> 8;
            int16_t y = (opcode & 0x00F0) >> 4;

            int sum = V[x] + V[y];
            V[0xF] = (sum > 255) ? 1 : 0;
            V[x] = sum & 0xFF;
            
            break;
        }
        case 0x5:
        {
            int16_t x = (opcode & 0x0F00) >> 8;
            int16_t y = (opcode & 0x00F0) >> 4;

            int diff = V[x] - V[y];
            V[0xF] = (diff < 0) ? 0 : 1;
            V[x] = diff & 0xFF;
            
            break;
        }
        case 0x6:
        {
            int16_t x = (opcode & 0x0F00) >> 8;
            V[0xF] = V[x] & 0x1; //store the  LS bit to V[f]
            V[x] >>= 1; //shift right 1 bit
            
            break;

        }
        case 0x7:
        {
            int16_t x = (opcode & 0x0F00) >> 8;
            int16_t y = (opcode & 0x00F0) >> 4;

            int diff = V[y] - V[x];
            V[0xF] = (diff < 0) ? 0 : 1;
            V[x] = diff & 0xFF;
            
            break;
        }
        case 0xE: 
        {   
            int16_t x = (opcode & 0x0F00) >> 8;
            V[0xF] = V[x] & 0x80; //store the  MS bit to V[f]
            V[x] <<= 1; //shift left 1 bit
            
            break;

        }
        break;

        }

    }
    case 0x9000://9XY0
    {
        int16_t x = (opcode & 0x0F00) >> 8;
        int16_t y = (opcode & 0x00F0) >> 4;

        if (V[x] != V[y]) {
            pc += 2;
        }
        
        break;
    }
    case 0xA000: //ANNN
    {
        int register_index_value = (opcode & 0x0FFF);
        I = register_index_value;
        
        break;
    }
    case 0xB000: //BNNN
    {
        int nnn = (opcode & 0x0FFF);
        pc = nnn + V[0];
        break;
    }
    case 0xC000: //CxNN
    {
        int x = (opcode & 0x0F00) >> 8;
        uint8_t nn = opcode & 0x00ff;

        uint8_t random_byte = rand() % 256;

        V[x] = random_byte & nn;
        
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
        
        drawFlag = true;
        break;

    }
    case 0xF000:
    {
        uint8_t x;
        switch (opcode & 0x00FF) {
        case 0x07: 
        {
            x = (opcode & 0x0F00) >> 8;
            V[x] = delay_timer;
            
            break;
        }
        case 0x0A:
        {
            x = (opcode & 0x0F00) >> 8;
            bool key_pressed = false; 

            for (int i = 0; i < 16; i++) {
                if (key[i] != 0) {
                    key_pressed = true;
                    V[x] = i;
                    break;
                }
            }

            if (key_pressed) {
                
            }

            break;
        }

        case 0x15:
        {
            x = (opcode & 0x0F00) >> 8;
            delay_timer = V[x];
            
            break;
        }
        case  0x18:
        {
            x = (opcode & 0x0F00) >> 8;
            sound_timer = V[x];
            
            break;
        }
        case 0x1E:
        {
            x = (opcode & 0x0F00) >> 8;
            I += V[x];
            
            break;
        }
        case  0x29:
        {
            x = (opcode & 0x0F00) >> 8;
            I = 0x050 + (V[x] * 5);
            
            break;

        }
        case 0x33:
        {
            x = (opcode & 0x0F00) >> 8;

            memory[I] = V[x] / 100;
            memory[I + 1] = (V[x] / 10) % 10;
            memory[I + 2] = V[x] % 10;

            
            break;
            
        }
        case 0x55: { // Fx55: Store V0 to Vx in memory starting at I
            x = (opcode & 0x0F00) >> 8;

            for (int i = 0; i <= x; ++i) {
                memory[I + i] = V[i];
            }
            // pc tăng sau khi thực hiện
            
            break;
        }
        case 0x65: { // Fx65: Load V0 to Vx from memory starting at I
            x = (opcode & 0x0F00) >> 8;
            for (int i = 0; i <= x; ++i) {
                V[i] = memory[I + i];
            }
            
            break;
        }
        
        default:
            std::cout << "Unknown Fx opcode: " << std::hex << opcode << "\n";
            
        }
        break;
    }
        default:
            std::cout << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
            
            break;
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
    printf("Opcode at 0x3DC: %02X%02X\n", memory[0x3DC], memory[0x3DC + 1]);

    return true;

}
    