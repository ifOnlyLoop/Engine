#include <bits/stdc++.h>
#include <cstdio>
#include <fstream> 
#include <chrono> 
#include "../../graphics/Camera.hpp"
#include "../../scripts/transform/gvec.hpp"

using namespace std;

//#include "cow.h" 

#define min3(a,b,c) min(a,min(b, c))
#define max3(a,b,c) max(a,max(b, c))
 
 
const uint32_t imageWidth  = 640; 
const uint32_t imageHeight = 480; 
 
const uint32_t ntris = 3156; 
const float nearClippingPLane = 1; 
const float farClippingPLane = 1000; 
float focalLength = 20; // in mm 
// 35mm Full Aperture in inches
float filmApertureWidth = 0.980; 
float filmApertureHeight = 0.735; 

float edgeFunction(gvec<float> &a, gvec<float> &b, gvec<float> &c) 
{ 
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); 
} 

int main() 
{ 
    //fstream inp;
    //inp.open(BWcow.txt);
    
    int n;
    //#######################################################
    //#######################################################
    
    FILE *fp;
    fp = fopen("/home/oo/iOL/Engine/src/examples/BWcow/BWcow1.txt", "r");
    
    int doo; float foo;

    // GET TRIS
    fscanf(fp,"%d",&n);
    vector<gvec<int>> 
        tris(n,gvec<int>(3,1));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            fscanf(fp,"%d",&doo);
            tris[i][j]=doo;
        }
    }
    // GET TXT TRIS
    fclose(fp);
    fp = fopen("/home/oo/iOL/Engine/src/examples/BWcow/BWcow3.txt", "r");
    fscanf(fp,"%d",&n);
    vector<gvec<int>> 
        txtris(n,gvec<int>(3,1));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            fscanf(fp,"%d",&doo);
            txtris[i][j]=doo;
        }
    }
    fclose(fp);
    fp = fopen("/home/oo/iOL/Engine/src/examples/BWcow/BWcow2.txt", "r");
    // GET VERTEX
    fscanf(fp,"%d",&n);
    vector<gvec<float>> 
        verts(n,gvec<float>(1,4));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            fscanf(fp,"%f",&foo);
            verts[i][j]=foo;
        }
        verts[i][3]=1;
    }

    // GET TXT
    fclose(fp);
    fp = fopen("/home/oo/iOL/Engine/src/examples/BWcow/BWcow4.txt", "r");
    fscanf(fp,"%d",&n);
    vector<gvec<float>> 
        stn(n,gvec<float>(1,2));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            fscanf(fp,"%f",&foo);
            stn[i][j]=foo;
        }
    }

    // get W2C
    gmat<float> cameraToWorld(4,4);
    fscanf(fp,"%d",&n);
    for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
        fscanf(fp,"%f",&foo);
        cameraToWorld(i,j)=foo;
    }
        
    gmat<float> worldToCamera(cameraToWorld);//cameraToWorld.inverse());
    fclose(fp);
    //#######################################################
    //#######################################################
    
    Camera cam(worldToCamera);
    // compute screen coordinates
    cam.setScreen ( 
        filmApertureWidth, 
        filmApertureHeight, 
        imageWidth, 
        imageHeight, 
        kOverscan, 
        nearClippingPLane, 
        focalLength
    ); 
    
    //#######################################################
    //#######################################################

    // define the frame-buffer and the depth-buffer. Initialize depth buffer
    // to far clipping plane.
    struct buff
    {
        unsigned char x=255,y=255,z=255;
    };
    
    buff *frameBuffer = new buff[imageWidth * imageHeight]; 
    //vector<unsigned char> frameBuffer(imageWidth * imageHeight, 255);
    float *depthBuffer = new float[imageWidth * imageHeight]; 
    
    for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) 
        depthBuffer[i] = farClippingPLane; 
 
    auto t_start = std::chrono::high_resolution_clock::now(); 

    for (uint32_t i = 0; i < ntris; ++i) 
    { 
        const gvec<float> &v0 = verts[tris[i][0]]; 
        const gvec<float> &v1 = verts[tris[i][1]]; 
        const gvec<float> &v2 = verts[tris[i][2]]; 
        gvec<float>v0Raster(3), v1Raster(3), v2Raster(3); 

         
        cam.toRaster(v0, v0Raster); 
        cam.toRaster(v1, v1Raster); 
        cam.toRaster(v2, v2Raster); 
 
        v0Raster[2] = 1 / v0Raster[2], 
        v1Raster[2] = 1 / v1Raster[2], 
        v2Raster[2] = 1 / v2Raster[2]; 

        gvec<float> st0 = stn[txtris[i][0]];
        gvec<float> st1 = stn[txtris[i][1]];
        gvec<float> st2 = stn[txtris[i][2]];
 
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

        for (uint32_t y = y0; y <= y1; ++y) { 
            for (uint32_t x = x0; x <= x1; ++x) { 
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

                        gvec<float> v0Cam(v0*worldToCamera), 
                                    v1Cam(v1*worldToCamera), 
                                    v2Cam(v2*worldToCamera); 
                        
                        float px = (v0Cam[0]/-v0Cam[2]) * w0 + (v1Cam[0]/-v1Cam[2]) * w1 + (v2Cam[0]/-v2Cam[2]) * w2; 
                        float py = (v0Cam[1]/-v0Cam[2]) * w0 + (v1Cam[1]/-v1Cam[2]) * w1 + (v2Cam[1]/-v2Cam[2]) * w2; 
                        
                        // pt is in camera space 
                        gvec<float> pt(4);
                        pt[0]= px*z;
                        pt[1]= py*z;
                        pt[2]= -z;
                        pt[3]= 0;

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
    ofs.open("./output.ppm"); 
    /*
    for (uint32_t i = 0; i < imageWidth * imageHeight; ++i)
    {
        for (int j = 0; j < 3; j++)
        {
            ofs<<(int)frameBuffer[i][j]<<' ';
        }
        ofs<<endl;
    }
    */
    ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    //ofs.write((char*)frameBuffer, imageWidth * imageWidth * 3);
    //ofs.close(); 
    
    
    /*for (int i = 0; i < size; i++) {
        temp[i * 3] = static_cast<unsigned char>(buff[i].x * 255);
        temp[i * 3 + 1] = static_cast<unsigned char>(buff[i].y * 255);
        temp[i * 3 + 2] = static_cast<unsigned char>(buff[i].z * 255);
    }*/
    ofs.write(reinterpret_cast<char*>(&frameBuffer[0]), imageWidth * imageWidth*3);

    if (ofs.fail()) {
        cerr << "Could not write data" << endl;
        return false;
    }

    ofs.close();
     

    //delete [] frameBuffer; 
    delete [] depthBuffer; 
 
    return 0; 
} 
 