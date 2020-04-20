#pragma once
#include "../../../transform/gvec.hpp"

class Vertex
{
public:

	Vertex();
   ~Vertex();

	float& operator [] (int i)
    {
		return location[i];
	}

	gvec<float> normal,location;
	
	int NORMAL_INDEX;
	int LOCATION_INDEX;
	
	void pushLocation(float,float,float,float);
	void pushNormal(float,float,float,float);
private:
	//std::vector<vect> vertex;
};

Vertex::Vertex()
{
	location.resize(1,4);
	normal.resize(1,4);
}

Vertex::~Vertex()
{
}

void Vertex::pushLocation(float x,float y,float z, float w=1)
{
 	location[0]=x;
	location[1]=y;
	location[2]=z;
	location[3]=w;
}
void Vertex::pushNormal(float x,float y,float z, float w=0)
{
 	normal[0]=x;
	normal[1]=y;
	normal[2]=z;
	normal[3]=w;
}