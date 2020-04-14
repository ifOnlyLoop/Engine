//#include "gmat.hpp"
#include "/home/oo/iOL/Engine/src/scripts/transform/gvec.hpp"
//#include "/home/oo/iOL/Engine/src/scripts/transform/gmat.hpp"

#include <bits/stdc++.h>


using namespace std;
using namespace chrono;

void computePixelCoordinates ( 
    
    const gvec<float> pWorld, 
    gvec<int> &pRaster, 
    const gmat<float> &worldToCamera, 
    
    const float &canvasWidth, 
    const float &canvasHeight, 
    const uint32_t &imageWidth, 
    const uint32_t &imageHeight

) { 
    gvec<float> pCamera (pWorld*worldToCamera);
    for(int i=0;i<3;i++)pCamera[i]/=pCamera[3];

    gvec<float> pScreen(2); 
    pScreen[0] = pCamera[0] / -pCamera[2]; 
    pScreen[1] = pCamera[1] / -pCamera[2]; 
    gvec<float> pNDC(2); 
    pNDC[0] = (pScreen[0] + canvasWidth * 0.5) / canvasWidth; 
    pNDC[1] = (pScreen[1] + canvasHeight * 0.5) / canvasHeight; 
    pRaster[0] = (int)(pNDC[0] * imageWidth); 
    pRaster[1] = (int)((1 - pNDC[1]) * imageHeight); 
} 


int main()
{


        freopen("wireTree.in", "r", stdin);


    std::ofstream ofs; 

    ofs.open("./wireTree.svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" height=\"512\" width=\"512\">" << std::endl;     
    
    int n; 
    
    gmat<float> cameraToWorld(4,4);
    
    scanf("%d\n",&n);
    for (int i = 0; i < n; i++)
    {
        scanf("%f\n",&cameraToWorld[i]);
    }
    gmat<float> worldToCamera(cameraToWorld.inverse());
    
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
    
    //std::cerr << worldToCamera << std::endl; 
    float    canvasWidth = 2,   canvasHeight = 2; 
    uint32_t imageWidth  = 512, imageHeight  = 512; 
    
    for (uint32_t i = 0; i < n; ++i) 
    { 
        // extract points from triangle
        const gvec<float> &v0World = verts[tris[i][0]]; 
        const gvec<float> &v1World = verts[tris[i][1]]; 
        const gvec<float> &v2World = verts[tris[i][2]]; 
        // get pixel coord
        gvec<int>v0Raster(2), v1Raster(2), v2Raster(2); 
        // Compute Pixel Coordinates
        computePixelCoordinates(v0World, v0Raster, worldToCamera, canvasWidth, canvasHeight, imageWidth, imageHeight); 
        computePixelCoordinates(v1World, v1Raster, worldToCamera, canvasWidth, canvasHeight, imageWidth, imageHeight); 
        computePixelCoordinates(v2World, v2Raster, worldToCamera, canvasWidth, canvasHeight, imageWidth, imageHeight); 
    
        //std::cerr << v0Raster << ", " << v1Raster << ", " << v2Raster << std::endl; 
        ofs << "<line x1=\"" << v0Raster[0] << "\" y1=\"" << v0Raster[1] << "\" x2=\"" << v1Raster[0] << "\" y2=\"" << v1Raster[1] << "\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n"; 
        ofs << "<line x1=\"" << v1Raster[0] << "\" y1=\"" << v1Raster[1] << "\" x2=\"" << v2Raster[0] << "\" y2=\"" << v2Raster[1] << "\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n"; 
        ofs << "<line x1=\"" << v2Raster[0] << "\" y1=\"" << v2Raster[1] << "\" x2=\"" << v0Raster[0] << "\" y2=\"" << v0Raster[1] << "\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n"; 
    } 
    ofs << "</svg>\n"; 
    ofs.close();
    return 0;
}