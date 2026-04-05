
#include "bird.hpp"
#include "IDisplay.hpp"
#include "cpu2d.hpp"

void simulate::CPU2D::update(std::vector<Bird>& boid, const windowInfo &windowInfo, float elapsedTime)
{
    for (Bird &bird : boid){
        if (bird.position.x > windowInfo.width)
            bird.position.x = 0;
        if (bird.position.y > windowInfo.heigth)
            bird.position.y = 0;
        if (bird.position.x < 0)
            bird.position.x = windowInfo.width;
        if (bird.position.y < 0)
            bird.position.y = windowInfo.heigth; 
        bird.position.x += 1000 * elapsedTime * bird.velocity.x;
        bird.position.y += 1000 * elapsedTime * bird.velocity.y;
    }   
}