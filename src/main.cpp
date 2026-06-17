#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <cmath>

#include "simulate/VulkanCore.hpp"
#include "simulate/SimulationEngine.hpp"

int main() {
    try {
        VulkanCore core;
        SimulationEngine sim(core, 1000);

        sf::RenderWindow window(sf::VideoMode({1920, 1080}), "Simulation Boids GPU + SFML");
        window.setFramerateLimit(60);

        sf::ConvexShape boidShape(3);
        boidShape.setPoint(0, sf::Vector2f(0.0f, -6.0f));
        boidShape.setPoint(1, sf::Vector2f(-4.0f, 6.0f));
        boidShape.setPoint(2, sf::Vector2f(4.0f, 6.0f));
        boidShape.setFillColor(sf::Color::Cyan);

        while (window.isOpen()) {
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
            }

            sim.step();

            std::vector<Boid> boids = sim.getBoidsData();

            window.clear(sf::Color(20, 20, 25));

            for (const auto& boid : boids) {

                float x = (boid.position.x + 1.0f) * (1920.0f / 2);
                float y = (boid.position.y + 1.0f) * (1080.0f / 2);
                boidShape.setPosition(sf::Vector2f(x, y));

                float angle = atan2(boid.velocity.y, boid.velocity.x) * 180.0f / 3.14159265f;
                boidShape.setRotation(sf::degrees(angle + 90.0f));

                window.draw(boidShape);
            }

            window.display();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}