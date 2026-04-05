#pragma once

#include <vector>
#include <memory>

#include "IDisplay.hpp"
#include "ISimulator.hpp"
#include "bird.hpp"


class Boid {
    public:
        Boid(const std::string &graphicLib, const std::string &computeMethode, unsigned int nbBird);
        void runBoid(void);
        ~Boid();
    private:
        unsigned int _nbBird;
        std::vector<Bird> _birdList;

        std::unique_ptr<IDisplay> _window;
        std::unique_ptr<ISimulator> _simulator;
};