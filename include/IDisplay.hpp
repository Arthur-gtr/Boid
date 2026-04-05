#pragma once

#include <vector>
#include <string>

struct Bird;

class IDisplay {
    public:
        virtual ~IDisplay() = default;

        virtual void openWindow(size_t heigth, size_t width, const std::string &windowName) = 0;
        virtual void closeWindow() = 0;
        virtual bool isOpen() const = 0;

        virtual void handleEvent() = 0;

        virtual void clear() = 0;
        virtual void draw(const Bird& entity) = 0;
        virtual void display() = 0;
};