#include <string>

#include "sfml.hpp"
#include "bird.hpp"

#include <SFML/Graphics.hpp>

void graphic::sfml::openWindow(size_t heigth, size_t width, const std::string &windowName)
{
    _window.create(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(heigth)}), windowName);
}

void graphic::sfml::closeWindow()
{
    if (_window.isOpen()) {
        _window.close();
    }
}

bool graphic::sfml::isOpen() const
{
    return _window.isOpen();
}

void graphic::sfml::clear()
{
    _window.clear(sf::Color::Black);
}

void graphic::sfml::draw(const Bird& entity)
{
    sf::CircleShape boidShape(3.0f);
    boidShape.setFillColor(sf::Color::White);
    boidShape.setPosition({entity.position.x, entity.position.y});
    
    _window.draw(boidShape);
}

void graphic::sfml::display()
{
    _window.display();
}

void graphic::sfml::handleEvent()
{
    while (const std::optional event = _window.pollEvent())
        if (event->is<sf::Event::Closed>())
            _window.close();
}