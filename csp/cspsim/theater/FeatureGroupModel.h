// Combat Simulator Project
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
 * @file FeatureGroupModel.h
 *
 **/


#ifndef __CSPSIM_THEATER_FEATUREGROUPMODEL_H__
#define __CSPSIM_THEATER_FEATUREGROUPMODEL_H__

#include <csp/cspsim/theater/FeatureModel.h>

namespace csp {

/**
 * class FeatureGroupModel (STATIC, ABSTRACT)
 *
 * A static collection of multiple FeatureModels that form part of
 * a FeatureGroup.
 */
class FeatureGroupModel: public FeatureModel {
public:
	CSP_DECLARE_ABSTRACT_OBJECT(FeatureGroupModel)
	FeatureGroupModel();
	virtual ~FeatureGroupModel();
};

} // namespace csp

#endif // __CSPSIM_THEATER_FEATUREGROUPMODEL_H__


