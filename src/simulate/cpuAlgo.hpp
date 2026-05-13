#pragma once

#include "bird.hpp"
#include "IDisplay.hpp"
#include "cpu2d.hpp"

#include <iostream>

glm::vec3 UpdateAlignment(const Bird &bird, const std::vector<Bird>& boidList);
glm::vec3 updateCohesion(const Bird &bird, const std::vector<Bird>& boidList);
glm::vec3 checkCollision(const Bird &bird, const std::vector<Bird>& boid);
