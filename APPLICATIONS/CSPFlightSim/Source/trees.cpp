#include "stdinc.h"

#include <stdlib.h>

#include <osg/Billboard>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>

#include <osgDB/ReadFile>

#include "VirtualBattlefield.h"


#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

using namespace osg;

/**
 * struct _tree
 */
struct _tree
{
    int n;
    float x, y, z;
    float w, h;
};


static Geometry *makeTree( _tree const &p_tree)//, StateSet *dstate )
{
    float vv[][3] =
    {
        { -p_tree.w/2.0, 0.0, 0.0 },
        {  p_tree.w/2.0,  0.0, 0.0 },
        {  p_tree.w/2.0, 0.0, 2 * p_tree.h },
        { -p_tree.w/2.0, 0.0, 2 * p_tree.h },
    };

    Vec3Array& v = *(osgNew Vec3Array(4));
    Vec2Array& t = *(osgNew Vec2Array(4));
    Vec4Array& l = *(osgNew Vec4Array(1));

    int   i;

    l[0][0] = l[0][1] = l[0][2] = l[0][3] = 1;

    for( i = 0; i < 4; i++ )
    {
        v[i][0] = vv[i][0];
        v[i][1] = vv[i][1];
        v[i][2] = vv[i][2];
    }

    t[0][0] = 0.0; t[0][1] = 0.0;
    t[1][0] = 1.0; t[1][1] = 0.0;
    t[2][0] = 1.0; t[2][1] = 1.0;
    t[3][0] = 0.0; t[3][1] = 1.0;

    Geometry *geom = osgNew Geometry;

    geom->setVertexArray( &v );

    geom->setTexCoordArray( 0, &t );

    geom->setColorArray( &l );
    geom->setColorBinding( Geometry::BIND_OVERALL );

    geom->addPrimitiveSet( osgNew DrawArrays(PrimitiveSet::QUADS,0,4) );

    //geom->setStateSet( dstate );

    return geom;
}

float RandomNumber()

{
	float fnum = static_cast<float>(rand())/RAND_MAX;
	return fnum;
}


std::vector<Geometry*> MakeBaseTreeArray(unsigned short p_ustypeTreeNumber, std::vector<StateSet *> p_pstate)
{
	float const treeWidthFactor = 1.0;
	float const treeHeightFactor = 1.0;

	_tree aTree;
	std::vector<Geometry*> treesArray;

	for (unsigned short i=0;i<p_ustypeTreeNumber;++i)
	{
		aTree.h = treeHeightFactor * 10.0;
		aTree.w = treeWidthFactor  * 5.0;
		treesArray.push_back(makeTree( aTree));//, osgNew StateSet(*p_pstate[i % 3]) ));
	}

	return treesArray;
}

std::vector<Geometry *> g_treesBase(1);

Node *makeTreesPatch( float xoff, float yoff, float spacing, float width, 
					 float height, VirtualBattlefield * pBattlefield)
{
    Group *group = osgNew Group;
    unsigned int i,j, icnt, jcnt;

	std::vector<std::string> treefileName;

	treefileName.push_back("Images/tree0.rgba");
    treefileName.push_back("Images/tree1.gif");
	treefileName.push_back("Images/tree2.gif");
	treefileName.push_back("Images/tree3.gif");
    treefileName.push_back("Images/tree4.gif");

	unsigned short typeTreeNumber = treefileName.size();

	std::vector<Texture2D*> tex;
    
	for (i=0;i<typeTreeNumber;++i)
	{
    Texture2D* aTex = osgNew Texture2D;
	osg::Image *image = osgDB::readImageFile ( treefileName[i] );
	aTex->setImage(image);
    tex.push_back(aTex);
    }
	
    std::vector<StateSet*> state;
	for (i=0;i<typeTreeNumber;++i)
	{
		StateSet *dstate = osgNew StateSet;

		dstate->setTextureAttributeAndModes(0, tex[i], StateAttribute::ON );
		
		dstate->setTextureAttribute(0, osgNew TexEnv );
		
		dstate->setAttributeAndModes( osgNew BlendFunc, StateAttribute::ON );
	
		AlphaFunc* alphaFunc = osgNew AlphaFunc;
		alphaFunc->setFunction(AlphaFunc::GEQUAL,0.05f);
		dstate->setAttributeAndModes( alphaFunc, StateAttribute::ON );
		
		dstate->setMode( GL_LIGHTING, StateAttribute::OFF );
		
		dstate->setRenderingHint( StateSet::TRANSPARENT_BIN );
		
		
		state.push_back(dstate);
    }

	icnt = 10; //width/spacing;
	jcnt = 10; //height/spacing;

	if (!g_treesBase[0]) 
	{
		g_treesBase = MakeBaseTreeArray(typeTreeNumber,state);
    }

	unsigned short count = 0;
	float baseRadius = 64.0 + 32.0 * RandomNumber();

	for (j=0;j<jcnt;++j)

	{
		Billboard *bb = osgNew Billboard;
		unsigned short indice;
		//bb->setMode(osg::Billboard::POINT_ROT_EYE );
		for (i=0;i<icnt;++i)
		{
			float angle = 2 * PI * RandomNumber();
			spacing = RandomNumber() * 128.0 + baseRadius;
			float x = xoff + spacing * ( cos(angle));
			float y = yoff + spacing * ( sin(angle) );
			float z = pBattlefield->getElevation(x, y);
			x = fmod(x, 64000.0f);
			y = fmod(y, 64000.0f);
			Vec3 pos(x,y,z);
			
			indice = (2 * count) % typeTreeNumber;
			Geometry * aGSet = osgNew Geometry(*g_treesBase[indice]);
            bb->addDrawable( aGSet, pos );
			++count;
		}
		group->addChild( bb );
	}
    group->setStateSet( state[static_cast<unsigned short>(RandomNumber() * typeTreeNumber)] );
    return group;
}
