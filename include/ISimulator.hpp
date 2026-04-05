#pragma once

#include <vector>

struct Bird;

class ISimulator {
    public:
        virtual ~ISimulator() = default;
        virtual void update(std::vector<Bird>& boid, const windowInfo &windowInfo) = 0;
};