#ifndef __TYPESPOLYGONMESH_H__
#define __TYPESPOLYGONMESH_H__

#include "Framework.h"

class StandardPolygonMesh
{

private:


public:

	//StandardPolygonMesh(StandardGraphics *Graphics) {};
  virtual ~StandardPolygonMesh() {};

  // functions
  virtual short             Initialize(PrimitiveTypes Type, unsigned short vertexCount, StandardVertex *Vertexes) = 0;
  virtual short             Initialize(PrimitiveTypes Type, unsigned short vertexCount, StandardNTVertex *Vertexes) = 0;
  virtual short             Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardTVertex *v) = 0;
  virtual short             Initialize(PrimitiveTypes type, unsigned short vertexCount, StandardCVertex * v) = 0;
  virtual void              Uninitialize() = 0;

  virtual void              BeginMesh(PrimitiveTypes type) = 0;
  virtual void              EndMesh() = 0;

  virtual void              AddVertex(StandardNTVertex & vertex) = 0;
  virtual void              AddTriangle(StandardNTVertex & vertex1, 
	                                      StandardNTVertex & vertex2,
										                    StandardNTVertex & vertex3) = 0;

  virtual void              SetIndexBuffer(void *buffer, unsigned long bufferbytecount) = 0;
  virtual void              SetIndexBuffer(int indexCount, void * indices) = 0;

  virtual unsigned long     Draw() = 0;
  virtual void              SetPos(float x, float y, float z) = 0;
  virtual void              SetMatrix(StandardMatrix4 *Matrix) = 0;
  virtual void              SetMatrix(const StandardMatrix4 & mat) = 0;
  virtual void              MultiMatrix(const StandardMatrix4 & mat) = 0;

};


#endif
