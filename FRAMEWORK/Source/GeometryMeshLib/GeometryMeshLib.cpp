#include <Framework.h>


StandardPolygonMesh * GeometryMeshLib::CreateCube(StandardGraphics *p_3D)
{
  CSP_LOG(CSP_GEOMETRY, CSP_TRACE, "GeometryMeshLib::CreateCube()");


   StandardPolygonMesh * pCube = p_3D->CreatePolygonMesh();
   StandardNTVertex cubeVert;

	pCube->BeginMesh(FW_TRIANGLES);

   // bottom face
   // triangle 1
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,-1.0f,1.0f,-0.33f,-0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,0.33f,-0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 2
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-.033f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,0.33f,-0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,-1.0f,0.33f,-.033f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // top face
   // triangle 3
   cubeVert.Initialize(-1.0f,1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 4
   cubeVert.Initialize(-1.0f,1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // left face
   // triangle 5
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 6
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,-1.0f,1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // right face
   // triangle 7
   cubeVert.Initialize(1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 8
   cubeVert.Initialize(1.0f,-1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // back face
   // triangle 9
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 10
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,-1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // front face
   // triangle 11
   cubeVert.Initialize(-1.0f,-1.0f,1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 12
   cubeVert.Initialize(-1.0f,-1.0f,1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   pCube->EndMesh();


   return pCube;
}

/**
 * GeometryMeshLib::CreateRandomTerrainMesh()
 * This routine creates a random terrain mesh.
 * 
 */
StandardPolygonMesh * 
GeometryMeshLib::CreateRandomTerrainMesh(
    StandardGraphics *p_3D,
    int grid_x_count,
    int grid_z_count,
    float grid_x_size,
    float grid_z_size,
    float heightfactor)
{
  CSP_LOG(CSP_GEOMETRY, CSP_TRACE, "GeometryMeshLib::CreateRandomTerrainMesh()");

  int i, j;
  int n=grid_x_count; 
  int m=grid_z_count;

  StandardPolygonMesh * pTerrainMesh = p_3D->CreatePolygonMesh();
  StandardNTVertex * terrainVert = new StandardNTVertex[grid_x_count*grid_z_count];

  _Index * indices = new _Index[(n-1)*(m-1)*2*3];


  unsigned short vertexCount=0;
  float grid_x_scalefactor = 2.0f*grid_x_size/grid_x_count;
  float grid_z_scalefactor = 2.0f*grid_z_size/grid_z_count;
  float grid_x_texturescale = 1.0f/grid_x_count;
  float grid_z_texturescale = 1.0f/grid_z_count;
  for(i=0;i<n;i++)
	  for(j=0;j<m;j++)
	  {
		terrainVert[vertexCount].p.x = -grid_x_size+i*grid_x_scalefactor; 
		terrainVert[vertexCount].p.y = (float)rand()/RAND_MAX*heightfactor; 
		//terrainVert[vertexCount].p.y = 0.0f;
		terrainVert[vertexCount].p.z = -grid_z_size+j*grid_z_scalefactor;
		terrainVert[vertexCount].n.x = 0.0;
		terrainVert[vertexCount].n.y = 1.0;
		terrainVert[vertexCount].n.z = 0.0;
		terrainVert[vertexCount].tu = i*grid_x_texturescale;           
		terrainVert[vertexCount].tv = j*grid_z_texturescale;

		vertexCount++;
		
	  }

	unsigned short indexCount=0;
	for (i=0;i<m-1;i++)
		for (j=0;j<n-1;j++)
		{
			// right hand triangles
			indices[indexCount++] = i*m+j;
			indices[indexCount++] = i*m+j+1;
			indices[indexCount++] = (i+1)*m+j;

			indices[indexCount++] = (i+1)*m+j;
			indices[indexCount++] = i*m+j+1;
			indices[indexCount++] = (i+1)*m+j+1;

			// left hand triangles
//			Indices[indexCount++] = i*m+j;
//			Indices[indexCount++] = (i+1)*m+j;
//			Indices[indexCount++] = i*m+j+1;

//			Indices[indexCount++] = (i+1)*m+j;
//			Indices[indexCount++] = (i+1)*m+j+1;
//			Indices[indexCount++] = i*m+j+1;
		
		}
	pTerrainMesh->Initialize(FW_TRIANGLES, vertexCount, terrainVert);
	pTerrainMesh->SetIndexBuffer(indexCount, indices);

    delete [] terrainVert;
    delete [] indices;

	return pTerrainMesh;

}

StandardPolygonMesh * 
GeometryMeshLib::CreateFractalTerrainMesh(
    StandardGraphics *p_3D,
    int grid_x_count,
    int grid_z_count,
    float grid_x_size,
    float grid_z_size,
    float heightfactor)
{
  CSP_LOG(CSP_GEOMETRY, CSP_TRACE, "GeometryMeshLib::CreateFractalTerrainMesh()");

  StandardPolygonMesh * pTerrainMesh = p_3D->CreatePolygonMesh();

  FractalTerrainGenerator generator;
  generator.process(pTerrainMesh);

//  StandardVector3 r0(-1, 0, 1);
//  StandardVector3 r1(1, 0, 1);
//  StandardVector3 r2(1, 0, -1);
//  StandardVector3 r3(-1, 0, -1);

//  subdivide_square(pTerrainMesh, r0, r1, r2, r3, 1, 0.1, 1.0);

  return pTerrainMesh;
}

void 
GeometryMeshLib::subdivide_square(StandardPolygonMesh * pTerrainMesh, StandardVector3 r0, StandardVector3 r1, 
                  StandardVector3 r2, StandardVector3 r3, int n, float d , float r)
{
    StandardVector3 m;
    StandardVector3 e0, e1, e2, e3;

    if (n <= 0)
    {
        // process final triangle
        return;
    }

    m = (r0 + r1 + r2 + r3)/4;
    m.y += d*rand()/RAND_MAX - d;
    e0 = (r0 + r3 + m + m)/4; e0.y += d*rand()/RAND_MAX - d;
    e1 = (r0 + r1 + m + m)/4; e1.y += d*rand()/RAND_MAX - d;
    e2 = (r1 + r2 + m + m)/4; e2.y += d*rand()/RAND_MAX - d;
    e3 = (r2 + r3 + m + m)/4; e3.y += d*rand()/RAND_MAX - d;

    d *= pow(2,-r);
 
    subdivide_square(pTerrainMesh, r0, e1, m, e0, n-1, d, r);
    subdivide_square(pTerrainMesh, e1, r1, e2, m, n-1, d, r);
    subdivide_square(pTerrainMesh, m, e2, r2, e3, n-1, d, r);
    subdivide_square(pTerrainMesh, e0, m, e3, r3, n-1, d, r);

}



/////////////////////////////////////////////////////////////////////






FractalTerrainGenerator::FractalTerrainGenerator()
{
  CSP_LOG(CSP_GEOMETRY, CSP_TRACE, "FractalTerrainGenerator::FractalTerrainGenerator()");

}
 
FractalTerrainGenerator::~FractalTerrainGenerator()
{
  CSP_LOG(CSP_GEOMETRY, CSP_TRACE, "FractalTerrainGenerator::~FractalTerrainGenerator()");
// clean up list and vertices
}

void 
FractalTerrainGenerator::process(StandardPolygonMesh * pTerrainMesh)
{
  CSP_LOG(CSP_GEOMETRY, CSP_TRACE, "FractalTerrainGenerator::process()");

  FractalVertex * v1 = new FractalVertex;
    v1->v = StandardVector3(-1,0,1);
    v1->v *= 50.0f;
    v1->num = -1;
    vertexList.push_back(v1);

    FractalVertex * v2 = new FractalVertex;
    v2->v = StandardVector3(1,0,1);
    v2->v *= 50.0f;
    v2->num = -1;
    vertexList.push_back(v2);

    FractalVertex * v3 = new FractalVertex;
    v3->v = StandardVector3(1,0,-1);
    v3->v *= 50.0f;
    v3->num = -1;
    vertexList.push_back(v3);

    // fractize top triangle
    subdivide_triangle(v1, v2, v3, 0, 0.0, 1.0);

//    FractalVertex * v4 = new FractalVertex;
//    v4->v = StandardVector3(1,0,1);
//    v4->v *= 50000.0f;
//    v4->num = -1;

//    subdivide_triangle(v2, v4, v3, 1, 5000.0, 1.0);

    //use vertex and triangle list to populate TerrainMesh
    StandardVertex * terrainVert = new StandardVertex[vertexList.size()];
    list<FractalVertex * >::iterator terrainVertItor;
    int count = 0;
    for (terrainVertItor = vertexList.begin(); terrainVertItor != vertexList.end(); ++terrainVertItor)
    {
        FractalVertex * vertex = *terrainVertItor;
        vertex->num = count;
        terrainVert[count].p.x = vertex->v.x;
        terrainVert[count].p.y = vertex->v.y;
        terrainVert[count].p.z = vertex->v.z;
        
        CSP_LOG(CSP_GEOMETRY, CSP_INFO, "Num: " << vertex->num <<
                ", Vertex: " << vertex->v );

        count++;
    }
   	pTerrainMesh->Initialize(FW_TRIANGLES, count, terrainVert);


    _Index * indices = new _Index[3*triangleList.size()];
    count = 0;
    list<FractalTriangle * >::iterator terrainTriangleItor;
    for (terrainTriangleItor = triangleList.begin() ; terrainTriangleItor != triangleList.end(); terrainTriangleItor++)
    {
        indices[count++] = (unsigned short)(*terrainTriangleItor)->fv1->num;
        indices[count++] = (unsigned short)(*terrainTriangleItor)->fv2->num;
        indices[count++] = (unsigned short)(*terrainTriangleItor)->fv3->num;

        CSP_LOG(CSP_GEOMETRY, CSP_INFO, "Indices: " << (*terrainTriangleItor)->fv1->num << ", "
                                                     << (*terrainTriangleItor)->fv2->num << ", "
                                                     << (*terrainTriangleItor)->fv3->num );
    
    }

	pTerrainMesh->SetIndexBuffer(count, indices);

    

}


void
FractalTerrainGenerator::subdivide_triangle( 
                FractalVertex * v1, FractalVertex * v2, FractalVertex * v3,
                int n, float d, float r)
{
  CSP_LOG(CSP_GEOMETRY, CSP_TRACE, "FractalTerrainGenerator::subdivide_triangle()");

    if (n <= 0)
    {
        FractalTriangle * triangle = new FractalTriangle;
        triangle->fv1 = v1;
        triangle->fv2 = v2;
        triangle->fv3 = v3;
        triangleList.push_back(triangle);
        // end of recursion, add the triangle to the mesh.
        return;
    }

    FractalVertex * m1 = new FractalVertex;
    m1->v = (v1->v + v2->v)/2.0;
    m1->v.y += d*rand()/RAND_MAX - d;
    m1->num = -1;
    vertexList.push_back(m1);

    FractalVertex * m2 = new FractalVertex;
    m2->v = (v2->v + v3->v)/2.0;
    m2->v.y += d*rand()/RAND_MAX - d;
    m2->num = -1;
    vertexList.push_back(m2);

    FractalVertex * m3 = new FractalVertex;
    m3->v = (v3->v + v1->v)/2.0;
    m3->v.y += d*rand()/RAND_MAX - d;
    m3->num = -1;
    vertexList.push_back(m3);

    d *= pow(2,-r);

    subdivide_triangle(v1, m1, m3, n-1, d, r);
    subdivide_triangle(v2, m2, m1, n-1, d, r);
    subdivide_triangle(v3, m3, m2, n-1, d, r);
    subdivide_triangle(m1, m2, m3, n-1, d, r);

}

