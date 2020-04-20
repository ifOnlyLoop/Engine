#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string.h>
#include <math.h>
#include <memory>
#include <algorithm>
#include "../../scripts/transform/gvec.hpp"
#include "../../scripts/transform/gmat.hpp"
#include "../../graphics/raster/Camera.hpp"
#include "./src/object/handleObject.h"

using namespace std;

#define min3(a,b,c) min(a,min(b, c))
#define max3(a,b,c) max(a,max(b, c))

float edgeFunction(gvec<float> &a, gvec<float> &b, gvec<float> &c) 
{ 
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); 
}

int fact=4;
const float 
nearClippingPLane = 1,
farClippingPLane  = 1000; 
float 
focalLength = 20; // in mm 
float // 35mm Full Aperture in inches 
filmApertureWidth  = 0.980,
filmApertureHeight = 0.735; 
uint32_t 
imageWidth  = 512*fact, 
imageHeight = 512*fact;
    

int main(int argc, char **argv)
{   
    
    handleObject obj("/home/oo/iOL/Engine/src/scripts/fixer/src/object/input.obj");

    // SET CAMERA TRANSFORM IN FRONT OF OBJECT 
    float 
        dx= obj.xmax-obj.xmin,
        dy= obj.ymax-obj.ymin,
        dd= max(dx,dy),
        dz= obj.zmin-dd/2;
        dx=(obj.xmax+obj.xmin)/2;
        dy=(obj.ymax+obj.ymin)/2;

    gmat<float> camera2world(4,4);
    camera2world.identity();
    
    camera2world(3,0)=dx;
    camera2world(3,1)=dy;
    camera2world(3,2)=-dz;
    gmat<float> world2camera(camera2world.inverse());

    Camera cam;
    // compute screen coordinates
    cam.setImage(imageWidth,imageHeight);
    cam.setFilm(filmApertureWidth,filmApertureWidth);
    //cam.setScreen();
    cam.gluPerspective (
        90,
        obj.zmin,
        obj.zmax
    );
    cam.glFrustum();

    // SET BUFFERS
    struct buff{ unsigned char x = 255, y = 255, z = 255; };
    buff *frameBuffer = new buff[ imageWidth * imageHeight]; 
    float*depthBuffer = new float[imageWidth * imageHeight]; 
    //memset(depthBuffer,obj.zmax+1,imageWidth * imageHeight);
    for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) 
            depthBuffer[i] = obj.zmin-1; 
    
    // ITERATE
    int nFace = obj.objData.faceList.size();
    for (unsigned int i = 0; i < nFace; i++)
    {
        // get tri face vertex 
        const gvec<float> &v0 = obj.objData.vertexList[obj.objData.faceList[i][0]].location; 
        const gvec<float> &v1 = obj.objData.vertexList[obj.objData.faceList[i][1]].location;  
        const gvec<float> &v2 = obj.objData.vertexList[obj.objData.faceList[i][2]].location; 
        // get raster space coord
        gvec<float> v0Raster(1,4), 
                    v1Raster(1,4), 
                    v2Raster(1,4); 
       
        cam.getRaster(world2camera,v0, v0Raster); // res: vector, size: 1x4
        cam.getRaster(world2camera,v1, v1Raster); 
        cam.getRaster(world2camera,v2, v2Raster); 

        gvec<float> st0(1,4);//stn[txtris[i][0]];
        st0 = 150;
        gvec<float> st1(1,4);//stn[txtris[i][1]];
        st1 = 200;
        gvec<float> st2(1,4);//stn[txtris[i][2]];
        st2 = 250;
 
        st0 *= v0Raster[2], st1 *= v1Raster[2], st2 *= v2Raster[2]; 
 
        float xmin = min3(v0Raster[0], v1Raster[0], v2Raster[0]); 
        float ymin = min3(v0Raster[1], v1Raster[1], v2Raster[1]); 
        float xmax = max3(v0Raster[0], v1Raster[0], v2Raster[0]); 
        float ymax = max3(v0Raster[1], v1Raster[1], v2Raster[1]); 
 
        // the triangle is out of screen
        if (xmin > imageWidth - 1 || xmax < 0 || ymin > imageHeight - 1 || ymax < 0) continue; 
        // be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
        uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(xmin))); 
        uint32_t x1 = std::min(int32_t(imageWidth) - 1, (int32_t)(std::floor(xmax))); 
        uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(ymin))); 
        uint32_t y1 = std::min(int32_t(imageHeight) - 1, (int32_t)(std::floor(ymax)));
        
        // tri area
        float area = edgeFunction(v0Raster, v1Raster, v2Raster); 
        // ROTATE PIXELS TO COLOR (SIMPLE APPROACH)
        for (uint32_t y = y0; y <= y1; ++y) 
        for (uint32_t x = x0; x <= x1; ++x) 
        { 
            //if (frameBuffer[y * imageWidth + x].x+frameBuffer[y * imageWidth + x].y+frameBuffer[y * imageWidth + x].z<255*3 ) continue;
            // pixel to point (+0.5 to get pixel center)
            gvec<float> pixel(1,4);
            pixel[0]= x + 0.5;
            pixel[1]= y + 0.5;
            pixel[2]= 0;
            // weights for bycentric coord if (<0) point outside tri
            float w0 = edgeFunction(v1Raster, v2Raster, pixel); 
            float w1 = edgeFunction(v2Raster, v0Raster, pixel); 
            float w2 = edgeFunction(v0Raster, v1Raster, pixel); 
            if (w0 < 0 || w1 < 0 || w2 < 0) continue; 
            //if (w0 >= 0 && w1 >= 0 && w2 >= 0){
            //get ratio
            w0 /= area;     w1 /= area;     w2 /= area;
            // calculate z inv
            float oneOverZ = v0Raster[2] * w0 + v1Raster[2] * w1 + v2Raster[2] * w2; 
            float z = 1 / oneOverZ; 

            // sort visibilty by depth
            if (z <= depthBuffer[y * imageWidth + x]) continue; 
            //if (z > depthBuffer[y * imageWidth + x]){
            // set new lesser depth        
            depthBuffer[y * imageWidth + x] = z;
            // set new color 
            gvec<float> st = st0 * w0 + st1 * w1 + st2 * w2; 
            // ?
            st *= z; 
            // transform original to camera (NO PROJECTION)
            gvec<float> v0Cam(v0*world2camera), 
                        v1Cam(v1*world2camera), 
                        v2Cam(v2*world2camera); 
            // ?

            float px = (v0Cam[0]/-v0Cam[2]) * w0 + (v1Cam[0]/-v1Cam[2]) * w1 + (v2Cam[0]/-v2Cam[2]) * w2; 
            float py = (v0Cam[1]/-v0Cam[2]) * w0 + (v1Cam[1]/-v1Cam[2]) * w1 + (v2Cam[1]/-v2Cam[2]) * w2; 
            
            // pt is in camera space 
            gvec<float> pt(1,4);    pt[0]= px*z;    pt[1]= py*z;    pt[2]= -z;    pt[3]= 0;
            // get face unit normal
            gvec<float> N = (v1Cam - v0Cam).cross(v2Cam - v0Cam);   N.normalize(); 
            
            // is normal facing camera (visible ?)
            gvec<float> viewDirection(-pt); viewDirection.normalize(); 
            float test = N*viewDirection;
            float nDotView =  std::max(0.f, test);//(N*viewDirection.transpose())[0]); 

            const int M = 10; // ??
            float checker = (fmod(st[0] * M, 1.0) > 0.5) ^ (fmod(st[1] * M, 1.0) < 0.5); 
            float c = 0.3 * (1 - checker) + 0.7 * checker; 
            nDotView *= c; 
            
            frameBuffer[y * imageWidth + x].x = nDotView * 255; 
            frameBuffer[y * imageWidth + x].y = nDotView * 255; 
            frameBuffer[y * imageWidth + x].z = nDotView * 255;  
            //}}
        } 
    }

    // print image
    std::ofstream ofs; 
    ofs.open("./mainppm.ppm"); 
    ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    ofs.write(reinterpret_cast<char*>(&frameBuffer[0]), imageWidth * imageWidth*3);

    if (ofs.fail()) {
        cerr << "Could not write data" << endl;
        return false;
    }

    ofs.close();

    delete [] frameBuffer; 
    delete [] depthBuffer; 

    obj.EXPORT("mainobj.obj");
    return 0;
}


    /*
    unsigned char *buffer = new unsigned char[imageWidth * imageHeight];
    memset(buffer, 0x0, imageWidth * imageHeight);
    
    int n = obj.objData.vertexList.size();

    for (uint32_t i = 0; i < n; ++i) {
        gvec<float> vertCamera(1,4), projectedVert(1,4);

        //[comment]
        // Transform to camera space
        //[/comment]

        bool onScreen = cam.getRaster(
            world2camera,
            obj.objData.vertexList[i].location,
            projectedVert
        );
        
        
        //[comment]
        // Project
        //[/comment]
        //multPointMatrix(vertCamera, projectedVert, Mproj);
        
        if (onScreen==0) continue;
        
        buffer[(int)projectedVert[1] * imageWidth + (int)projectedVert[0]] = 255;
        
    }
    
    // export to image
    std::ofstream ofs;
    ofs.open("./main.ppm");
    ofs << "P5\n" << imageWidth << " " << imageHeight << "\n255\n";
    ofs.write((char*)buffer, imageWidth * imageHeight);
    ofs.close();
    delete [] buffer;
    */

























