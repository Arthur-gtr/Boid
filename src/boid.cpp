#include <memory>
#include <iostream>
#include <chrono>
#include <random>

#include "sfml.hpp"
#include "cpu2d.hpp"
#include "boid.hpp"




Boid::Boid(const std::string &graphicLib, const std::string &computeMethode, unsigned int nbBird) : _nbBird(nbBird)
{
    if (graphicLib == "SFML")
        this->_window = std::make_unique<graphic::sfml>();
    else
        throw std::runtime_error("Error pls load an avaible graphic LIB [SFML]");

    if (computeMethode == "CPU-2D")
        this->_simulator = std::make_unique<simulate::CPU2D>();
    else if (computeMethode == "CPU-3D")
        this->_simulator = std::make_unique<simulate::CPU2D>();
    else
        throw std::runtime_error("Error pls load an avaible simulate method [CPU-2D]");

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(1920.f)); 
    std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(1080.f));
    
    std::uniform_real_distribution<float> distZ((computeMethode == "CPU-2D") ? (0.0f, 0.0f) : (0.0f, 1000.0f));
    std::uniform_real_distribution<float> distVel(-50.0f, 50.0f);

    for (size_t i = 0; i < nbBird; i++){
        Bird bird = {{distX(gen), distY(gen), distZ(gen)}, {distVel(gen), distVel(gen), distVel(gen)}};
        this->_birdList.push_back(bird);
    }
}


Boid::~Boid()
{
    /*Close the windo*/

    /*Realease the vector*/
}

void Boid::runBoid()
{
    this->_window->openWindow(1080, 1920, "BoidSimulation:", this->_windowInfo);

    auto previousTime = std::chrono::steady_clock::now();

    while(this->_window->isOpen()){
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> durationObject = currentTime - previousTime;
        float elapsedTime = durationObject.count();
        previousTime = currentTime;
        this->_window->handleEvent();
        this->_simulator->update(this->_birdList, this->_windowInfo, elapsedTime);
        this->_window->clear();
        for (Bird &bird : this->_birdList)
            this->_window->draw(bird);
        this->_window->display();
    }
}