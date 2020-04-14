//#include "gmat.hpp"
#include "src/scripts/transform/gvec.hpp"
//#include "src/scripts/transform/tmat.hpp"

#include <bits/stdc++.h>

struct engine
{
    char :0;
};

using namespace std;
using namespace chrono;

bool computePixelCoordinates( 
    const gvec<float> &pWorld, 
    const gmat<float> &worldToCamera, 
    const float &b, 
    const float &l, 
    const float &t, 
    const float &r, 
    const float &near, 
    const uint32_t &imageWidth, 
    const uint32_t &imageHeight, 
    gvec<int> &pRaster) 
{ 
    gvec<float> pCamera(pWorld*worldToCamera); 
    gvec<float> pScreen(2); 
    pScreen[0] = pCamera[0] / -pCamera[2] * near; 
    pScreen[1] = pCamera[1] / -pCamera[2] * near; 
 
    gvec<float> pNDC(2); 
    pNDC[0] = (pScreen[0] + r) / (2 * r); 
    pNDC[1] = (pScreen[1] + t) / (2 * t); 
    pRaster[0] = (int)(     pNDC[0]  * imageWidth); 
    pRaster[1] = (int)((1 - pNDC[1]) * imageHeight); 
 
    bool visible = true; 
    if (pScreen[0] < l || pScreen[0] > r || pScreen[1] < b || pScreen[1] > t) 
        visible = false; 
 
    return visible; 
} 

float focalLength = 35; // in mm 
// 35mm Full Aperture in inches
float filmApertureWidth = 0.825; 
float filmApertureHeight = 0.446; 
static const float inchToMm = 25.4; 
float nearClippingPlane = 0.1; 
float farClipingPlane = 1000; 
// image resolution in pixels
uint32_t imageWidth = 512; 
uint32_t imageHeight = 512; 
 
enum FitResolutionGate { kFill = 0, kOverscan }; 
     FitResolutionGate fitFilm = kOverscan; 

int main()
{


    #ifndef ONLINE_JUDGE
        freopen("/home/oo/iOL/Engine/src/examples/wireBoat/wireBoat.in", "r", stdin);
        //freopen("res.svg", "w", stdout);
    #endif


    float filmAspectRatio = filmApertureWidth / filmApertureHeight; 
    float deviceAspectRatio = imageWidth / (float)imageHeight; 
 
    float top = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPlane; 
    float right = ((filmApertureWidth * inchToMm / 2) / focalLength) * nearClippingPlane; 
 
    float xscale = 1; 
    float yscale = 1; 
 
    switch (fitFilm) { 
        default: 
        case kFill: 
            if (filmAspectRatio > deviceAspectRatio) { 
                xscale = deviceAspectRatio / filmAspectRatio; 
            } 
            else { 
                yscale = filmAspectRatio / deviceAspectRatio; 
            } 
            break; 
        case kOverscan: 
            if (filmAspectRatio > deviceAspectRatio) { 
                yscale = filmAspectRatio / deviceAspectRatio; 
            } 
            else { 
                xscale = deviceAspectRatio / filmAspectRatio; 
            } 
            break; 
    } 
 
    right *= xscale; 
    top *= yscale; 
 
    float bottom = -top; 
    float left = -right; 
 

    
    int n; 
    
    scanf("%d\n",&n);
    vector<gvec<float>> 
        verts(n,gvec<float>(4,1));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            scanf("%f\n",&verts[i][j]);
        }
        verts[i][3]=1;
    }

    scanf("%d\n",&n);
    vector<gvec<int>> 
        tris(n,gvec<int>(3,1));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            scanf("%d\n",&tris[i][j]);
        }
    }
    
    gmat<float> cameraToWorld(4,4);
    
    scanf("%d\n",&n);
    for (int i = 0; i < n; i++)
    {
        scanf("%f\n",&cameraToWorld[i]);
    }
    gmat<float> worldToCamera(cameraToWorld.inverse());

    //std::cerr << worldToCamera << std::endl; 
    float    canvasWidth = 2,   canvasHeight = 2; 
    uint32_t imageWidth  = 512, imageHeight  = 512; 

    
    std::ofstream ofs; 

    ofs.open("./testRes.svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" height=\"512\" width=\"512\">" << std::endl;     
    
    for (uint32_t i = 0; i < n; ++i) 
    { 
        // extract points from triangle
        const gvec<float> &v0World = verts[tris[i][0]]; 
        const gvec<float> &v1World = verts[tris[i][1]]; 
        const gvec<float> &v2World = verts[tris[i][2]]; 
        // get pixel coord
        gvec<int>v0Raster(2), v1Raster(2), v2Raster(2); 
        // Compute Pixel Coordinates
        bool visible = true; 
        visible &= computePixelCoordinates(v0World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v0Raster); 
        visible &= computePixelCoordinates(v1World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v1Raster); 
        visible &= computePixelCoordinates(v2World, worldToCamera, bottom, left, top, right, nearClippingPlane, imageWidth, imageHeight, v2Raster); 
 
        int val = visible ? 0 : 255; 
        ofs << "<line x1=\"" << v0Raster[0] << "\" y1=\"" << v0Raster[1] << "\" x2=\"" << v1Raster[0] << "\" y2=\"" << v1Raster[1] << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n"; 
        ofs << "<line x1=\"" << v1Raster[0] << "\" y1=\"" << v1Raster[1] << "\" x2=\"" << v2Raster[0] << "\" y2=\"" << v2Raster[1] << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n"; 
        ofs << "<line x1=\"" << v2Raster[0] << "\" y1=\"" << v2Raster[1] << "\" x2=\"" << v0Raster[0] << "\" y2=\"" << v0Raster[1] << "\" style=\"stroke:rgb(" << val << ",0,0);stroke-width:1\" />\n"; 
    } 
    ofs << "</svg>\n"; 
    ofs.close();
    return 0;
}