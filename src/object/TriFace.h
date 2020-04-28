#pragma once

#include "../scripts/transform/gvec.hpp"

class Vertex;

class TriFace
{
public:
	TriFace(int,int,int);
   ~TriFace();
    
	int operator [] (int i)
    {
		return vertexList[i];
	}
	
	int v0, v1, v2;
	void pushTri(int,int,int);

private:
	int vertexList[3];
};


TriFace::TriFace(int vv0,int vv1,int vv2)
{
	v0=vv0;
    v1=vv1;
    v2=vv2;
}

TriFace::~TriFace()
{
}

void TriFace::pushTri(int vv0,int vv1,int vv2)
{
	v0=vv0;//vertexList[0]
	v1=vv1;//vertexList[1]
	v2=vv2;//vertexList[2]
}