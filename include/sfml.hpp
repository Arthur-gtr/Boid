#pragma once

#include "IDisplay.hpp"

#include <SFML/Graphics.hpp>

namespace graphic {
    class sfml final : public IDisplay
    {
        public:
            sfml() = default;
            ~sfml() = default;

            void openWindow(size_t heigth, size_t width, const std::string &windowName, windowInfo &windowInfo) override;
            void closeWindow() override;
            bool isOpen() const override;

            void handleEvent() override;

            void clear() override;
            void draw(const Bird& entity) override;
            void display() override;
        private:
            void initBirdSprite();
            sf::CircleShape boidShape;

            sf::RenderWindow _window;

    };
}