#include "stdinc.h"
#include "DirVectorDrawable.h"
#include "AirplaneObject.h"

extern int g_ScreenWidth;
extern int g_ScreenHeight;
extern BaseObject * g_pPlayerObject;

bool DirVectorCullCallback::cull( osg::NodeVisitor *visitor, osg::Drawable *drawable, osg::State *state) const
{
	CSP_LOG(CSP_APP, CSP_DEBUG, "DirVectorCullCallback::cull" );
	return true;
}


DirVectorDrawable::DirVectorDrawable()
{
	setSupportsDisplayList(false);
	DirVectorCullCallback * pCallback = new DirVectorCullCallback;
	setCullCallback( pCallback );

}

DirVectorDrawable::DirVectorDrawable(const DirVectorDrawable& other,const osg::CopyOp& copyop) :
        Drawable(other,copyop)
{

}


DirVectorDrawable::~DirVectorDrawable()
{
}

osg::Object* DirVectorDrawable::cloneType() const
{
    // return a clone of this type of drawable.
	return new DirVectorDrawable();

}

osg::Object* DirVectorDrawable::clone(const osg::CopyOp& copyop) const
{
    // return a clone of this object, via the copy constructor.
    return new DirVectorDrawable(*this,copyop);

}

DirVectorDrawable&  DirVectorDrawable::operator = (const DirVectorDrawable&)
{
	return *this;

}

const char*  DirVectorDrawable::className() const
{
	return "DirVectorDrawable"; 
}


bool DirVectorDrawable::computeBound() const
{
	_bbox._min.x() = -25;
	_bbox._min.y() = -25;
	_bbox._min.y() = -25;
	_bbox._max.x() = 25;
	_bbox._max.y() = 25;
	_bbox._max.z() = 25;
	_bbox_computed = true;
    return true;
}

void DirVectorDrawable::drawImplementation(osg::State& state) const
{

	return;

	StandardVector3 pos = g_pPlayerObject->getLocalPosition();
	StandardVector3 dir = g_pPlayerObject->getDirection();
	StandardVector3 vel = g_pPlayerObject->getVelocity();
	StandardVector3 norm = g_pPlayerObject->getUpDirection();
	StandardVector3 right = dir^norm;
	vel.Normalize();
	right.Normalize();

	glLineWidth(3);

    glBegin(GL_LINES);

    glColor3f(0.0, 1.0, 0.0);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x + dir.x*25.0, pos.y + dir.y*25.0, pos.z + dir.z*25.0);

	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x + norm.x*25.0, pos.y + norm.y*25.0, pos.z + norm.z*25.0);

	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x + right.x*25.0, pos.y + right.y*25.0, pos.z + right.z*25.0);

    glColor3f(0.0, 0.0, 1.0);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x + vel.x*25.0, pos.y + vel.y*25.0, pos.z + vel.z*25.0);


    glEnd();


}
