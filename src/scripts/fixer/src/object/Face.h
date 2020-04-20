#pragma once

#include "../../../transform/gvec.hpp"

class Vertex;

class Face
{
public:
	Face();
   ~Face();
	
	int& operator [] (int i)
    {
		return thisFaceVertex[i];
	}

	gvec<int> thisFaceVertex;	
	gvec<int> thisFaceNormal;	

	void pushTri(int,int,int);
	void pushNor(int,int,int);

private:
	
};


Face::Face()
{
	thisFaceVertex.resize(3);
}

Face::~Face()
{
}

void Face::pushTri(int v0,int v1,int v2)
{
	thisFaceVertex[0]=v0;
	thisFaceVertex[1]=v1;
	thisFaceVertex[2]=v2;
}

void Face::pushNor(int n0,int n1,int n2)
{
	thisFaceNormal[0]=n0;
	thisFaceNormal[1]=n1;
	thisFaceNormal[2]=n2;
}