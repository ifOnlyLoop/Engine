#pragma once

#include <fstream>
#include <algorithm>
#include <math.h>
#include "../../scripts/transform/gvec.hpp"

static const float inchToMm = 25.4; 

// TODO: FACTOR IT to head and source
class Camera
{
private:

    // Canvas Boarder and Depth
    float top,bottom,left,right,near,far;
    
    float   imageAspectRatio,
            filmApertureWidth,
            filmApertureHeight,
            filmAspectRatio;
     
    // Screen Pixels
    uint32_t imageWidth,imageHeight;
    // OPENGL PERSPECTIVE PROJECTION MATRIX  
    gmat<float> ProjectionMatrix;

public: 

    //Camera(gmat<float> w2c);
    Camera();
   ~Camera(){};


    void setImage(uint32_t W,uint32_t H);
    void setFilm (float W,float H);
    /** 
     * CAMERA SPECs
     * compute screen coordinates based 
     * on physically-based camera model 
     * 
     *      void setScreen (const bool isFill);
     */    

    /** 
     * SCREEN SPECs
     * Compute vertex raster screen coordinates
     */    
    void getRaster ( 
        const gmat<float> &worldToCamera,
        const gvec<float> &pWorld, 
        gvec<float> &pRaster
    );

    void gluPerspective( 
        const float &angleOfView, 
        const float &n, 
        const float &f
    );

    void glFrustum ();
};

/* 
    TODO: if camera on motion you need 
    to input world matrix to get the 
    inverse for every frame
*/

Camera::Camera()
{
    ProjectionMatrix.resize(4,4);  
}
/** 
 * CAMERA SPECs
 * compute screen coordinates based 
 * on physically-based camera model 
 */

void Camera::setFilm(float W,float H)
{
    filmApertureWidth=W;
    filmApertureHeight=H;
    filmAspectRatio=W/H;
}
void Camera::setImage(uint32_t W,uint32_t H)
{
    imageWidth=W;
    imageHeight=H;
    imageAspectRatio=W/(float)H; // deviceAspectRatio
}

/** 
 * SCREEN SPECs
 * Compute vertex raster screen coordinates
 */        
void Camera::getRaster ( 
    const gmat<float> &worldToCamera,
    const gvec<float> &pWorld, 
    gvec<float> &projectedVert
) { 
    projectedVert=pWorld*worldToCamera*ProjectionMatrix; // [-1,+1]
    //  FOR TRANSFORMATION MATRIX ONLY 
    if(projectedVert[3]!=1)
    for (int i=0;i<4;i++)
    projectedVert[i]/=projectedVert[3];
    
    projectedVert[0] = std::min(imageWidth  - 1, (uint32_t)(     (projectedVert[0] + 1) * 0.5  * imageWidth ));
    projectedVert[1] = std::min(imageHeight - 1, (uint32_t)((1 - (projectedVert[1] + 1) * 0.5) * imageHeight));      
    projectedVert[2] = 1 / projectedVert[2];
} 

// set OpenGL limits matrix
void Camera::gluPerspective ( 
    const float &angleOfView, 
    const float &n, 
    const float &f
) { 
    float scale = std::tan(angleOfView * 0.5 * M_PI / 180) * n; 
    near=n;
    far=f;    
    right = imageAspectRatio * scale;
    left = -right; 
    top = scale; 
    bottom = -top;
} 

// set OpenGL perspective projection matrix
void Camera::glFrustum () 
{ 
    ProjectionMatrix(0,0) = 2 * near / (right - left); 
    ProjectionMatrix(1,1) = 2 * near / (top - bottom); 
    ProjectionMatrix(2,0) = (right + left) / (right - left); 
    ProjectionMatrix(2,1) = (top + bottom) / (top - bottom); 
    ProjectionMatrix(2,2) =-(far + near) / (far - near); 
    ProjectionMatrix(2,3) =-1; 
    ProjectionMatrix(3,2) =-2 * far * near / (far - near); 
} 





/*void Camera::setScreen (const bool isFill=0)
{
    // Scale to Fit the Movie into Screen 
    float xscale = 1; 
    float yscale = 1; 
    float F2D    = filmAspectRatio/imageAspectRatio; 
    
    isFill ?
        (F2D>1)? xscale/=F2D : yscale*=F2D: // FILL SCREEN
        (F2D>1)? yscale*=F2D : xscale/=F2D; // OVER SCAN

    // Final Boundary Limits
    right *= xscale; 
    top   *= yscale; 
    bottom = -top; 
    left   = -right;     
}*/