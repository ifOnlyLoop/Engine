#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SCREEN_HEIGHT 22
#define SCREEN_WIDTH  78

// Simulated frame buffer
char Screen[SCREEN_HEIGHT][SCREEN_WIDTH];

void SetPixel(long x, long y, char color)
{
  if ((x < 0) || (x >= SCREEN_WIDTH) ||
      (y < 0) || (y >= SCREEN_HEIGHT))
  {
    return;
  }

  Screen[y][x] = color;
}
 
void Visualize(void)
{
  long x, y;

  for (y = 0; y < SCREEN_HEIGHT; y++)
  {
    for (x = 0; x < SCREEN_WIDTH; x++)
    {
      printf("%c", Screen[y][x]);
    }

    printf("\n");
  }
}

typedef struct
{
  long x, y;
  unsigned char color;
} Point2D;


// min X and max X for every horizontal line within the triangle
long ContourX[SCREEN_HEIGHT][2];

#define ABS(x) ((x >= 0) ? x : -x)

// Scans a side of a triangle setting min X and max X in ContourX[][]
// (using the Bresenham's line drawing algorithm).
void ScanLine(long x1, long y1, long x2, long y2)
{
  long sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

  sx = x2 - x1;
  sy = y2 - y1;

  if (sx > 0) dx1 = 1;
  else if (sx < 0) dx1 = -1;
  else dx1 = 0;

  if (sy > 0) dy1 = 1;
  else if (sy < 0) dy1 = -1;
  else dy1 = 0;

  m = ABS(sx);
  n = ABS(sy);
  dx2 = dx1;
  dy2 = 0;

  if (m < n)
  {
    m = ABS(sy);
    n = ABS(sx);
    dx2 = 0;
    dy2 = dy1;
  }

  x = x1; y = y1;
  cnt = m + 1;
  k = n / 2;

  while (cnt--)
  {
    if ((y >= 0) && (y < SCREEN_HEIGHT))
    {
      if (x < ContourX[y][0]) ContourX[y][0] = x;
      if (x > ContourX[y][1]) ContourX[y][1] = x;
    }

    k += n;
    if (k < m)
    {
      x += dx2;
      y += dy2;
    }
    else
    {
      k -= m;
      x += dx1;
      y += dy1;
    }
  }
}

void DrawTriangle(Point2D p0, Point2D p1, Point2D p2)
{
  int y;

  for (y = 0; y < SCREEN_HEIGHT; y++)
  {
    ContourX[y][0] = LONG_MAX; // min X
    ContourX[y][1] = LONG_MIN; // max X
  }

  ScanLine(p0.x, p0.y, p1.x, p1.y);
  ScanLine(p1.x, p1.y, p2.x, p2.y);
  ScanLine(p2.x, p2.y, p0.x, p0.y);

  for (y = 0; y < SCREEN_HEIGHT; y++)
  {
    if (ContourX[y][1] >= ContourX[y][0])
    {
      long x = ContourX[y][0];
      long len = 1 + ContourX[y][1] - ContourX[y][0];

      // Can draw a horizontal line instead of individual pixels here
      while (len--)
      {
        SetPixel(x++, y, p0.color);
      }
    }
  }
}

int main(void)
{
  Point2D p0, p1, p2;

  // clear the screen
  memset(Screen, ' ', sizeof(Screen));

  // generate random triangle coordinates
  srand((unsigned)time(NULL));

  p0.x = rand() % SCREEN_WIDTH;
  p0.y = rand() % SCREEN_HEIGHT;

  p1.x = rand() % SCREEN_WIDTH;
  p1.y = rand() % SCREEN_HEIGHT;

  p2.x = rand() % SCREEN_WIDTH;
  p2.y = rand() % SCREEN_HEIGHT;

  // draw the triangle
  p0.color = '1';
  DrawTriangle(p0, p1, p2);

  // also draw the triangle's vertices
  SetPixel(p0.x, p0.y, '*');
  SetPixel(p1.x, p1.y, '*');
  SetPixel(p2.x, p2.y, '*');

  Visualize();

  return 0;
}





























/*/#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono> 

//#include "./quat.h"
#include "../../graphics/raster/Camera.hpp"
#include "./src/object/handleObject.h"
#include "../../scripts/transform/gmat.hpp"

using namespace std;
using namespace chrono;

// SPECS
const uint32_t imageWidth  = 640; 
const uint32_t imageHeight = 480;  
//const uint32_t ntris = 3156; 
const float nearClippingPLane = 1; 
const float farClippingPLane = 1000; 
float focalLength = 20; // in mm 
// 35mm Full Aperture in inches
float filmApertureWidth = 0.980; 
float filmApertureHeight = 0.735; 

#define min3(a,b,c) min(a,min(b, c))
#define max3(a,b,c) max(a,max(b, c))

float edgeFunction(gvec<float> &a, gvec<float> &b, gvec<float> &c) 
{ 
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); 
}


int main()
{ 

    int n = 0;
    // measure import time
    auto start = high_resolution_clock::now();
    // import file
    handleObject obj("/home/oo/iOL/Engine/src/scripts/fixer/src/object/teddy.obj");
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << endl << duration.count() / 1e6 << endl;
    
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

    struct buff{ unsigned char x = 255, y = 255, z = 255; };
    buff *frameBuffer = new buff[ imageWidth * imageHeight]; 
    float*depthBuffer = new float[imageWidth * imageHeight]; 
    



    for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) 
        depthBuffer[i] = farClippingPLane; 
 
    auto t_start = std::chrono::high_resolution_clock::now(); 
    
    int ntris = obj.objData.faceList.size();

    for (uint32_t i = 0; i < ntris; ++i) 
    { 
        const gvec<float> &v0 = obj.objData.vertexList[obj.objData.faceList[i][0]].location; 
        const gvec<float> &v1 = obj.objData.vertexList[obj.objData.faceList[i][1]].location;  
        const gvec<float> &v2 = obj.objData.vertexList[obj.objData.faceList[i][2]].location; 

        gvec<float>v0Raster(1,4), v1Raster(1,4), v2Raster(1,4); 

         
        cam.getRaster(world2camera,v0, v0Raster); 
        cam.getRaster(world2camera,v1, v1Raster); 
        cam.getRaster(world2camera,v2, v2Raster); 
 
        
        gvec<float> st0 = 150;//stn[txtris[i][0]];
        gvec<float> st1 = 200;//stn[txtris[i][1]];
        gvec<float> st2 = 250;//stn[txtris[i][2]];
 
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
 
        float area = edgeFunction(v0Raster, v1Raster, v2Raster); 

        for (uint32_t y = y0; y <= y1; ++y) 
        { 
            for (uint32_t x = x0; x <= x1; ++x) 
            { 
                gvec<float> pixelSample(3);
                pixelSample[0]= x + 0.5;
                pixelSample[1]= y + 0.5;
                pixelSample[2]= 0;

                float w0 = edgeFunction(v1Raster, v2Raster, pixelSample); 
                float w1 = edgeFunction(v2Raster, v0Raster, pixelSample); 
                float w2 = edgeFunction(v0Raster, v1Raster, pixelSample); 
                
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) { 
                    w0 /= area; 
                    w1 /= area; 
                    w2 /= area; 
                    float oneOverZ = v0Raster[2] * w0 + v1Raster[2] * w1 + v2Raster[2] * w2; 
                    float z = 1 / oneOverZ; 

                    if (z < depthBuffer[y * imageWidth + x]) { 
                        depthBuffer[y * imageWidth + x] = z; 
 
                        gvec<float> st = st0 * w0 + st1 * w1 + st2 * w2; 
 
                        st *= z; 

                        gvec<float> v0Cam(v0*world2camera), 
                                    v1Cam(v1*world2camera), 
                                    v2Cam(v2*world2camera); 
                        
                        float px = (v0Cam[0]/-v0Cam[2]) * w0 + (v1Cam[0]/-v1Cam[2]) * w1 + (v2Cam[0]/-v2Cam[2]) * w2; 
                        float py = (v0Cam[1]/-v0Cam[2]) * w0 + (v1Cam[1]/-v1Cam[2]) * w1 + (v2Cam[1]/-v2Cam[2]) * w2; 
                        
                        // pt is in camera space 
                        gvec<float> pt(3);
                        pt[0]= px*z;
                        pt[1]= py*z;
                        pt[2]= -z;
                        //pt[3]= 0;

                        gvec<float> N = (v1Cam - v0Cam).cross(v2Cam - v0Cam); 
                        N.normalize(); 
                        gvec<float> viewDirection(-pt); 
                        viewDirection.normalize(); 
                        float test = N*viewDirection;
                        float nDotView =  std::max(0.f, test);//(N*viewDirection.transpose())[0]); 
    
                        const int M = 10; 
                        float checker = (fmod(st[0] * M, 1.0) > 0.5) ^ (fmod(st[1] * M, 1.0) < 0.5); 
                        float c = 0.3 * (1 - checker) + 0.7 * checker; 
                        nDotView *= c; 
                        frameBuffer[y * imageWidth + x].x = nDotView * 255; 
                        frameBuffer[y * imageWidth + x].y = nDotView * 255; 
                        frameBuffer[y * imageWidth + x].z = nDotView * 255; 
                    } 
                } 
            } 
        } 
    } 
 
    auto t_end = std::chrono::high_resolution_clock::now(); 
	auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count(); 
	std::cerr << "Wall passed time:  " << passedTime << " ms" << std::endl; 

    std::ofstream ofs; 
    ofs.open("./test.ppm"); 
    ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    ofs.write(reinterpret_cast<char*>(&frameBuffer[0]), imageWidth * imageWidth*3);

    if (ofs.fail()) {
        cerr << "Could not write data" << endl;
        return false;
    }

    ofs.close();
     

    delete [] frameBuffer; 
    delete [] depthBuffer; 
 
 


    //#################################################
    //#################################################
    
    // measure export time
    start = high_resolution_clock::now();
    // export file
    obj.EXPORT("objRes.obj");
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << endl << duration.count() / 1e6 << endl;
    
    //system("pause");
	return 0;
}
*/