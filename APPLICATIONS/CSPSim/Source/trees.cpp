// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file trees.cpp
 *
 **/

#include <sstream>

#include <osg/Billboard>
#include <osg/BlendFunc>
#include <osg/Geometry>
#include <osg/Texture2D>

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

    Vec3Array& v = *(new Vec3Array(4));
    Vec2Array& t = *(new Vec2Array(4));
    Vec4Array& l = *(new Vec4Array(1));

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

    Geometry *geom = new Geometry;

    geom->setVertexArray( &v );

    geom->setTexCoordArray( 0, &t );

    geom->setColorArray( &l );
    geom->setColorBinding( Geometry::BIND_OVERALL );

    geom->addPrimitiveSet( new DrawArrays(PrimitiveSet::QUADS,0,4) );

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
		aTree.h = treeHeightFactor * 8.0;
		aTree.w = treeWidthFactor  * 8.0;
		treesArray.push_back(makeTree( aTree));//, new StateSet(*p_pstate[i % 3]) ));
	}

	return treesArray;
}

std::vector<Geometry *> g_treesBase;

Node *makeTreesPatch( float xoff, float yoff, float spacing, float width, 
					 float height, VirtualBattlefield * pBattlefield)
{
    Group *group = new Group;
    unsigned int i,j, icnt, jcnt;

	std::vector<std::string> treefileName;

    treefileName.push_back("Images/tree0.rgba");

	for (i=5;i<15;++i)
	{
	 std::ostringstream osStream;
	 osStream << "Images/tree" << i << ".png";
	 string a = osStream.str();
     treefileName.push_back(osStream.str());
	}
    
	treefileName.push_back("Images/tree1.gif");
	treefileName.push_back("Images/tree2.gif");
	treefileName.push_back("Images/tree3.gif");
    treefileName.push_back("Images/tree4.gif");
    
	unsigned short typeTreeNumber = treefileName.size();

	std::vector<Texture2D*> tex;
    
	for (i=0;i<typeTreeNumber;++i)
	{
    Texture2D* aTex = new Texture2D;
	osg::Image *image = osgDB::readImageFile ( treefileName[i] );
	aTex->setImage(image);
    tex.push_back(aTex);
    }
	
    std::vector<StateSet*> state;
	for (i=0;i<typeTreeNumber;++i)
	{
		StateSet *dstate = new StateSet;

		dstate->setTextureAttributeAndModes(0, tex[i], StateAttribute::ON );
		
		dstate->setTextureAttribute(0, new TexEnv );
		
		dstate->setAttributeAndModes( new BlendFunc, StateAttribute::ON );
	
		AlphaFunc* alphaFunc = new AlphaFunc;
		alphaFunc->setFunction(AlphaFunc::GEQUAL,0.05f);
		dstate->setAttributeAndModes( alphaFunc, StateAttribute::ON );
		
		dstate->setMode( GL_LIGHTING, StateAttribute::OFF );
		
		dstate->setRenderingHint( StateSet::TRANSPARENT_BIN );
		
		
		state.push_back(dstate);
    }

	icnt = 10; //width/spacing;
	jcnt = 10; //height/spacing;

	if (g_treesBase.empty()) 
	{
		g_treesBase = MakeBaseTreeArray(typeTreeNumber,state);
    }

	unsigned short count = 0;
	float baseRadius = 64.0 + 32.0 * RandomNumber();

	unsigned short iState = static_cast<unsigned short>(RandomNumber() * typeTreeNumber);

	for (j=0;j<jcnt;++j)
	{
		Billboard *bb = new Billboard;
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
			unsigned short indice = (2 * count) % typeTreeNumber;
			Geometry * aGSet = new Geometry(*g_treesBase[indice]);
            bb->addDrawable( aGSet, pos );
			++count;
		}
		group->addChild( bb );
	}
	group->setStateSet( state[iState] );
    return group;
}
