#pragma once

#include "../scripts/transform/gvec.hpp"

class Vertex
{
public:

	Vertex();
	Vertex(float,float,float);
   ~Vertex();

	float& operator [] (int i)
    {
		return coord[i];
	}

	gvec<float> normal,coord,texture;
	
	int NORMAL_INDEX;
	int LOCATION_INDEX;
	
	void pushCoord(float,float,float,float);
	void pushNormal(float,float,float,float);
	void pushTexture(float,float);
private:
	//std::vector<vect> vertex;
};

Vertex::Vertex(float x,float y,float z)
{
	coord.resize(1,4);
	texture.resize(1,2);
	normal.resize(1,4);
	coord[0]=x;
	coord[1]=y;
	coord[2]=z;
	coord[3]=1;
}

Vertex::Vertex()
{
	coord.resize(1,4);
	texture.resize(1,2);
	normal.resize(1,4);
}

Vertex::~Vertex()
{
}

void Vertex::pushCoord(float x,float y,float z, float w=1)
{
 	coord[0]=x;
	coord[1]=y;
	coord[2]=z;
	coord[3]=w;
}
void Vertex::pushNormal(float x,float y,float z, float w=0)
{
 	normal[0]=x;
	normal[1]=y;
	normal[2]=z;
	normal[3]=w;
}

// right now it's just pushing color
void Vertex::pushTexture(float u, float v)
{
 	texture[0]=u;
	texture[1]=v;
}