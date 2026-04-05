#pragma once

#include <vector>

struct Bird;

class ISimulator {
    public:
        virtual ~ISimulator() = default;
        virtual void update(std::vector<Bird>& boid) = 0;
};