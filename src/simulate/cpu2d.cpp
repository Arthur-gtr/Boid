
#include "bird.hpp"
#include "IDisplay.hpp"
#include "cpu2d.hpp"

void simulate::CPU2D::update(std::vector<Bird>& boid, const windowInfo &windowInfo)
{
    for (Bird &bird : boid){
        if (bird.position.x >= windowInfo.width)
            bird.position.x = 0;
        bird.position.x += 1;
    }   
}