
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
void simulate::CPU2D::update(std::vector<Bird>& boid, const windowInfo &windowInfo, float elapsedTime)
{
    std::vector<glm::vec3> newVelocities(boid.size());

    for (size_t i = 0; i < boid.size(); i++) {
        glm::vec3 separation = checkCollision(boid[i], boid);
        glm::vec3 alignement = UpdateAlignment(boid[i], boid);
        glm::vec3 cohesion = updateCohesion(boid[i], boid);
        
        float weightSeparation = 0.5f;
        float weightAlignement = 0.2f;
        float weightCohesion = 0.1;
        
        glm::vec3 strengthTotale = (separation * weightSeparation) + 
                                (alignement * weightAlignement) + 
                                (cohesion   * weightCohesion);
        
        glm::vec3 futureVelocity = boid[i].velocity + strengthTotale;
        
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
