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

#include "render.hpp"

#include "../../object/TriMesh.h"
#include "../../object/handleObject.h"

#include "../image/Texture.hpp"

auto timeStart = std::chrono::high_resolution_clock::now();
auto timeEnd = std::chrono::high_resolution_clock::now();
auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
        
#define getTime(A) timeStart = std::chrono::high_resolution_clock::now(); A; timeEnd = std::chrono::high_resolution_clock::now(); passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count(); std::cerr << "Time: " << passedTime/1000 << std::endl;


int main(int argc, char **argv)
{
    //Texture  texture ("/home/oo/iOL/Engine/src/scripts/fixer/tests/test0.bmp");
    TriMesh *obj = new TriMesh;//*mesh;
    handleObject objHandler;
    
    getTime (   
        objHandler.IMPORT("/home/oo/iOL/Engine/src/scripts/fixer/tests/test0.obj",obj);
    );
    //mesh=&obj;
    //return 0;

    // setting up options (camera)

    Options options;
    options.cameraToWorld.resize(4,4);
    options.cameraToWorld.identity();
    
    float 
        dx= obj->bmax[0]-obj->bmin[0],
        dy= obj->bmax[1]-obj->bmin[1],
        dz= obj->bmin[2]-0.5*std::max(dx,dy);

    dx=0.5*(obj->bmax[0]+obj->bmin[0]);
    dy=0.5*(obj->bmax[1]+obj->bmin[1]);

    options.cameraToWorld(3,0)=dx;
    options.cameraToWorld(3,1)=dy;
    options.cameraToWorld(3,2)=-dz;
    //options.cameraToWorld(2,2)=-1;
    //options.cameraToWorld(2,2)=-1;    
    options.fov = 130;

    std::vector<std::unique_ptr<Object>> objects;
    if (obj != nullptr) objects.push_back(std::unique_ptr<Object>(obj));    
    // finally, render
    render(options, objects, 0);

    //getTime (   
    //    objHandler.EXPORT("reyAyaRes.obj",obj);
    //);
    
    //delete[] mesh;
    return 0;
}
