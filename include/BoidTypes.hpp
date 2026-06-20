#pragma once
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <cstdint>

struct alignas(16) Boid {
    glm::vec2 position;
    glm::vec2 velocity;
};

struct alignas(16) SimParams {
    float deltaT;
    uint32_t boidCount;
    float rule1Distance;
    float rule2Distance;
    float rule3Distance;
    float rule1Scale;
    float rule2Scale;
    float rule3Scale;
    float maxSpeed;
    float audioReactivity;
    float bassLevel;
    float trebleLevel;
    float padding;
};