#include "FaceUtil.h"


FaceUtil::FaceUtil()
{
}

FaceUtil::~FaceUtil()
{
}

void FaceUtil::triangulation(int faceIndex, ObjData& objData)
{
    // Extract Face
    Face face = objData.faceList[faceIndex];
    // Declare Mesh Pivot
    int pivot = face.faceVertexList[0];
    // Temp Face Datatype to Store New Faces
    /* *
     * in the future copy face properties to it
     * */
    Face tempFace;
    // Size of the Polygon "n"
    int argSize = face.faceVertexList.size();
    // Special Case
    if(argSize<=3) 
        return;
    // Triangulate the Verteces
    for (int i = 1; i < argSize - 1; i++)
    {   
        // create new face to push
        tempFace.push
        (
            pivot,
            face.faceVertexList[i],
            face.faceVertexList[i + 1]
        );
        // replace original face by last face
        if(i!=argSize-2)
            objData.faceList.push_back(tempFace); 
        else 
            objData.faceList[faceIndex]=tempFace;
    }
}

void FaceUtil::subDivision(int faceIndex, ObjData& objData, float u, float v)
{

    /* Equations
     * #1: uV1 + vV2 + wV3 = p(x,y,z)
     * #2: u   + v   + w   = 1
     */
    
    // Add Constrains Laters
    // Extract Face
    Face face = objData.faceList[faceIndex];
    Face tempFace;
     // Extract Segement Points
    
    vect
        pivot = objData.vertexList[face[2]]
        + u * (objData.vertexList[face[0]] - objData.vertexList[face[0]])
        + v * (objData.vertexList[face[1]] - objData.vertexList[face[0]]);

    // Add to File Vertex List
    Vertex tempVertex;
    tempVertex.push(pivot);
    // Add to File Vertex List
    objData.vertexList.push_back(tempVertex);
    // Get Pivot Index in the Vertex List
    int j = objData.vertexList.size() - 1, 
        z = face.size();
    // Push SubDivided Faces to Face List 
    for (int i = 0; i < z; i++)
        if(i) // replace original face 
            objData.faceList[faceIndex]=Face(j, face[i], face[(i + 1) % z]);
        else
            objData.faceList.push_back(Face(j, face[i], face[(i + 1) % z]));
}