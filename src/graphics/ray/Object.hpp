#pragma once

#include "geometry.h"

extern const float kInfinity = std::numeric_limits<float>::max();
extern const float kEpsilon = 1e-8;
extern const Vec3f kDefaultBackgroundColor = Vec3f(0,0,0);

class Object
{
 public:
             Object() {}
    virtual ~Object() {}

    virtual bool intersect (
        const Vec3f &, 
        const Vec3f &, 
        float &, 
        uint32_t &, 
        Vec2f &
    )   const = 0;
    
    virtual void getSurfaceProperties (
        const Vec3f &, 
        const Vec3f &, 
        const uint32_t &, 
        const Vec2f &, 
        Vec3f &, 
        Vec2f &
    )   const = 0;
};
