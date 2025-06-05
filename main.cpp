#include <SFML/Graphics.hpp>
#include <iostream>
#include "Chip8.h"

const int SCALE = 10;
const int WINDOW_WIDTH = 64 * SCALE;
const int WINDOW_HEIGHT = 32 * SCALE;

int main() {
    Chip8 chip8;

    if (!chip8.LoadRom("C:/Users/ASUS/Downloads/4-flags.ch8")) {
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
            if (e.type == sf::Event::Closed)
                window.close();
        }

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
