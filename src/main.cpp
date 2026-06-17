#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include "simulate/VulkanEngine.hpp"

int main() {
    try {
        sf::WindowBase window(sf::VideoMode({1920, 1080}), "Boids 3D Native Vulkan");
        VulkanEngine engine(window, 10000);

        while (window.isOpen()) {
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) window.close();
            }
            engine.drawFrame();
        }
    } catch (const std::exception& e) {
        std::cerr << "error fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}