#include "stdinc.h"
#include <math.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/Depth>
#include <osg/StateSet>

#include <osgDB/ReadFile>

using namespace osg;

Node *makeBase( void )
{
    int i, c;
    float theta;
    float ir = 25000.0f;

    Vec3Array *coords = new Vec3Array(19);
    Vec2Array *tcoords = new Vec2Array(19);
    Vec4Array *colors = new Vec4Array(1);

    (*colors)[0].set(1.0f,1.0f,1.0f,1.0f);

    c = 0;
    (*coords)[c].set(0.0f,0.0f,0.0f);
    (*tcoords)[c].set(0.0f,0.0f);
    
    for( i = 0; i <= 18; i++ )
    {
        theta = osg::DegreesToRadians((float)i * 20.0);

        (*coords)[c].set(ir * cosf( theta ), ir * sinf( theta ), 0.0f);
        (*tcoords)[c].set((*coords)[c][0]/36.0f,(*coords)[c][1]/36.0f);

        c++;
    }

    Geometry *geom = osgNew Geometry;

    geom->setVertexArray( coords );

    geom->setTexCoordArray( 0, tcoords );

    geom->setColorArray( colors );
    geom->setColorBinding( Geometry::BIND_OVERALL );

    geom->addPrimitiveSet( new DrawArrays(PrimitiveSet::TRIANGLE_FAN,0,19) );

    Texture2D *tex = new Texture2D;

    tex->setImage(osgDB::readImageFile("Images/water.rgb"));
    tex->setWrap( Texture::WRAP_S, Texture::REPEAT );
    tex->setWrap( Texture::WRAP_T, Texture::REPEAT );

    StateSet *dstate = new StateSet;
    dstate->setMode( GL_LIGHTING, StateAttribute::ON );
    dstate->setTextureAttributeAndModes( 0, tex, StateAttribute::ON );

    dstate->setTextureAttributeAndModes( 0, new TexEnv );

    // clear the depth to the far plane.
    osg::Depth* depth = osgNew osg::Depth;
    depth->setFunction(osg::Depth::ALWAYS);
    depth->setRange(1.0,1.0);   
    dstate->setAttributeAndModes(depth,StateAttribute::ON );

    dstate->setRenderBinDetails(-1,"RenderBin");


    geom->setStateSet( dstate );

    Geode *geode = osgNew Geode;
    geode->addDrawable( geom );

    geode->setName( "Base" );

	//return 0;
    return geode;
}
