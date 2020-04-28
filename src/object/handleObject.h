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
#include"TriFace.h"
#include"TriMesh.h"

class handleObject
{
private:
	// PRIVATE MEMBER DATATYPES //

	std::string
		// Path of .obj Import File
		IMPORT_PATH,
		// Path of .obj Export File
		EXPORT_PATH,
		// Path of .bmp Texture File
		TEXTURE_PATH,
		// Geometric Element
		ELEMENT;
	
	// Vertex Dummy Position for String Streaming
	std::string  
		x, y, z, index, vertexIndex, normalIndex, textureIndex;
	
	// .obj Geometric Data Type for String Streaming
	std::string   
		dataType;


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
	//
	void handleTexture();
	// Handle Face
	void handleFace();
	// split face's normal and vertex index
	void objElementSplit(std::string&);

	// object to handle
	TriMesh* Obj;

public:
	// Constrtuctor //
 	handleObject();
   ~handleObject();
	handleObject(std::string);

	// PUBLIC MEMBER FUNCTIONS //
	void IMPORT(std::string, TriMesh*);
	void EXPORT(std::string, TriMesh&);
	void ROTATE(float, float, float);
};



handleObject::handleObject()
{
	ELEMENT = "";
    x = y = z = 0.0f;

}

handleObject::handleObject(std::string objFilePath)
{
	ELEMENT = "";
    x = y = z = 0.0f;
    //const_cast<char*>(txtFilePath.c_str()
}

handleObject::~handleObject()
{
}


// Read .obj File Data 
void handleObject::IMPORT(std::string filePath, TriMesh* object)
{
	// set object
	Obj = object;
    // Set File Path
    IMPORT_PATH = filePath;
	// Open File
	obj.open(IMPORT_PATH);
	// Read Data
	read();
	// Close File
	obj.close();
	//std::cerr<<(int)(Obj->objFaceList.size())<<' '<<(int)(Obj->objVertexList.size());
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
		if (dataType == "vt")
		    handleTexture();
        if (dataType == "f")
            handleFace();
        
		// Clear Buffer
        info.clear();
    }
	// all normals are linked 
	Obj->clearTempData();
}

void handleObject::objElementSplit(std::string& s)
{
    for (char& c : s)
        if (c == '/') c = ' ';
}

void handleObject::handleVertex(bool isCoord=0)
{	// vertexDataType 1:location 0:normal 2:texture
    
	// Get Vertex Postion (debug for type 2)
	info >> x >> y >> z;
	float xx=stof(x), yy=stof(y), zz=stof(z);

	if(isCoord)
	{	// get bounding box
		Obj->bmin[0]=std::min(Obj->bmin[0],xx);
		Obj->bmin[1]=std::min(Obj->bmin[1],yy);
		Obj->bmin[2]=std::min(Obj->bmin[2],zz);
		Obj->bmax[0]=std::max(Obj->bmax[0],xx);
		Obj->bmax[1]=std::max(Obj->bmax[1],yy);
		Obj->bmax[2]=std::max(Obj->bmax[2],zz);
	}

    // String to Float
	if(isCoord)
 		// Push the Postion
		Obj->objVertexList.push_back(Vertex(xx,yy,zz));
	else
		// temp storage until normals are linked to vertex
		Obj->pushNormal(xx,yy,zz);

    // Clear Buffer for Next Reading
    info.clear(); // maybe you should put it after read in import ? 
}

void handleObject::handleTexture()
{
	info >> x >> y;// >> z;
	float u=stof(x), v=stof(y);//, zz=stof(z);
	// Clear Buffer for Next Reading
    Obj->pushTexture(u,v);
	info.clear(); // maybe you should put it after read in import ? 
}

//

void handleObject::handleFace()
{
    int vidx, nidx, tidx;
    
	// Read Face Vertex Index List
	std::vector<int> FaceVertexList;
    while ( info 
		>> vertexIndex 
		>> textureIndex 
		>> normalIndex
	) {
		// one to zero based
		vidx=stoi(vertexIndex) -1;
		tidx=stoi(textureIndex)-1;
		nidx=stoi(normalIndex) -1;
		
		// Link point with normal/texture
		Obj->link(vidx,tidx,nidx);
		// in case of convex coplanar polygon
		FaceVertexList.push_back(vidx);
    }
    
	// triangulate polygon faces
	// TODO: add complex triangulation function
	for (int i = 1; i < FaceVertexList.size()-1; i++)
	{
		Obj->objFaceList.push_back(TriFace(
			FaceVertexList[0],
			FaceVertexList[i],
			FaceVertexList[i+1]
		));
	}
}


void handleObject::EXPORT(std::string filePath, TriMesh& object)
{
	Obj=&object;
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
    for (auto p : Obj->objVertexList)
    {	
        objEXPORT << "v\t"   << p.coord[0]  << "\t" << p.coord[1]  << "\t" << p.coord[2] << std::endl;
		objEXPORT << "vn\t"  << p.normal[0]    << "\t" << p.normal[1]    << "\t" << p.normal[2]   << std::endl;
		objEXPORT << "vt\t"  << p.texture[0]   << "\t" << p.texture[1]   <<std::endl;
	}	//std::cerr<<Obj->objVertexList.size()<<std::endl;
	// face
	
    for (auto f : Obj->objFaceList)
    {
		objEXPORT << "f\t" << f[0]+1 << '/' << f[0]+1 << '/' << f[0]+1  << "\t"
				  		   << f[1]+1 << '/' << f[1]+1 << '/' << f[1]+1  << "\t"
				           << f[2]+1 << '/' << f[2]+1 << '/' << f[2]+1         
						   << std::endl;			  
    }	//std::cerr<<Obj->objFaceList.size()<<std::endl;
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



