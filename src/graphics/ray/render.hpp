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
#include "Object.hpp"
// constants

//static const float kInfinity = std::numeric_limits<float>::max();
//static const float kEpsilon = 1e-8;
//static const Vec3f kDefaultBackgroundColor = Vec3f(0,0,0);

// external functions

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
    uint32_t width  = 640;
    uint32_t height = 480;
    float fov = 15;
    Vec3f backgroundColor = kDefaultBackgroundColor;
    Matrix44f cameraToWorld;
};

// 
bool trace (
    const Vec3f &orig,  // ray 
    const Vec3f &dir,   // ray
    const std::vector<std::unique_ptr<Object>> &objects, //objs
    float &tNear,       // depth buffer
    uint32_t &index,    //
    Vec2f &uv,          // texturing
    Object **hitObject  // intersected obj
) {
    *hitObject = nullptr;
    
    for (uint32_t k = 0; k < objects.size(); ++k) 
    {
        float tNearTriangle = kInfinity;
        
        uint32_t indexTriangle;
        Vec2f    uvTriangle;
        
        if (objects[k]->intersect(orig, dir, tNearTriangle, indexTriangle, uvTriangle) && tNearTriangle < tNear) {
            *hitObject = objects[k].get();
            tNear = tNearTriangle;
            index = indexTriangle;
            uv = uvTriangle;
        }
    }

    return (*hitObject != nullptr);
}
 
Vec3f castRay(
    const Vec3f &orig, const Vec3f &dir,
    const std::vector<std::unique_ptr<Object>> &objects,
    const Options &options)
{
    Vec3f hitColor = options.backgroundColor;
    float tnear = kInfinity;
    Vec2f uv;
    uint32_t index = 0;
    Object *hitObject = nullptr;
    if (trace(orig, dir, objects, tnear, index, uv, &hitObject)) {
        Vec3f hitPoint = orig + dir * tnear;
        Vec3f hitNormal;
        Vec2f hitTexCoordinates;
        hitObject->getSurfaceProperties(hitPoint, dir, index, uv, hitNormal, hitTexCoordinates);
        float NdotView = std::max(0.f, hitNormal.dotProduct(-dir));
        const int M = 10;
        float checker = (fmod(hitTexCoordinates.x * M, 1.0) > 0.5) ^ (fmod(hitTexCoordinates.y * M, 1.0) < 0.5);
        float c = 0.3 * (1 - checker) + 0.7 * checker;
        
        hitColor = c * NdotView; //Vec3f(uv.x, uv.y, 0);
    }

    return hitColor;
}


void render(
    const Options &options,
    const std::vector<std::unique_ptr<Object>> &objects,
    const uint32_t &frame)
{
    std::unique_ptr<Vec3f []> framebuffer(new Vec3f[options.width * options.height]);
    Vec3f *pix = framebuffer.get();
    float scale = tan(deg2rad(options.fov * 0.5));
    float imageAspectRatio = options.width / (float)options.height;
    Vec3f orig;
    options.cameraToWorld.multVecMatrix(Vec3f(0), orig);
    auto timeStart = std::chrono::high_resolution_clock::now();
    for (uint32_t j = 0; j < options.height; ++j) {
        for (uint32_t i = 0; i < options.width; ++i) {
            // generate primary ray direction
            float x = (2 * (i + 0.5) / (float)options.width - 1) * imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)options.height) * scale;
            Vec3f dir;
            options.cameraToWorld.multDirMatrix(Vec3f(x, y, -1), dir);
            dir.normalize();
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
