#pragma once

#include <fstream>
#include "../scripts/transform/gvec.hpp"

static const float inchToMm = 25.4; 
enum FitResolutionGate { kFill = 0, kOverscan }; 

class Camera
{
private:

    // Canvas Boarder and Depth
    float top,bottom,left,right,near;
    // Screen Pixels
    float imageWidth,imageHeight;
    //  From World To Camera Matrix (worldToCamera)
    gmat<float> W2C;
    // Points of the World from Camera Perspective
    gvec<float> pCam;
    // Point Position After Scale on the Screen
    gvec<float> pScr;      
    // normalized Position  [0,1] on the Screen
    gvec<float> pNDC;    
        
public: 

    Camera(gmat<float> w2c);
    Camera(){};
   ~Camera(){};

    /** 
     * CAMERA SPECs
     * compute screen coordinates based 
     * on physically-based camera model 
     */    
    void setScreen ( 
        const float &filmApertureWidth, 
        const float &filmApertureHeight, 
        const uint32_t &ImageWidth, 
        const uint32_t &ImageHeight, 
        const FitResolutionGate &fitFilm, 
        const float &nearClippingPLane, 
        const float &focalLength 
    );
    /** 
     * SCREEN SPECs
     * Compute vertex raster screen coordinates
     */    
    void toRaster ( 
        const gvec<float> &pWorld, 
        gvec<float> &pRaster 
    );
};

/* 
    TODO: if camera on motion you need 
    to input world matrix to get the 
    inverse for every frame
*/

Camera::Camera(gmat<float> w2c)
{
    W2C=w2c;
    pScr.resize(2);
    pNDC.resize(2);  
}
/** 
 * CAMERA SPECs
 * compute screen coordinates based 
 * on physically-based camera model 
 */
void Camera::setScreen ( 
    const float &filmApertureWidth, 
    const float &filmApertureHeight, 
    const uint32_t &ImageWidth, 
    const uint32_t &ImageHeight, 
    const FitResolutionGate &fitFilm, 
    const float &nearClippingPLane, 
    const float &focalLength 
){
    imageWidth  = ImageWidth;
    imageHeight = ImageHeight;
    // Original Film Ratio When The Movie was Taken
    float filmAspectRatio   = filmApertureWidth / filmApertureHeight; 
    // Defice Screen Ratio Where The Movie is Displayed
    float deviceAspectRatio = imageWidth / (float)imageHeight; 
    // Get Canvas Boarder Limits
    top   = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPLane; 
    right = ((filmApertureWidth  * inchToMm / 2) / focalLength) * nearClippingPLane; 
    near  =   nearClippingPLane;
    // Scale to Fit the Movie into Screen 
    float xscale = 1; 
    float yscale = 1; 
    
    // Set the Scale TODO: git rid of the switch
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
    // Final Boundary Limits
    right *= xscale; 
    top   *= yscale; 
    bottom = -top; 
    left   = -right; 
} 

/** 
 * SCREEN SPECs
 * Compute vertex raster screen coordinates
 */        
void Camera::toRaster ( 
    const gvec<float> &pWorld, 
    gvec<float> &pRaster 
) { 
    //gmat<float> w2c(W2C);
    pCam=pWorld*W2C;
    // convert to screen space
    pScr[0] = near * pCam[0] / -pCam[2]; 
    pScr[1] = near * pCam[1] / -pCam[2]; 
 
    // now convert point from screen space to NDC space (in range [-1,1])
    
    pNDC[0] = 2 * pScr[0] / (right - left) - (right + left) / (right - left); 
    pNDC[1] = 2 * pScr[1] / (top - bottom) - (top + bottom) / (top - bottom); 
 
    // convert to raster space
    pRaster[0] = (pNDC[0] + 1) / 2 * imageWidth; 
    // in raster space y is down so invert direction
    pRaster[1] = (1 - pNDC[1]) / 2 * imageHeight; 
    pRaster[2] = -pCam[2]; 
} 