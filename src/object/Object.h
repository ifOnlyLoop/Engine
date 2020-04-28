#pragma once

//#include "../graphics/ray/geometry.h"
#include "../scripts/transform/gvec.hpp"

extern const float kInfinity = std::numeric_limits<float>::max();
extern const float kEpsilon = 1e-8;
extern const gvec<float> kDefaultBackgroundColor = gvec<float>(3);

class Object
{
 public:
             Object() {}
    virtual ~Object() {}

    virtual bool intersect (
        const gvec<float> &, 
        const gvec<float> &, 
        float &, 
        uint32_t &, 
        gvec<float> &
    )   const = 0;
    
    virtual void getSurfaceProperties (
        const gvec<float> &, 
        const gvec<float> &, 
        const uint32_t &, 
        const gvec<float> &, 
        gvec<float> &, 
        gvec<float> &
    )   const = 0;
    /*
    virtual void link (
        int, // vertex  index
        int, // texture index
        int  // normal  index
    )   = 0;

	virtual void pushUnlinkedNormal (
        float,
        float, 
        float
    )   = 0;
	
    virtual void pushUnlinkedTexture (
        float,
        float, 
        float
    )   = 0;

	virtual void clearTempData() = 0;
    */
};


