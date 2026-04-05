#include <memory>

#include "sfml.hpp"
#include "boid.hpp"

Boid::Boid(const std::string &graphicLib, const std::string &computeMethode, unsigned int nbBird) : _nbBird(nbBird)
{
    if (graphicLib == "SFML")
        this->_window = std::make_unique<graphic::sfml>();

    if (graphicLib == "CPU-2D")
    //    this->_simulator = std::make_unique<simulate::CPU2D>();
    /*Init window*/

    /*Init vector of bird*/
}

Boid::~Boid()
{
    /*Close the windo*/

    /*Realease the vector*/
}

void Boid::runBoid()
{
    this->_window->openWindow(1080, 1920, "BoidSimulation:", this->windowInfo);

    while(this->_window->isOpen()){
        this->_window->handleEvent();
        this->_simulator->update(this->_birdList, this->windowInfo);
        for (auto bird : this->_birdList)
            this->_window->draw(bird);
    }
}
