#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <cmath>

#include <osg/Depth>
#include <osg/Fog>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Light>
#include <osg/Texture2D>
#include <osg/TexEnv>

#include <osgDB/ReadFile>

using namespace osg;

const float SKY_RED = 0.1f;
const float SKY_GREEN = 0.1f;
const float SKY_BLUE = 0.6f;
const float SKY_ALPHA = 1.0f;

const float FOG_RED = 0.4f;
const float FOG_GREEN = 0.4f;
const float FOG_BLUE = 0.5f;
const float FOG_ALPHA = 1.0f;


Node *makeSky( void )
{
    /*
	int i, j;
    float lev[] = {-10, -5, -1.0, 1.0, 15.0, 30.0, 45.0, 60.0, 75.0, 90.0  };
    float cc[][4] =
    {
        { 0.0, 0.0, 0.15 },
        { 0.0, 0.0, 0.15 },
        { 0.4, 0.4, 0.5 },
        { 0.4, 0.4, 0.5 },
        { 0.4, 0.4, 0.5 },
//      { 0.2, 0.2, 0.6 },
        { 0.75*FOG_RED + 0.25*SKY_RED, 0.75*FOG_GREEN + 0.25*SKY_GREEN, 0.75*FOG_BLUE + 0.25*SKY_BLUE },
        { 0.5*FOG_RED + 0.5*SKY_RED, 0.5*FOG_GREEN + 0.5*SKY_GREEN, 0.5*FOG_BLUE + 0.5*SKY_BLUE },
        { 0.25*FOG_RED + 0.75*SKY_RED, 0.25*FOG_GREEN + 0.75*SKY_GREEN, 0.25*FOG_BLUE + 0.75*SKY_BLUE },
        { 0.1*FOG_RED + 0.9*SKY_RED, 0.1*FOG_GREEN + 0.9*SKY_GREEN, 0.1*FOG_BLUE + 0.9*SKY_BLUE },
        { SKY_RED, SKY_GREEN, SKY_BLUE },
    };
    float x, y, z;
    float alpha, theta;
    float radius = 32000.0f;
    int nlev = sizeof( lev )/sizeof(float);
    Geometry *geom = osgNew Geometry;

    Vec3Array& coords = *(new Vec3Array(19*nlev));
    Vec4Array& colors = *(new Vec4Array(19*nlev));
    Vec2Array& tcoords = *(new Vec2Array(19*nlev));
    
    
    int ci, ii;
    ii = ci = 0;

    for( i = 0; i < nlev; i++ )
    {
        for( j = 0; j <= 18; j++ )
        {
            alpha = osg::DegreesToRadians(lev[i]);
            theta = osg::DegreesToRadians((float)(j*20));

            x = radius * cosf( alpha ) * cosf( theta );
            y = radius * cosf( alpha ) * -sinf( theta );
            z = radius * sinf( alpha );

            coords[ci][0] = x;
            coords[ci][1] = y;
            coords[ci][2] = z;

            colors[ci][0] = cc[i][0];
            colors[ci][1] = cc[i][1];
            colors[ci][2] = cc[i][2];
            colors[ci][3] = 1.0;

            tcoords[ci][0] = (float)j/18.0;
            tcoords[ci][0] = (float)i/(float)(nlev-1);

            ci++;
        }


    }

    for( i = 0; i < nlev-1; i++ )
    {
        for( j = 0; j <= 18; j++ )
        {

            DrawElementsUShort* drawElements = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP);
            drawElements->reserve(38);

            for( j = 0; j <= 18; j++ )
            {
                drawElements->push_back((i+1)*19+j);
                drawElements->push_back((i+0)*19+j);
            }
            
            geom->addPrimitiveSet(drawElements);
        }
    }
    
    geom->setVertexArray( &coords );
    geom->setTexCoordArray( 0, &tcoords );

    geom->setColorArray( &colors );
    geom->setColorBinding( Geometry::BIND_PER_VERTEX );


    Texture2D *tex = osgNew Texture2D;
    tex->setImage(osgDB::readImageFile("Images/CSP_Splash.tga"));

    StateSet *dstate = osgNew StateSet;

    dstate->setTextureAttributeAndModes( 0, tex, StateAttribute::OFF );
    dstate->setTextureAttributeAndModes( 0, osgNew TexEnv );
    dstate->setMode( GL_LIGHTING, StateAttribute::OFF );
    dstate->setMode( GL_CULL_FACE, StateAttribute::ON );
    

    // clear the depth to the far plane.
    osg::Depth* depth = osgNew osg::Depth;
    depth->setFunction(osg::Depth::ALWAYS);
    depth->setRange(1.0,1.0);   
    dstate->setAttributeAndModes(depth,StateAttribute::ON );
    dstate->setMode ( GL_FOG, osg::StateAttribute::OFF );
    dstate->setRenderBinDetails(-2,"RenderBin");

    geom->setStateSet( dstate );

    Geode *geode = osgNew Geode;
    geode->addDrawable( geom );

    geode->setName( "Sky" );

    return geode;

   */
    
    // create a new group for the sky dome
    osg::Group * skydome_group = osgNew osg::Group;

    // read in the sky model
    osg::Node * skydome = osgDB::readNodeFile ( "Models/skydome.osg" );

	osg::BoundingSphere s = skydome->getBound();
	float r = s.radius();
	osg::Vec3 c = s.center();

    // add the skydome
    skydome_group->addChild ( skydome );

    // special stateset for the skydome group
    osg::StateSet * stateset = osgNew osg::StateSet;

	osg::Depth* depth = osgNew osg::Depth;
    depth->setFunction(osg::Depth::ALWAYS);
    depth->setRange(1.0,1.0);   
    stateset->setAttributeAndModes(depth,StateAttribute::ON );
	stateset->setRenderBinDetails(-2,"RenderBin");

	osg::StateSet * pFogState = osgNew osg::StateSet;
    osg::Fog* fog = osgNew osg::Fog;
    fog->setMode(osg::Fog::LINEAR);
    fog->setDensity(1.9f);
    fog->setStart(100.0f);
    fog->setEnd(30000);

	osg::Vec4 fogColor;
	fogColor[0] = FOG_RED;
	fogColor[1] = FOG_GREEN; 
	fogColor[2] = FOG_BLUE; 
	fogColor[3] = FOG_ALPHA;
	fog->setColor(fogColor);
    pFogState->setAttributeAndModes(fog ,osg::StateAttribute::ON);
	
	
	osg::Light * pLight = osgNew osg::Light;
	//pLight->setDirection( osg::Vec3(1,1,1) );
	//pLight->setAmbient( osg::Vec4(0.75, 0.75, 0.75, 1) );
	//pLight->setDiffuse( osg::Vec4(0.6, 0.6, 0.6, 1) );

	pLight->setDirection( osg::Vec3(0,0,-1) );
	//pLight->setAmbient( osg::Vec4(0.2f,0.2f,0.2f,1.0f) );
	pLight->setAmbient( osg::Vec4(0.4f,0.4f,0.4f,1.0f) );
	pLight->setDiffuse( osg::Vec4(0.8f,0.8f,0.8f,1.0f) );
	pLight->setSpecular( osg::Vec4(0.75f,0.75f,0.75f,1.0f) );
	osg::StateSet * pLightSet = osgNew osg::StateSet;
	
	pFogState->setAttributeAndModes(pLight, osg::StateAttribute::ON);
	
	stateset->merge(*pFogState);
	 // ... as we want to disable fogging
    stateset->setMode ( GL_FOG, osg::StateAttribute::ON );

    // assign the state set
    skydome_group->setStateSet (stateset);

    return skydome_group;
	
	
}

