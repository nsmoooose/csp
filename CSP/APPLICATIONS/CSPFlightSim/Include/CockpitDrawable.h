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
 * @file CockpitDrawable.h
 *
 **/

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

#endif // __COCKPITDRAWABLE_H__

