// Combat Simulator Project
// Copyright (C) 2004-2005 The Combat Simulator Project
// http: //csp.sourceforge.net
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
 * @file InternalView.h
 *
 **/

#ifndef __CSPSIM_INTERNALVIEW_H__
#define __CSPSIM_INTERNALVIEW_H__

#include <map>

#include <csp/csplib/data/Quat.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Vector3.h>
#include "View.h"


/** An internal view that supports padlocking.
 */
class InternalView: public View {
public:
	InternalView(size_t vm);
	virtual void activate();
	virtual void select();
	virtual void reselect();
	virtual void deselect();

protected:
	using View::Contact;
	virtual void updateView(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);

private:
	simdata::Ref<DynamicObject> m_Padlock;

	bool m_Padlocked;
	void setPadlocked();
	bool checkPadlocked();

	double m_EyePhi;
	double m_EyeTheta;
	double m_NeckPhi;
	double m_NeckTheta;
	double m_TorsoTheta;
	double m_TorsoSideLean;
	double m_FlipTime;
	const double m_EyeRate;
	const double m_NeckRate;
	const double m_TorsoRate;
	const double m_EyeTurnLimit;
	const double m_EyeTiltLimit;
	const double m_NeckTurnLimit;
	const double m_NeckTiltLimit;
	const double m_TorsoTurnLimit;
	const simdata::Vector3 m_NeckPivotOffset;
	simdata::Vector3 m_NeckBobbing;
	simdata::Vector3 m_Up;
	simdata::Vector3 m_Dir;
	simdata::Vector3 m_DirLocal;
	simdata::Vector3 m_PadlockPosition;
	simdata::Vector3 m_PadlockVelocity;
	double m_PadlockLossTime;

	struct SortById;
	void moveViewpoint(simdata::Vector3& ep, simdata::Vector3& lp, simdata::Vector3& up, double dt);
	bool inVisualRange(Contact contact) const;
};


#endif // __CSPSIM_INTERNALVIEW_H__

