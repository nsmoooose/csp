#ifndef __DIRVECTORDRAWABLE_H__
#define __DIRVECTORDRAWABLE_H__

#include <osg/Drawable>
#include <osg/NodeVisitor>
#include <osg/Object>

class DirVectorCullCallback : public osg::Drawable::CullCallback
{
	virtual bool cull (osg::NodeVisitor *visitor, osg::Drawable *drawable,class osg::State *) const;

};


class DirVectorDrawable : public osg::Drawable
{

public:

  DirVectorDrawable();
  DirVectorDrawable(const DirVectorDrawable&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
                                         
  DirVectorDrawable&  operator = (const DirVectorDrawable&) ;
  virtual ~DirVectorDrawable();

  virtual const char*         className() const;

  virtual void                drawImmediateMode(osg::State&);

  virtual osg::Object*        cloneType() const;
  virtual osg::Object*        clone(const osg::CopyOp& copyop) const;

        protected:
        
            virtual const bool          computeBound() const;

};

#endif
