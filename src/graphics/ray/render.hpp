#pragma once 

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <utility>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <chrono>
#include <string>

//#include "geometry.h"
#include "../../object/Object.h"
#include "../../scripts/transform/gvec.hpp"
// constants

//static const float kInfinity = std::numeric_limits<float>::max();
//static const float kEpsilon = 1e-8;
//static const Vec3f kDefaultBackgroundColor = Vec3f(0,0,0);

// external functions

struct buff{ float x = 0, y = 0, z = 0; };

inline
float clamp (
    const float &lo, 
    const float &hi, 
    const float &v
)
{ 
    return std::max(lo, std::min(hi, v)); 
}

inline
float deg2rad (const float &deg)
{ 
    return deg * M_PI / 180; 
}

// structures 

struct Options
{
    uint32_t width  = 100;//640;
    uint32_t height = 100;//480;
    float fov = 15;
    gvec<float> backgroundColor = kDefaultBackgroundColor;
    gmat<float> cameraToWorld;
    //Matrix44f cameraToWorld;
};

// 
bool trace (
    const gvec<float> &orig,  // ray 
    const gvec<float> &dir,   // ray
    const std::vector<std::unique_ptr<Object>> &objects, //objs
    float &tNear,       // depth buffer
    uint32_t &index,    // intersected triangle index
    gvec<float> &uv,    // texturing
    Object **hitObject  // intersected obj
) {
    // intersected object
    *hitObject = nullptr;
    
    for (uint32_t k = 0; k < objects.size(); ++k) 
    {
        float tNearTriangle = kInfinity;
        
        uint32_t indexTriangle;
        gvec<float> uvTriangle(3);
        
        if (
            objects[k]->intersect(orig, dir, tNearTriangle, indexTriangle, uvTriangle)
            && tNearTriangle < tNear
        ) {
            *hitObject = objects[k].get();
            tNear = tNearTriangle;
            index = indexTriangle;
            uv = uvTriangle;
        }
    }

    return (*hitObject != nullptr);
}
 
buff castRay (
    const gvec<float> &orig, 
    const gvec<float> &dir,
    const std::vector<std::unique_ptr<Object>> &objects,
    const Options &options
) {
    gvec<float> hitColor = options.backgroundColor;
    float tnear = kInfinity;
    gvec<float> uv(3);
    uint32_t index = 0;
    Object *hitObject = nullptr;
    
    if (
        trace(orig, dir, objects, tnear, index, uv, &hitObject)
    ) {
        gvec<float> hitPoint = orig + dir * tnear;
        gvec<float> hitNormal;//3
        gvec<float> hitTexCoordinates;//2
        //std::cerr<<"\nIT DID IT: "<<index<<std::endl;
        hitObject->getSurfaceProperties (
            hitPoint, 
            dir, 
            index, 
            uv, 
            hitNormal, 
            hitTexCoordinates
        );
        
        float NdotView = std::max(0.f, hitNormal*(-dir));
        const int M = 10;
        float checker = (fmod(hitTexCoordinates[0] * M, 1.0) > 0.5) ^ (fmod(hitTexCoordinates[1] * M, 1.0) < 0.5);
        float c = 0.3 * (1 - checker) + 0.7 * checker;
        
        hitColor = c * NdotView; //Vec3f(uv.x, uv.y, 0);
    }
    buff color;
    color.x=hitColor[0];
    color.y=hitColor[1];
    color.z=hitColor[2];
    return color;//hitColor;
}


void render(
    const Options &options,
    const std::vector<std::unique_ptr<Object>> &objects,
    const uint32_t &frame)
{
    std::unique_ptr<buff[]>  framebuffer(new buff  [options.width * options.height]); //std::unique_ptr<buff[]> 
    std::unique_ptr<float[]> depthbuffer(new float [options.width * options.height]); //std::unique_ptr<float[]>
    
    for (uint32_t i = 0; i < options.width * options.height; ++i) 
            depthbuffer[i] = INT32_MAX; 
    
    buff *pix = framebuffer.get();

    float scale = tan(deg2rad(options.fov * 0.5));
    float imageAspectRatio = options.width / (float)options.height;
    
    gvec<float> orig(1,4);
                orig[3]=1;
    orig=orig*options.cameraToWorld;
    gvec<float> dir(1,4);

    auto timeStart = std::chrono::high_resolution_clock::now();
    
    for (uint32_t j = 0; j < options.height; ++j) 
    { 
        for (uint32_t i = 0; i < options.width; ++i) 
        {
            // generate primary ray direction
            dir[0] = (2 * (i + 0.5) / (float)options.width - 1) * imageAspectRatio * scale;
            dir[1] = (1 - 2 * (j + 0.5) / (float)options.height) * scale;
            dir[2] = -1;
            
            dir=dir*options.cameraToWorld;
            dir.normalize();
            //options.cameraToWorld.multDirMatrix(Vec3f(x, y, -1), dir);
            
            *(pix++) = castRay(orig, dir, objects, options);
        }
        fprintf(stderr, "\r%3d%c", uint32_t(j / (float)options.height * 100), '%');
    }
    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
    fprintf(stderr, "\rDone: %.2f (sec)\n", passedTime / 1000);
    
    // save framebuffer to file
    char buff[256];
    sprintf(buff, "out.%04d.ppm", frame);
    std::ofstream ofs;
    ofs.open(buff);
    ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
    for (uint32_t i = 0; i < options.height * options.width; ++i) {
        char r = (char)(255 * clamp(0, 1, framebuffer[i].x));
        char g = (char)(255 * clamp(0, 1, framebuffer[i].y));
        char b = (char)(255 * clamp(0, 1, framebuffer[i].z));
        ofs << r << g << b;
    }
    ofs.close();    
}

