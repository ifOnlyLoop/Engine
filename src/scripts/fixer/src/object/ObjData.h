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
	std::vector<float> 
	tempNormalList,
	tempTextureList;
	// Mesh List of Faces
	std::vector<Face> faceList;
	// Temp Data to Save Face list of 
	// vertex indeces for processing 
	std::vector<int> tempFaceVertexData;

	void link (int Vidx, int tidx, int Nidx);
	void pushTempNormal(float fx,float fy, float fz);
	void pushTempTexture(float fx,float fy);
	void clearTempData();

private:

};

ObjData::ObjData()
{
}

ObjData::~ObjData()
{
}

void ObjData::link (int Vidx, int tidx, int Nidx)
{
	vertexList[Vidx].pushNormal (
		tempNormalList[Nidx+0],
		tempNormalList[Nidx+1],
		tempNormalList[Nidx+2]
	);
	vertexList[Vidx].pushTexture (
		tempTextureList[tidx+0],
		tempTextureList[tidx+1]
	);
}

void ObjData::pushTempNormal(float fx,float fy, float fz)
{
	tempNormalList.push_back(fx);
	tempNormalList.push_back(fy);
	tempNormalList.push_back(fz);
}

void ObjData::pushTempTexture(float fx,float fy)
{
	tempTextureList.push_back(fx);
	tempTextureList.push_back(fy);
	//tempNormalList.push_back(fz);
}
void ObjData::clearTempData()
{
	tempNormalList.clear();
	tempTextureList.clear();
}

//void ObjData::pushTempTexture(float fx,float fy, float fz=0.0f)