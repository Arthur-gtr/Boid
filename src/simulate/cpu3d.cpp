#include "cpuAlgo.hpp"

void simulate::CPU3D::update(std::vector<Bird>& boid, const windowInfo &windowInfo, float elapsedTime)
{
    std::vector<glm::vec3> newVelocities(boid.size());
    const float VISION_ZONE = DANGER_ZONE * 2; 

    glm::vec3 centerPoint(windowInfo.width / 2.0f, windowInfo.heigth / 2.0f, 500.0f);

    for (size_t i = 0; i < boid.size(); i++) {
        glm::vec3 separation(0.0f);
        glm::vec3 alignement(0.0f);
        glm::vec3 cohesion(0.0f);
        
        size_t nbAlign = 0, nbSep = 0;

        for (size_t j = 0; j < boid.size(); j++) {
            if (i == j) continue;
            
            float dist = glm::distance(boid[i].position, boid[j].position);
            
            if (dist > 0.0001f) {
                if (dist < VISION_ZONE) {
                    alignement += boid[j].velocity;
                    cohesion += boid[j].position;
                    nbAlign++;
                }
                if (dist < DANGER_ZONE) {
                    glm::vec3 dir = glm::normalize(boid[i].position - boid[j].position);
                    separation += dir * ((DANGER_ZONE - dist) / DANGER_ZONE);
                    nbSep++;
                }
            }
        }

        if (nbAlign > 0) {
            alignement = glm::normalize(alignement / static_cast<float>(nbAlign));
            cohesion = glm::normalize((cohesion / static_cast<float>(nbAlign)) - boid[i].position);
        }
        if (nbSep > 0) {
            separation /= static_cast<float>(nbSep);
        }

        glm::vec3 attraction(0.0f);
        glm::vec3 directionToCenter = centerPoint - boid[i].position;
        float distToCenter = glm::length(directionToCenter);
        
        if (distToCenter > 0.0001f) {
            float pullStrength = distToCenter / 500.0f;
            if (distToCenter < 200.0f) {
                pullStrength = 0.0f;
            }
            attraction = glm::normalize(directionToCenter) * pullStrength;
        }
        float weightSeparation = 0.8f;
        float weightAlignement = 0.2f;
        float weightCohesion = 0.1f;
        float weightAttraction = 0.05f;

        glm::vec3 futureVelocity = boid[i].velocity + 
                                  (separation * weightSeparation) + 
                                  (alignement * weightAlignement) + 
                                  (cohesion * weightCohesion) +
                                  (attraction * weightAttraction);
        
        if (glm::length(futureVelocity) > 0.0001f) 
            futureVelocity = glm::normalize(futureVelocity);
            
        newVelocities[i] = futureVelocity;
    }

    for (size_t i = 0; i < boid.size(); i++) {
        boid[i].velocity = newVelocities[i];
        
        float speed = 200.0f; 
        boid[i].position.x += speed * elapsedTime * boid[i].velocity.x;
        boid[i].position.y += speed * elapsedTime * boid[i].velocity.y;
        boid[i].position.z += speed * elapsedTime * boid[i].velocity.z;

        if (boid[i].position.x > windowInfo.width) boid[i].position.x = 0;
        if (boid[i].position.y > windowInfo.heigth) boid[i].position.y = 0;
        if (boid[i].position.x < 0) boid[i].position.x = windowInfo.width;
        if (boid[i].position.y < 0) boid[i].position.y = windowInfo.heigth; 

        float maxDepth = 1000.0f;
        if (boid[i].position.z > maxDepth) boid[i].position.z = 0;
        if (boid[i].position.z < 0) boid[i].position.z = maxDepth;
    }   
}