#include "stdinc.h"
#include "CockpitDrawable.h"

extern int g_ScreenWidth;
extern int g_ScreenHeight;


CockpitDrawable::CockpitDrawable()
{
	setSupportsDisplayList(false);

}

CockpitDrawable::CockpitDrawable(const CockpitDrawable& other,const osg::CopyOp& copyop) :
        Drawable(other,copyop)
{

}


CockpitDrawable::~CockpitDrawable()
{

}

osg::Object* CockpitDrawable::cloneType() const
{
    // return a clone of this type of drawable.
	return new CockpitDrawable();

}

osg::Object* CockpitDrawable::clone(const osg::CopyOp& copyop) const
{
    // return a clone of this object, via the copy constructor.
    return new CockpitDrawable(*this,copyop);

}

CockpitDrawable&  CockpitDrawable::operator = (const CockpitDrawable&)
{
	return *this;

}

const char*  CockpitDrawable::className() const
{
	return "CockpitDrawable"; 
}


const bool CockpitDrawable::computeBound() const
{
    return false;
}

void CockpitDrawable::drawImmediateMode(osg::State& state)
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0 , g_ScreenWidth, 0 , g_ScreenHeight, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

    glColor3f(0.5, 0.5, 0.5);

	int halfHeight = g_ScreenHeight >> 1;
	int halfWidth = g_ScreenWidth >> 1;

    glBegin(GL_LINES);
    glVertex2f(halfWidth-10, halfHeight);
    glVertex2f(halfWidth+10, halfHeight);
    glVertex2f(halfWidth, halfHeight-10);
    glVertex2f(halfWidth, halfHeight+10);
    glEnd();

  	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();


}
