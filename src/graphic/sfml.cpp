#include <string>

#include "sfml.hpp"
#include "bird.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>

void graphic::sfml::openWindow(size_t heigth, size_t width, const std::string &windowName, windowInfo &windowInfo)
{
    windowInfo.heigth = heigth;
    windowInfo.width = width;
    _window.create(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(heigth)}), windowName);
    _window.setFramerateLimit(60);
    this->initBirdSprite();
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

void graphic::sfml::initBirdSprite()
{
    this->boidShape.setRadius(3.0f);
    boidShape.setFillColor(sf::Color::White);
}

void graphic::sfml::draw(const Bird& entity)
{
    float cx = _window.getSize().x / 2.0f;
    float cy = _window.getSize().y / 2.0f;

    float fov = 500.0f;
    float z = entity.position.z;

    if (z < -fov)
        return; 

    float scale = fov / (fov + z);

    float projX = cx + (entity.position.x - cx) * scale;
    float projY = cy + (entity.position.y - cy) * scale;
    
    this->boidShape.setPosition({projX, projY});

    this->boidShape.setRadius(3.0f * scale);

    uint8_t colorVal = (std::clamp(255.0f * scale, 50.0f, 255.0f));
    this->boidShape.setFillColor(sf::Color(colorVal, colorVal, colorVal));
    
    _window.draw(this->boidShape);
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