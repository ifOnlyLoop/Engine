#pragma once

#include <vector>
#include "../../../transform/gvec.hpp"
#include "Vertex.h"
#include "Face.h"

class ObjData
{
public:
	ObjData();
   ~ObjData();

	bool isSub;

	// Mesh List of Verteces
	std::vector<Vertex> vertexList;
	std::vector<float> tempNormalList;
	// Mesh List of Faces
	std::vector<Face> faceList;
	// Temp Data to Save Face list of 
	// vertex indeces for processing 
	std::vector<int> tempFaceVertexData;

	void link (int Vidx, int Nidx);
	void pushTempNormal(float fx,float fy, float fz);
	void clearTempNormal();

private:

};

ObjData::ObjData()
{
}

ObjData::~ObjData()
{
}

void ObjData::link (int Vidx, int Nidx)
{
	vertexList[Vidx].pushNormal (
		tempNormalList[Nidx+0],
		tempNormalList[Nidx+1],
		tempNormalList[Nidx+2]
	);
}

void ObjData::pushTempNormal(float fx,float fy, float fz)
{
	tempNormalList.push_back(fx);
	tempNormalList.push_back(fy);
	tempNormalList.push_back(fz);
}

void ObjData::clearTempNormal()
{
	tempNormalList.clear();
}