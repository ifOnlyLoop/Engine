#pragma once

#include"Vertex.h"
#include"Face.h"
#include"ObjFile.h"

class Vertex;
class Face;

class FaceUtil: public Face
{
public:
	 FaceUtil();
	~FaceUtil();

	void triangulation(int, ObjData&);
	void subDivision(int, ObjData&, float, float);
	
private:

};
