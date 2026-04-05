#pragma once
#include "bird.hpp"
#include "IGraphique.hpp"

#include <vector>
#include <memory>

class Boid {
    public:
        Boid(unsigned int nbBird);
        void runBoid(void);
        ~Boid();
    private:
        void appliedBoidAlgo();
        void updateBoidCoord();
        void displayTheBoid(); 
        std::vector<Bird> _birdList;

        std::unique_ptr<IWindow> _window;
};