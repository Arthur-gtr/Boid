#pragma once

#include "vec2.hpp"

class Bird 
{
    public:
        void updateCoord(Vec2 coord);
        void updateDir(Vec2 dir);
        Vec2 getCoord() const;
        Vec2 getDir() const;
    private:
        Vec2 coord;
        Vec2 dir;
};