#include <osg/Geode>
#include <osg/Geometry>

#include "BaseObject.h"
#include "HudGeometricElement.h"

extern BaseObject * g_pPlayerObject;

extern int g_ScreenWidth;
extern int g_ScreenHeight;

HudGeometricElement::HudGeometricElement(std::string const & p_selementName, osg::Vec3 const & p_position,
										 float const p_fheadingAngle, float const p_fpitchAngle):
HudElement(p_selementName, p_position), m_fheadingAngle(p_fheadingAngle), m_fpitchAngle(p_fpitchAngle)
{
	if ( m_selementName == "HSI" )
	{
		MakeHsi();
	}
	else 
		if (m_selementName == "FPM")
		{
			MakeFpm();
		}
	m_fheadingAngle = 0.0;
	m_fpitchAngle = 0.0;
}

HudGeometricElement::~HudGeometricElement()
{
}

void HudGeometricElement::MakeHsi()
{
	unsigned short const levelsNumber = 9;
	double const hsiStepSize = .1; 

	osg::Vec3Array* vertices = osgNew osg::Vec3Array;
	osg::Geometry* linesGeom = osgNew osg::Geometry;
	
	// middle segment
	vertices->push_back(osg::Vec3(-0.15 , 0.0, 0.0));
	vertices->push_back(osg::Vec3(0.15 , 0.0, 0.0));
	
	// fondamental vertices top: generate others segments with those ones
	osg::Vec3Array* fondVerticesTop = osgNew osg::Vec3Array;
	
	fondVerticesTop->push_back(osg::Vec3(-0.06 , 0.0, 0.05 ));
	fondVerticesTop->push_back(osg::Vec3(-0.06 , 0.0, 0.07 ));
	fondVerticesTop->push_back(osg::Vec3(-0.01 , 0.0, 0.07 ));

	unsigned short i;

    // top left
	for (i = 0; i < levelsNumber; ++i)
		for (unsigned short j = 0; j < fondVerticesTop->size(); ++j)
		{
			osg::Vec3 u((*fondVerticesTop)[j]);
			vertices->push_back(u + osg::Vec3(0.0, 0.0, hsiStepSize * i ));
		}

	// top right
	for (i = 0; i < levelsNumber; ++i)
		for (unsigned short j = 0; j < fondVerticesTop->size(); ++j)
		{
			osg::Vec3 u((*fondVerticesTop)[j]);
			u.x() =  - u.x();
			vertices->push_back(u + osg::Vec3(0.0, 0.0, hsiStepSize * i ));
		}
    
	// fondamental vertices bottom: generate others segments with those ones
	osg::Vec3Array* fondVerticesBottom = osgNew osg::Vec3Array;
	
	fondVerticesBottom->push_back(osg::Vec3(-0.06 , 0.0, 0.05 ));
	fondVerticesBottom->push_back(osg::Vec3(-0.06 , 0.0, 0.07 ));

    fondVerticesBottom->push_back(osg::Vec3(-0.06 , 0.0, 0.07 ));
	fondVerticesBottom->push_back(osg::Vec3(-0.05 , 0.0, 0.07 ));

    fondVerticesBottom->push_back(osg::Vec3(-0.04 , 0.0, 0.07 ));
	fondVerticesBottom->push_back(osg::Vec3(-0.03 , 0.0, 0.07 ));

    fondVerticesBottom->push_back(osg::Vec3(-0.02 , 0.0, 0.07 ));
	fondVerticesBottom->push_back(osg::Vec3(-0.01 , 0.0, 0.07 ));

	// bottom left
	for (i = 0; i < levelsNumber; ++i)
		for (unsigned short j = 0; j < fondVerticesBottom->size(); ++j)
		{
			osg::Vec3 u((*fondVerticesBottom)[j]);
			u.z() =  - u.z();
			vertices->push_back(u - osg::Vec3(0.0, 0.0, hsiStepSize * i ));
		}

	// bottom right
	for (i = 0; i < levelsNumber; ++i)
		for (unsigned short j = 0; j < fondVerticesBottom->size(); ++j)
		{
			osg::Vec3 u((*fondVerticesBottom)[j]);
			u.x() =  - u.x();
			u.z() =  - u.z();
			vertices->push_back(u - osg::Vec3(0.0, 0.0, hsiStepSize * i ));
		}

	// pass the created vertex array to the points geometry object.
	linesGeom->setVertexArray(vertices);
	
	// add all hsi segments
	// plain horizontal
	linesGeom->addPrimitiveSet(osgNew osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

	// level segments
	// top segments
    for (i = 0; i < 2 * levelsNumber; ++i)
	  linesGeom->addPrimitiveSet(osgNew osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,2 + fondVerticesTop->size() * i,
	                          fondVerticesTop->size()));
    // bottom segments
	for (i = 0; i < 2 * levelsNumber; ++i)
	{
	  osg::DrawArrays * array = osgNew osg::DrawArrays(osg::PrimitiveSet::LINES,
		       2 + 2 * fondVerticesTop->size() * levelsNumber + fondVerticesBottom->size() * i,fondVerticesBottom->size());
	  linesGeom->addPrimitiveSet(array);
	}

	osg::Geode * aGeode = osgNew osg::Geode();
	aGeode->addDrawable(linesGeom);
	addChild(aGeode);
}

void HudGeometricElement::MakeFpm(float const p_radius, float const p_segLengthH, float const p_segLengthV)
{
	unsigned short const verticesCircleNumber = 10;

	osg::Vec3Array* vertices = osgNew osg::Vec3Array;
	osg::Geometry* linesGeom = osgNew osg::Geometry;

	unsigned short i;

	// make a circle
	for (i = 0; i < verticesCircleNumber; ++i)
	{
		float angle = 2 * pi * i/verticesCircleNumber;
		vertices->push_back(osg::Vec3(p_radius * cos( angle ), 0.0, p_radius * sin( angle )));
	}

	// add horizontal segments
	vertices->push_back(osg::Vec3(- p_radius - p_segLengthH, 0.0, 0.0));
    vertices->push_back(osg::Vec3(- p_radius, 0.0, 0.0));
    vertices->push_back(osg::Vec3(p_radius, 0.0, 0.0));
    vertices->push_back(osg::Vec3(p_radius + p_segLengthH, 0.0, 0.0));
	// add vertical segment
	vertices->push_back(osg::Vec3(0.0, 0.0, p_radius));
    vertices->push_back(osg::Vec3(0.0, 0.0, p_radius + p_segLengthV));

	linesGeom->setVertexArray(vertices);

	// add circle primitive
    linesGeom->addPrimitiveSet(osgNew osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,verticesCircleNumber));
	// add segment primitives
	for (i = 0; i < 3; ++i)
		linesGeom->addPrimitiveSet(osgNew osg::DrawArrays(osg::PrimitiveSet::LINES,verticesCircleNumber + 2 * i, 
    		                                               2));
	osg::Geode * aGeode = osgNew osg::Geode();
	aGeode->addDrawable(linesGeom);
	addChild(aGeode);
}

void HudGeometricElement::OnUpdate()
{
  if ( m_selementName == "HSI" )
	{
		OnUpdateHsi();
	}
	else 
		if (m_selementName == "FPM")
		{
			OnUpdateFpm();
		}
}

void HudGeometricElement::OnUpdate(osg::Vec3 const & p_position, float p_fangle)
{
 
}

void HudGeometricElement::OnUpdateHsi()
{
  m_fheadingAngle = - osg::DegreesToRadians(g_pPlayerObject->getRoll()) / 2.0;
  m_fpitchAngle = - osg::DegreesToRadians(g_pPlayerObject->getPitch()) / 2.0;

  setPosition(osg::Vec3(0.0, 0.0, m_fpitchAngle));
  osg::Quat quat(m_fheadingAngle, osg::Vec3(0.0, 1.0, 0.0));
  setAttitude(quat);
}

void HudGeometricElement::OnUpdateFpm()
{
	StandardVector3 t1 = g_pPlayerObject->getVelocity() / g_pPlayerObject->getSpeed() - g_pPlayerObject->getDirection();
	osg::Vec3 t = osg::Vec3(t1.x, t1.y, t1.z) / 4.0; 
	//t.normalize();
	setPosition(t);
} 
