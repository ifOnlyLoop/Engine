#pragma once 

#include <cstdint>
#include <memory>
#include <iostream>
#include <fstream>
#include <math.h>

#include "Object.hpp"
//#include "geometry.h"


//extern TriMesh* loadPolyMeshFromFile(std::string file);

extern bool rayTriangleIntersect (
    const Vec3f &orig, // primary array 
    const Vec3f &dir,  // primary array 
    const Vec3f &v0,   // tri vertex
    const Vec3f &v1,   // tri vertex
    const Vec3f &v2,   // tri vertex
    float &t, // length to intersection
    float &u, // barycentric + texturing
    float &v  // barycentric + texturing
);


class TriMesh : public Object
{
public:
    // Build a triangle mesh from a face index array and a vertex index array
    TriMesh (
        const uint32_t nfaces,
        const std::unique_ptr<uint32_t []> &faceIndex,
        const std::unique_ptr<uint32_t []> &vertsIndex,
        const std::unique_ptr<Vec3f []> &verts,
        std::unique_ptr<Vec3f []> &normals,
        std::unique_ptr<Vec2f []> &st
    );

    // Test if the ray interesests this triangle mesh
    bool intersect (
        const Vec3f &orig,  // primary ray   
        const Vec3f &dir,   // primary ray  
        float &tNear,       // depth buffer
        uint32_t &triIndex, // tri vertex
        Vec2f &uv           // texturing 
    )   const;
    
    // get color/texture
    void getSurfaceProperties (
        const Vec3f &hitPoint,
        const Vec3f &viewDirection,
        const uint32_t &triIndex,
        const Vec2f &uv,
        Vec3f &hitNormal,
        Vec2f &hitTextureCoordinates
    ) const;

    // member variables
    uint32_t numTris;                         // number of triangles
    std::unique_ptr<Vec3f []> P;              // triangles vertex position
    std::unique_ptr<uint32_t []> trisIndex;   // vertex index array
    std::unique_ptr<Vec3f []> N;              // triangles vertex normals
    std::unique_ptr<Vec2f []> texCoordinates; // triangles texture coordinates
};


TriMesh::TriMesh (
        const uint32_t nfaces,
        const std::unique_ptr<uint32_t []> &faceIndex,
        const std::unique_ptr<uint32_t []> &vertsIndex,
        const std::unique_ptr<Vec3f []> &verts,
        std::unique_ptr<Vec3f []> &normals,
        std::unique_ptr<Vec2f []> &st
    ) : numTris(0) {

        uint32_t k = 0, maxVertIndex = 0;
        // find out how many triangles we need to create for this mesh
        for (uint32_t i = 0; i < nfaces; ++i) {
            numTris += faceIndex[i] - 2;
            for (uint32_t j = 0; j < faceIndex[i]; ++j)
                if (vertsIndex[k + j] > maxVertIndex)
                    maxVertIndex = vertsIndex[k + j];
            k += faceIndex[i];
        }
        maxVertIndex += 1;
        
        // allocate memory to store the position of the mesh vertices
        P = std::unique_ptr<Vec3f []>(new Vec3f[maxVertIndex]);
        for (uint32_t i = 0; i < maxVertIndex; ++i) {
            P[i] = verts[i];
        }
        
        // allocate memory to store triangle indices
        trisIndex = std::unique_ptr<uint32_t []>(new uint32_t [numTris * 3]);
        uint32_t l = 0;
        // [comment]
        // Generate the triangle index array
        // Keep in mind that there is generally 1 vertex attribute for each vertex of each face.
        // So for example if you have 2 quads, you only have 6 vertices but you have 2 * 4
        // vertex attributes (that is 8 normals, 8 texture coordinates, etc.). So the easiest
        // lazziest method in our triangle mesh, is to create a new array for each supported
        // vertex attribute (st, normals, etc.) whose size is equal to the number of triangles
        // multiplied by 3, and then set the value of the vertex attribute at each vertex
        // of each triangle using the input array (normals[], st[], etc.)
        // [/comment]
        N = std::unique_ptr<Vec3f []>(new Vec3f[numTris * 3]);
        texCoordinates = std::unique_ptr<Vec2f []>(new Vec2f[numTris * 3]);
        for (uint32_t i = 0, k = 0; i < nfaces; ++i) { // for each  face
            for (uint32_t j = 0; j < faceIndex[i] - 2; ++j) { // for each triangle in the face
                trisIndex[l] = vertsIndex[k];
                trisIndex[l + 1] = vertsIndex[k + j + 1];
                trisIndex[l + 2] = vertsIndex[k + j + 2];
                N[l] = normals[k];
                N[l + 1] = normals[k + j + 1];
                N[l + 2] = normals[k + j + 2];
                texCoordinates[l] = st[k];
                texCoordinates[l + 1] = st[k + j + 1];
                texCoordinates[l + 2] = st[k + j + 2];
                l += 3;
            }                                                                                                                                                                                                                                
            k += faceIndex[i];
        }
    }


bool TriMesh::intersect (
        const Vec3f &orig,  // primary ray   
        const Vec3f &dir,   // primary ray  
        float &tNear,       // depth buffer
        uint32_t &triIndex, // tri vertex
        Vec2f &uv           // texturing 
    ) const {
        uint32_t j = 0;
        bool isect = false;
        for (uint32_t i = 0; i < numTris; ++i) 
        {   
            // get tri vertecies
            const Vec3f &v0 = P[trisIndex[j]];
            const Vec3f &v1 = P[trisIndex[j + 1]];
            const Vec3f &v2 = P[trisIndex[j + 2]];
            
            float t = kInfinity, u, v;
            // test intersection
            if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v) && t < tNear) 
            { // if found intersection is closer to pixel than the last one 
                
                tNear = t;
                uv.x  = u;
                uv.y  = v;

                triIndex = i;
                isect = true;
            }
            // each 3 vertecies are a tri skip by 3 each time                                                                                                                                                                                                                                
            j += 3;
        }
        return isect;
    }


void TriMesh::getSurfaceProperties (
        const Vec3f &hitPoint,
        const Vec3f &viewDirection,
        const uint32_t &triIndex,
        const Vec2f &uv,
        Vec3f &hitNormal,
        Vec2f &hitTextureCoordinates
) const {
    
    // face normal
    const Vec3f &v0 = P[trisIndex[triIndex * 3]];
    const Vec3f &v1 = P[trisIndex[triIndex * 3 + 1]];
    const Vec3f &v2 = P[trisIndex[triIndex * 3 + 2]];
    hitNormal = (v1 - v0).crossProduct(v2 - v0);
    hitNormal.normalize();
    
    // texture coordinates
    const Vec2f &st0 = texCoordinates[triIndex * 3];
    const Vec2f &st1 = texCoordinates[triIndex * 3 + 1];
    const Vec2f &st2 = texCoordinates[triIndex * 3 + 2];
    hitTextureCoordinates = (1 - uv.x - uv.y) * st0 + uv.x * st1 + uv.y * st2;

    // vertex normal
    /*
    const Vec3f &n0 = N[triIndex * 3];
    const Vec3f &n1 = N[triIndex * 3 + 1];
    const Vec3f &n2 = N[triIndex * 3 + 2];
    hitNormal = (1 - uv.x - uv.y) * n0 + uv.x * n1 + uv.y * n2;
    */
}


// NON MEMBER FUNCTION


extern bool rayTriangleIntersect (
    const Vec3f &orig, // primary array 
    const Vec3f &dir,  // primary array 
    const Vec3f &v0,   // tri vertex
    const Vec3f &v1,   // tri vertex
    const Vec3f &v2,   // tri vertex
    float &t, // length to intersection
    float &u, // barycentric + texturing
    float &v  // barycentric + texturing
) {
    // get tri coplanar edges
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    // get normal between ray and edge and .. 
    Vec3f pvec = dir.crossProduct(v0v2);
    // .. compare it to tri normal if they are close then ..
    float det = v0v1.dotProduct(pvec);
    // .. ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;

    // to use in cramer rule
    float invDet = 1 / det;

    // get u,v if one is (<0) or (1<sum) ..
    // then point isn't on triangle
    Vec3f tvec = orig - v0;
    u = tvec.dotProduct(pvec) * invDet;
    if (u < 0 || u > 1) return false;

    Vec3f qvec = tvec.crossProduct(v0v1);
    v = dir.dotProduct(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    // get t
    t = v0v2.dotProduct(qvec) * invDet;
    
    return true;
}


extern TriMesh* loadPolyMeshFromFile(std::string/*const char **/file)
{
    std::ifstream ifs;
    ifs.open(file);
    try 
    {
        ifs.open(file); 
            //if (ifs.fail()) 
            //    throw;
        
        std::stringstream ss;
            ss << ifs.rdbuf();
        uint32_t numFaces;
            ss >> numFaces;

        std::unique_ptr<uint32_t []> faceIndex(new uint32_t[numFaces]);
        uint32_t vertsIndexArraySize = 0;
        
        // reading face index array
        for (uint32_t i = 0; i < numFaces; ++i) {
            ss >> faceIndex[i];
            vertsIndexArraySize += faceIndex[i];
        }
        std::unique_ptr<uint32_t []> vertsIndex(new uint32_t[vertsIndexArraySize]);
        uint32_t vertsArraySize = 0;
        
        // reading vertex index array
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
            ss >> vertsIndex[i];
            if (vertsIndex[i] > vertsArraySize) vertsArraySize = vertsIndex[i];
        }
        vertsArraySize += 1;
        
        // reading vertices
        std::unique_ptr<Vec3f []> verts(new Vec3f[vertsArraySize]);
        for (uint32_t i = 0; i < vertsArraySize; ++i) {
            ss >> verts[i].x >> verts[i].y >> verts[i].z;
        }
        
        // reading normals
        std::unique_ptr<Vec3f []> normals(new Vec3f[vertsIndexArraySize]);
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
            ss >> normals[i].x >> normals[i].y >> normals[i].z;
        }
        
        // reading st coordinates
        std::unique_ptr<Vec2f []> st(new Vec2f[vertsIndexArraySize]);
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
            ss >> st[i].x >> st[i].y;
        }
        
        return new TriMesh(numFaces, faceIndex, vertsIndex, verts, normals, st);
    }
    catch (...) {
        ifs.close();
    }
    ifs.close();
    
    return nullptr;
}

