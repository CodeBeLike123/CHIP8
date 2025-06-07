#include <SFML/Graphics.hpp>
#include <iostream>
#include "Chip8.h"

const int SCALE = 10;
const int WINDOW_WIDTH = 64 * SCALE;
const int WINDOW_HEIGHT = 32 * SCALE;


std::map<sf::Keyboard::Key, int> keymap = {
{sf::Keyboard::X, 0x0},
    {sf::Keyboard::Num1, 0x1},
    {sf::Keyboard::Num2, 0x2},
    {sf::Keyboard::Num3, 0x3},
    {sf::Keyboard::Q, 0x4},
    {sf::Keyboard::W, 0x5},
    {sf::Keyboard::E, 0x6},
    {sf::Keyboard::A, 0x7},
    {sf::Keyboard::S, 0x8},
    {sf::Keyboard::D, 0x9},
    {sf::Keyboard::Z, 0xA},
    {sf::Keyboard::C, 0xB},
    {sf::Keyboard::Num4, 0xC},
    {sf::Keyboard::R, 0xD},
    {sf::Keyboard::F, 0xE},
    {sf::Keyboard::V, 0xF}, };

int main() {
    Chip8 chip8;
    chip8.Initialize();

    if (!chip8.LoadRom("C:/Users/ASUS/Downloads/6-keypad.ch8")) {
        std::cerr << "ROM failed to load\n";
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chip8 Emulator ");
    sf::RectangleShape pixel(sf::Vector2f(SCALE, SCALE));
    pixel.setFillColor(sf::Color::Cyan);

    sf::Clock clock;
    float timer = 0.f;
    const float cycleDelay = 1.0f / 60.0f;  // 60Hz 

    while (window.isOpen()) {
        sf::Event e;
        
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
            if (e.type == sf::Event::KeyPressed || e.type == sf::Event::KeyReleased) {
                bool pressed = (e.type == sf::Event::KeyPressed);

                auto it = keymap.find(e.key.code);
                if (it != keymap.end()) {
                    chip8.key[it->second] = pressed ? 1 : 0;
                }
            }
        }
        
        chip8.delayTimerCountDown();
        chip8.soundTimerCountDown();

        timer += clock.restart().asSeconds();
        while (timer >= cycleDelay) {
            chip8.EmulateCycle();
            timer -= cycleDelay;
        }

        if (chip8.drawFlag) {
            window.clear();

            for (int y = 0; y < 32; ++y) {
                for (int x = 0; x < 64; ++x) {
                    if (chip8.screen_pixels[y * 64 + x]) {
                        pixel.setPosition(x * SCALE, y * SCALE);
                        window.draw(pixel);
                    }
                }
            }

            window.display();
            chip8.drawFlag = false;
        }
    }

    return 0;
}
