#pragma once

#include <cstdint>
#include <memory>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

#include "Object.h"
#include "Vertex.h"
#include "TriFace.h"

#include "../scripts/transform/gvec.hpp"
//#include "geometry.h"

extern bool rayTriangleIntersect (
    const gvec<float> &orig, // primary array 
    const gvec<float> &dir,  // primary array 
    const gvec<float> &v0,   // tri vertex
    const gvec<float> &v1,   // tri vertex
    const gvec<float> &v2,   // tri vertex
    float &t, // length to intersection
    float &u, // barycentric + texturing
    float &v  // barycentric + texturing
);

//struct buff{ unsigned char x = 0, y = 0, z = 0; };

class TriMesh : public Object
{
public:
    // Build a triangle mesh from a face index array and a vertex index array
    TriMesh ();

    //-------------------------------------------------------
    
    gvec<float> bmin,bmax;
    std::vector<Vertex> objVertexList;
    std::vector<float>
            unlinkedNormals,
            unlinkedTexture;

    std::vector<TriFace> objFaceList;

    //-------------------------------------------------------

    // Test if the ray interesests this triangle mesh
    bool intersect (
        const gvec<float> &orig,  // primary ray   
        const gvec<float> &dir,   // primary ray  
        float &tNear,             // depth buffer
        uint32_t &triIndex,       // tri vertex
        gvec<float> &uv           // texturing 
    )   const;

    // get color/texture
    void getSurfaceProperties (
        const gvec<float> &hitPoint,
        const gvec<float> &viewDirection,
        const uint32_t &triIndex,
        const gvec<float> &uv,
        gvec<float> &hitNormal,//3
        gvec<float> &hitTextureCoordinates//2
    ) const;

    //-------------------------------------------------------

    void link (
        int Vidx, // vertex  index
        int tidx, // texture index
        int Nidx  // normal  index
    );

	void pushNormal (
        float fx,
        float fy, 
        float fz
    );
	
    void pushTexture (
        float fx,
        float fy, 
        float fz
    );

	virtual void clearTempData();

    //-------------------------------------------------------
};


TriMesh::TriMesh () 
{
    bmin.resize(3);
    bmax.resize(3);
    bmin=INT32_MAX;
    bmax=INT32_MIN;
};

//-------------------------------------------------------

void TriMesh::link (int Vidx, int Tidx, int Nidx)
{
    
	objVertexList[Vidx].pushNormal (
		unlinkedNormals[3*Nidx+0],
		unlinkedNormals[3*Nidx+1],
		unlinkedNormals[3*Nidx+2]
	);
	objVertexList[Vidx].pushTexture (
		unlinkedTexture[2*Tidx+0],
		unlinkedTexture[2*Tidx+1]
	);
}

void TriMesh::pushNormal(float fx,float fy, float fz)
{
	unlinkedNormals.push_back(fx);
	unlinkedNormals.push_back(fy);
	unlinkedNormals.push_back(fz);
}

void TriMesh::pushTexture(float fx,float fy, float fz=0)
{
	unlinkedTexture.push_back(fx);
	unlinkedTexture.push_back(fy);
}

void TriMesh::clearTempData()
{
	unlinkedNormals.clear();
	unlinkedTexture.clear();
}

//-------------------------------------------------------


bool TriMesh::intersect (
    const gvec<float> &orig,  // primary ray   
    const gvec<float> &dir,   // primary ray  
    float &tNear,             // depth buffer
    uint32_t &triIndex,       // tri vertex
    gvec<float> &uv           // texturing 
) const {
    bool isect = false;

    int numTris=objFaceList.size();
    for (uint32_t i = 0; i < numTris; ++i) 
    {
        // get tri vertecies
        const gvec<float> &v0 = objVertexList[objFaceList[i].v0].coord;//trisIndex[j]];
        const gvec<float> &v1 = objVertexList[objFaceList[i].v1].coord;//trisIndex[j + 1]];
        const gvec<float> &v2 = objVertexList[objFaceList[i].v2].coord;//trisIndex[j + 2]];
        
        float t = kInfinity, u, v;
        // test intersection
        if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v) && t < tNear) 
        { // if found intersection is closer to pixel than the last one 
            
            tNear = t;
            uv[0] = u;
            uv[1] = v;

            triIndex = i;
            isect = true;
        }
    }
    //std::cerr<<"scan done\n";//<<std::endl;
    return isect;
}


void TriMesh::getSurfaceProperties (
        const gvec<float> &hitPoint,
        const gvec<float> &viewDirection,
        const uint32_t &triIndex,
        const gvec<float> &uv,
        gvec<float> &hitNormal,//3
        gvec<float> &hitTextureCoordinates//2
) const {
    // vertex normal
    const gvec<float> &n0 = objVertexList[objFaceList[triIndex].v0].normal;
    const gvec<float> &n1 = objVertexList[objFaceList[triIndex].v1].normal;
    const gvec<float> &n2 = objVertexList[objFaceList[triIndex].v2].normal;
    hitNormal = n0*(1 - uv[0] - uv[1]) + n1*uv[0] + n2*uv[1];
        
    // texture coordinates
    const gvec<float> &st0 = objVertexList[objFaceList[triIndex].v0].texture;
    const gvec<float> &st1 = objVertexList[objFaceList[triIndex].v1].texture;
    const gvec<float> &st2 = objVertexList[objFaceList[triIndex].v2].texture;
    hitTextureCoordinates = st0*(1 - uv[0] - uv[1]) + st1*uv[0] + st2*uv[1] ;

    // face normal(in case)
    /*
    const Vec3f &v0 = P[trisIndex[triIndex * 3]];
    const Vec3f &v1 = P[trisIndex[triIndex * 3 + 1]];
    const Vec3f &v2 = P[trisIndex[triIndex * 3 + 2]];
    hitNormal = (v1 - v0).crossProduct(v2 - v0);
    hitNormal.normalize();
    */
}


// NON MEMBER FUNCTION


extern bool rayTriangleIntersect (
    const gvec<float> &orig, // primary array 
    const gvec<float> &dir,  // primary array 
    const gvec<float> &v0,   // tri vertex
    const gvec<float> &v1,   // tri vertex
    const gvec<float> &v2,   // tri vertex
    float &t, // length to intersection
    float &u, // barycentric + texturing
    float &v  // barycentric + texturing
) {
    // get tri coplanar edges
    gvec<float> v0v1 = v1 - v0;
    gvec<float> v0v2 = v2 - v0;
    // get normal between ray and edge and .. 
    gvec<float> pvec = dir.cross(v0v2);
    // .. compare it to tri normal if they are close then ..
    float det = v0v1*pvec;
    // .. ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;

    // to use in cramer rule
    float invDet = 1 / det;

    // get u,v if one is (<0) or (1<sum) ..
    // then point isn't on triangle
    gvec<float> tvec = orig - v0;
    u = (tvec*pvec) * invDet;
    if (u < 0 || u > 1) return false;

    gvec<float> qvec = tvec.cross(v0v1);
    v = (dir*qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    // get t
    t = (v0v2*qvec) * invDet;
    
    return true;
}