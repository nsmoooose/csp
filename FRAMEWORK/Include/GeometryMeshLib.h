#ifndef __GEOMETRYMESHLIB_H__
#define __GEOMETRYMESHLIB_H__

#include <Framework.h>


struct FractalVertex
{
    StandardVector3 v;
    int num;
};

struct FractalTriangle
{
    FractalVertex * fv1;
    FractalVertex * fv2;
    FractalVertex * fv3;
    int num;
};

class FractalTerrainGenerator
{
public:

    FractalTerrainGenerator();
    virtual ~FractalTerrainGenerator();
    void process(StandardPolygonMesh * pTerrainMesh);

private:
    list<FractalVertex * > vertexList;
    list<FractalTriangle * > triangleList;

    void
        subdivide_triangle( 
                FractalVertex * v1, FractalVertex * v2, FractalVertex * v3,
                int n, float d, float r);


};

class GeometryMeshLib
{

public:
    static StandardPolygonMesh * 
    CreateRandomTerrainMesh(StandardGraphics *p_3D,
        int grid_x_count,
        int grid_z_count,
        float grid_x_size,
        float grid_z_size,
        float heightfactor);

    static StandardPolygonMesh * CreateCube(StandardGraphics *p_3D);

    static StandardPolygonMesh * CreateFractalTerrainMesh(
    StandardGraphics *p_3D,
    int grid_x_count,
    int grid_z_count,
    float grid_x_size,
    float grid_z_size,
    float heightfactor);

    static void 
    subdivide_square(StandardPolygonMesh * pTerrainMesh, StandardVector3 r0, StandardVector3 r1, 
                  StandardVector3 r2, StandardVector3 r3, int n, float d, float r);



};



#endif
