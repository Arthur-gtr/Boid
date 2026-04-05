#pragma once

#include "IGraphique.hpp"

#include <SFML/Graphics.hpp>

namespace sfml {
    class sfml : public IWindow
    {
        public:
            sfml();
            ~sfml();

            void openWindow(size_t heigth, size_t width, const std::string &windowName) override;
            void closeWindow() override;
            bool isOpen() const override;

            void clear() override;
            void draw(const Bird& entity) override;
            void display() override;
        private:
            sf::RenderWindow _window;

    };

    class Sprite 
    {
        sf::Sprite sprite;
    };

    sfml::sfml()
    {
    }

    sfml::~sfml()
    {
    }
}