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
#include "TriMesh.hpp"
#include "render.hpp"

// for camera/screen use

int main(int argc, char **argv)
{
    // setting up options
    Options options;
    //options.cameraToWorld[3][2] = 10;
    Matrix44f tmp = Matrix44f(0.707107, -0.331295, 0.624695, 0, 0, 0.883452, 0.468521, 0, -0.707107, -0.331295, 0.624695, 0, -1.63871, -5.747777, -40.400412, 1);
    options.cameraToWorld = tmp.inverse();
    options.fov = 50.0393;
#if 1
    std::vector<std::unique_ptr<Object>> objects;
    TriMesh *mesh = loadPolyMeshFromFile("/home/oo/iOL/Engine/src/graphics/ray/cow.geo");
    if (mesh != nullptr) objects.push_back(std::unique_ptr<Object>(mesh));
    
    // finally, render
    render(options, objects, 0);
#else
    for (uint32_t i = 0; i < 10; ++i) {
        int divs = 5 + i;
        // creating the scene (adding objects and lights)
        std::vector<std::unique_ptr<Object>> objects;
        TriMesh *mesh = generatePolyShphere(2, divs);
        objects.push_back(std::unique_ptr<Object>(mesh));
    
        auto timeStart = std::chrono::high_resolution_clock::now();
        // finally, render
        render(options, objects, i);
        auto timeEnd = std::chrono::high_resolution_clock::now();
        auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
        std::cerr << mesh->numTris << " " << passedTime << std::endl;
    }
#endif

    return 0;
}