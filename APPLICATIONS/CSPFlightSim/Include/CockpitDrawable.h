#ifndef __COCKPITDRAWABLE_H__
#define __COCKPITDRAWABLE_H__

#include <osg/Drawable>
#include <osg/Object>



/**
 * class CockpitDrawable - Describe me!
 *
 * @author unknown
 */
class CockpitDrawable : public osg::Drawable
{

public:

  CockpitDrawable();
  CockpitDrawable(const CockpitDrawable&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
                                         
  CockpitDrawable&  operator = (const CockpitDrawable&) ;
  virtual ~CockpitDrawable();

  virtual const char*         className() const;

  virtual void                drawImplementation(osg::State&) const;

  virtual osg::Object*        cloneType() const;
  virtual osg::Object*        clone(const osg::CopyOp& copyop) const;

  protected:
        
  virtual bool          computeBound() const;

};

#endif
