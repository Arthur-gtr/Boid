#pragma once

#include <vector>

#include "ISimulator.hpp"

struct Bird;

namespace simulate 
{
    class CPU2D final : public ISimulator 
    {
        public:
            ~CPU2D() = default; 
            void update(std::vector<Bird>& boid);
    };
}