#ifndef __DIRVECTORDRAWABLE_H__
#define __DIRVECTORDRAWABLE_H__

#include <osg/Drawable>
#include <osg/NodeVisitor>
#include <osg/Object>


/**
 * class DirVectorCullCallback - Describe me!
 *
 * @author unknown
 */
class DirVectorCullCallback : public osg::Drawable::CullCallback
{
	virtual bool cull (osg::NodeVisitor *visitor, osg::Drawable *drawable, osg::State *state=NULL) const;

};



/**
 * class DirVectorDrawable - Describe me!
 *
 * @author unknown
 */
class DirVectorDrawable : public osg::Drawable
{

public:

  DirVectorDrawable();
  DirVectorDrawable(const DirVectorDrawable&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
                                         
  DirVectorDrawable&  operator = (const DirVectorDrawable&) ;
  virtual ~DirVectorDrawable();

  virtual const char*         className() const;

  virtual void                drawImplementation(osg::State&) const;

  virtual osg::Object*        cloneType() const;
  virtual osg::Object*        clone(const osg::CopyOp& copyop) const;

        protected:
        
            virtual bool          computeBound() const;

};

#endif
