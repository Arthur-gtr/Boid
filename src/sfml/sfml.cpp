#include "sfml.hpp"

void sfml::openWindow(size_t heigth, size_t width, const std::string &windowName) 
{
  sf::VideoMode video({static_cast<unsigned int>(heigth), static_cast<unsigned int>(width)});
  this->_window.create(video, windowName);
}

void sfml::closeWindow()
{
  if (this->_window.isOpen())
    this->_window.close();
}
