
#include "cpuAlgo.hpp"

void simulate::CPU2D::update(std::vector<Bird>& boid, const windowInfo &windowInfo, float elapsedTime)
{
    std::vector<glm::vec3> newVelocities(boid.size());

    for (size_t i = 0; i < boid.size(); i++) {
        glm::vec3 separation = checkCollision(boid[i], boid);
        glm::vec3 alignement = UpdateAlignment(boid[i], boid);
        glm::vec3 cohesion = updateCohesion(boid[i], boid);
        
        float weightSeparation = 0.8f;
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
