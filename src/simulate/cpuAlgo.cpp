#include "bird.hpp"
#include "IDisplay.hpp"
#include "cpu2d.hpp"

#include <iostream>

glm::vec3 UpdateAlignment(const Bird &bird, const std::vector<Bird>& boidList)
{
    glm::vec3 AverageSpeed(0.0f, 0.0f, 0.0f);
    size_t nbNeighbor = 0;
    const float VISION_ZONE = DANGER_ZONE * 2; 

    for (const Bird &other : boidList) {
        if (&bird == &other) continue;
        
        float dist = glm::distance(bird.position, other.position);
        
        if (dist > 0.0001f && dist < VISION_ZONE) {
            AverageSpeed += other.velocity;
            nbNeighbor++;
        }
    }

    if (nbNeighbor > 0) {
        AverageSpeed /= static_cast<float>(nbNeighbor);
        AverageSpeed = glm::normalize(AverageSpeed);
    }
        
    return AverageSpeed;
}

glm::vec3 updateCohesion(const Bird &bird, const std::vector<Bird>& boidList)
{
    glm::vec3 massCenter(0.0f, 0.0f, 0.0f);
    size_t nbNeighbor = 0;
    const float VISION_ZONE = DANGER_ZONE * 2;

    for (const Bird &other : boidList) {
        if (&bird == &other) continue;
        
        float dist = glm::distance(bird.position, other.position);
        
        if (dist > 0.0001f && dist < VISION_ZONE) {
            massCenter += other.position;
            nbNeighbor++;
        }
    }

    if (nbNeighbor > 0) {
        massCenter /= static_cast<float>(nbNeighbor);
        glm::vec3 directionCentre = massCenter - bird.position;
        directionCentre = glm::normalize(directionCentre);
        return directionCentre;
    }
        
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 checkCollision(const Bird &bird, const std::vector<Bird>& boid)
{
    glm::vec3 outForce(0.0f, 0.0f, 0.0f);
    size_t nbNeighbor = 0; 

    for (const Bird &other : boid){
        if (&bird == &other)
            continue;
            
        float dist = glm::distance(bird.position, other.position);
        
        if (dist > 0.0001f && dist < DANGER_ZONE){
            glm::vec3 directionFuite = bird.position - other.position;
            directionFuite = glm::normalize(directionFuite);

            float intensite = (DANGER_ZONE - dist) / DANGER_ZONE;

            outForce += directionFuite * intensite;
            nbNeighbor++;
        }
    }

    if (nbNeighbor > 0)
        outForce /= static_cast<float>(nbNeighbor);
        
    return outForce;
}