
#include "bird.hpp"
#include "IDisplay.hpp"
#include "cpu2d.hpp"

#include <iostream>

glm::vec3 checkCollision(Bird &bird, std::vector<Bird>& boid)
{
    glm::vec3 outForce(0.0f, 0.0f, 0.0f);
    size_t nbNeighbor = 0;

    for (Bird &bird_ : boid){
        if (&bird == &bird_)
            continue;
        float dist = glm::distance(bird.position, bird_.position);
        if (dist > 0.00001f && dist < DANGER_ZONE){
            glm::vec3 tmpOutForce = bird.position - bird_.position;
            tmpOutForce = glm::normalize(tmpOutForce);
            tmpOutForce = tmpOutForce / dist;
            outForce += tmpOutForce;
            nbNeighbor++;
        }
    }

    if (nbNeighbor > 0)
        outForce /= static_cast<float>(nbNeighbor);
    return outForce;
}

void simulate::CPU2D::update(std::vector<Bird>& boid, const windowInfo &windowInfo, float elapsedTime)
{
    std::vector<glm::vec3> newVelocities(boid.size());

    for (size_t i = 0; i < boid.size(); i++) {
        glm::vec3 forceFuite = checkCollision(boid[i], boid);
        glm::vec3 futureVelocity = boid[i].velocity + forceFuite;

        if (glm::length(futureVelocity) > 0.0001f)
            futureVelocity = glm::normalize(futureVelocity);
        
        newVelocities[i] = futureVelocity;
    }

    for (size_t i = 0; i < boid.size(); i++) {
        boid[i].velocity = newVelocities[i];
        
        float speed = 200.0f; 
        boid[i].position.x += speed * elapsedTime * boid[i].velocity.x;
        boid[i].position.y += speed * elapsedTime * boid[i].velocity.y;

        if (boid[i].position.x > windowInfo.width)
            boid[i].position.x = 0;
        if (boid[i].position.y > windowInfo.heigth)
            boid[i].position.y = 0;
        if (boid[i].position.x < 0)
            boid[i].position.x = windowInfo.width;
        if (boid[i].position.y < 0)
            boid[i].position.y = windowInfo.heigth; 
    }   
}