#pragma once

// STL
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
// UD
#include"Vertex.h"
#include"Face.h"
#include"ObjData.h"

class handleObject
{
private:
	// PRIVATE MEMBER DATATYPES //

	std::string
		// Path of .obj Import File
		IMPORT_PATH,
		// Path of .obj Export File
		EXPORT_PATH,
		// Geometric Element
		ELEMENT;
	char * 
		// c String Version
		ELEMENT_C_STRING;

	
	/*
	 * START OF Temp DATA 
	 */
	
	Vertex  // Vertex Dummy Info for String Streaming
		tempVertex;
	Face
		tempFace;
	std::string  // Vertex Dummy Position for String Streaming
		x, y, z, index, vertexIndex, normalIndex, textureIndex;
	std::string//char   // .obj Geometric Data Type for String Streaming
		dataType;

	/*
	 * END OF DUMMY DATA
	 */
	
	// Set the .obj Import File Reader
	std::ifstream obj;
	// Set Geometric Element Data Reader
	std::stringstream info;
	// Set the .obj Export File
	std::ofstream objEXPORT;
	
	// PRIVATE MEMBER FUNCTIONS //

	// Read From .obj Data
	void read();
	// Write to .obj Data
	void write();
	// Handle Vertex
	void handleVertex(bool);
	// Handle Face
	void handleFace();
	// split face's normal and vertex index
	void objElementSplit(std::string&);

public:
	// Constrtuctor //
	 handleObject();
	~handleObject();
	 handleObject(std::string);

	// PUBLIC MEMBER FUNCTIONS //
	void IMPORT(std::string);
	void EXPORT(std::string);
	void ROTATE(float, float, float);
	// PUBLIC MEMBER DATATYPES //
	std::vector<Vertex> vertexList;
	//std::vector<gvec<float>> tempNormalList;
	std::vector<Face> faceList;
	// .obj File Extracted Data
	ObjData objData;
	// Traingulation Parameter
	float u, v;

	float xmin,
		  xmax,
		  ymin,
		  ymax,
		  zmin,
		  zmax;
	
};



handleObject::handleObject()
{
}

handleObject::handleObject(std::string filePath)
{
	xmin=INT32_MAX,
	xmax=INT32_MIN,
	ymin=INT32_MAX,
	ymax=INT32_MIN,
	zmin=INT32_MAX,
	zmax=INT32_MIN;
    
	ELEMENT = "";
    ELEMENT_C_STRING = NULL;
    x = y = z = 0.0f;
    u = 0.2f; 
    v = 0.7f;
    IMPORT(filePath);
}
handleObject::~handleObject()
{
}


// Read .obj File Data 
void handleObject::IMPORT(std::string filePath)
{
    // Set File Path
    IMPORT_PATH = filePath;
	// Open File
	obj.open(IMPORT_PATH);
	// Read Data
	read();
	// Close File
	obj.close();
}


// Store Data in the Suitasble c++ Format
void handleObject::read()
{
    while (!obj.eof())
    {
        // Get the First Element
        std::getline(obj, ELEMENT);
        // Split Vertex//Normal Index
        objElementSplit(ELEMENT);
        // Stream to String Buffer
        info.str(ELEMENT);
        // Get Element Type
        info >> dataType;
        // Proccess Based on Type
        if (dataType == "v")
            handleVertex(1);
        if (dataType == "vn")
            handleVertex(0);
        if (dataType == "f")
            handleFace();
        // Clear Buffer
        info.clear();
    }
	// all normals are linked 
	objData.clearTempNormal();
}

void handleObject::objElementSplit(std::string& s)
{
    for (char& c : s)
        if (c == '/') c = ' ';
}

void handleObject::handleVertex(bool isLocation=0)
{
    // Get Vertex Postion
    info >> x >> y >> z;
	float xx=stof(x), yy=stof(y), zz=stof(z);

	xmin=std::min(xmin,xx);	xmax=std::max(xmax,xx);
	ymin=std::min(ymin,yy);	ymax=std::max(ymax,yy);
	zmin=std::min(zmin,zz);	zmax=std::max(zmax,zz);

    // String to Float
	if(isLocation)
	{	// Push the Postion
		tempVertex.pushLocation(xx,yy,zz);
        objData.vertexList.push_back(tempVertex);
	}
    else
	{	// temp storage until normals are linked to vertex
		objData.pushTempNormal(xx,yy,zz);
	}
    // Clear Buffer for Next Reading
    info.clear();
}
//
void handleObject::handleFace()
{
    // Face tempFace;
    // EVEN: vertex ODD: normal
	int i = 0;
	int vidx, nidx, tidx;
    int vertexOrNormal = 0;
    // Read Face Vertex Index List
    while (info >> vertexIndex >> textureIndex >>normalIndex)
    {
		vidx=stoi(vertexIndex) -1;
		nidx=stoi(normalIndex) -1;
		tidx=stoi(textureIndex)-1;
		// Link point with its normal
		//objData.link(vidx,nidx);
		// add vertex index to face (tri face!)
		tempFace[i++]=vidx; 
    }
        
    // Process Face
    objData.faceList.push_back(tempFace);
    //FaceUtil().triangulation(objData.faceList.size() - 1, objData);
    // Clear temp Data for next Iteration
    //tempFace.clear();
}


void handleObject::EXPORT(std::string filePath)
{
    // Set File Path
    EXPORT_PATH = filePath;
    // Open File
    objEXPORT.open(EXPORT_PATH);
    // Write Data
    write();
    // Close File
    objEXPORT.close();
}


void handleObject::write()
{	// points
    for (auto p : objData.vertexList)
    {
        objEXPORT << "v " << p.location[0] << " " << p.location[1] << " " << p.location[2] << std::endl;
    }
	// normals
    for (auto n : objData.vertexList)
    {
        //objEXPORT << "vn\t"<< n.normal[0] << "\t" << n.normal[1] << "\t" << n.normal[2] << std::endl;
    }
	// face
    for (auto f : objData.faceList)
    {
		objEXPORT << "f "  << f[0]+1 /*<< "//" << f[0]+1*/ << " "
				  		   << f[1]+1 /*<< "//" << f[1]+1*/ << " "
				           << f[2]+1 /*<< "//" << f[2]+1*/ << std::endl;
				  
    }
}



void handleObject::ROTATE(float h, float p, float b)
{
    /*quat rx, ry, rz;
        rx.setToRotateAboutX(h);
        ry.setToRotateAboutY(p);
        rz.setToRotateAboutZ(b);
    quat r(rx * ry * rz);

    for (vect& v : objData.vertexList)
        r* v;
    
    //for (vect& v : Data.normalList)
      //  r* v;*/
}



