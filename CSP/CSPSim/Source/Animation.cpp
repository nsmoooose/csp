// Combat Simulator Project - CSPSim
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file Animation.cpp
 *
 **/


#include "Animation.h"

#include <osg/MatrixTransform>

#include <SimCore/Util/Log.h>

SIMDATA_REGISTER_INTERFACE(Animation)
SIMDATA_REGISTER_INTERFACE(Rotation)
SIMDATA_REGISTER_INTERFACE(DrivenRotation)
SIMDATA_REGISTER_INTERFACE(TimedAnimationProxy)
SIMDATA_REGISTER_INTERFACE(TimedRotation)
SIMDATA_REGISTER_INTERFACE(Translation)
SIMDATA_REGISTER_INTERFACE(DrivenMagnitudeTranslation)
SIMDATA_REGISTER_INTERFACE(DrivenVectorialTranslation)
SIMDATA_REGISTER_INTERFACE(TimedMagnitudeTranslation)
SIMDATA_REGISTER_INTERFACE(DrivenSwitch)
SIMDATA_REGISTER_INTERFACE(TimedSequence)
SIMDATA_REGISTER_INTERFACE(TimedAnimationPath)


void AnimationCallback::bind(osg::NodeCallback &node_callback) {
	node_callback.setNestedCallback(this);
	node_callback.setUserData(new UpdateCount);
	dirty();
}

bool AnimationCallback::needsUpdate(osg::NodeCallback& node_callback) {
	UpdateCount *count = dynamic_cast<UpdateCount*>(node_callback.getUserData());
	if (count) {
		if (count->getCount() < m_DirtyCount) {
			count->updateCount(m_DirtyCount);
			return true;
		}
	}
	return false;
}

Animation::Animation(float default_value):
	m_LOD(0),
	m_Default(default_value),
	m_Limit0(0.0f),
	m_Limit1(1.0f),
	m_Gain(1.0f) {
}

Rotation::Rotation():
	m_Axis(simdata::Vector3::ZERO),
	m_Phase(0.0f) {
}

void DrivenRotation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	updateValue();
	if (getNestedCallback() || (!node->getUpdateCallback()->getNestedCallback() && needsUpdate(*node))) {
		osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) {
			osg::Matrix m = osg::Matrix::rotate(m_Parameters->getGain() * m_Value + m_Parameters->getPhase(), 
												m_Parameters->getAxis());
			m.setTrans(t->getMatrix().getTrans());
			t->setMatrix(m);
		}
	} else if (node->getUpdateCallback()->getNestedCallback()) {
			osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(node);
			if (t) {
				osg::Matrix m = osg::Matrix::rotate(m_Parameters->getGain() * m_Value + m_Parameters->getPhase(), m_Parameters->getAxis());
				osg::Vec3 translation = t->getMatrix().getTrans();
				m.preMult(t->getMatrix());
				m.setTrans(translation);
				t->setMatrix(m);
			}
	}
	traverse(node, nv);
}

void TimedRotation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	updateValue();
	if (needsUpdate(*node)) {
		osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) {
			osg::Matrix m = osg::Matrix::rotate(m_Parameters->getTimedAngle(m_Value), m_Parameters->getAxis());
			m.setTrans(t->getMatrix().getTrans());
			t->setMatrix(m);
		}
	}
	else if (needsUpdate(*node->getUpdateCallback())) {
			osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(node);
			if (t) {
				osg::Matrix m = osg::Matrix::rotate(m_Parameters->getTimedAngle(m_Value), m_Parameters->getAxis());
				osg::Vec3 translation = t->getMatrix().getTrans();
				m.preMult(t->getMatrix());
				m.setTrans(translation);
				t->setMatrix(m);
			}
		}
	traverse(node, nv);
}

Translation::Translation():
	m_Direction(simdata::Vector3::ZERO),
	m_Offset(simdata::Vector3::ZERO) {
}

void DrivenMagnitudeTranslation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	updateValue();
	if (needsUpdate(*node)) {
		osg::MatrixTransform *t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) {
			osg::Vec3 translation = m_Parameters->getDirection() * m_Parameters->getGain() * m_Value 
								  + m_Parameters->getOffset();
			osg::Matrix m = t->getMatrix();
			m.setTrans(translation);
			t->setMatrix(m);
		}
	}
	traverse(node, nv);
}

void TimedMagnitudeTranslation::postCreate() {
	Translation::postCreate();
	m_TimedAnimationProxy->setRate(getLimit0(),getLimit1());
}

void TimedMagnitudeTranslation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	updateValue();
	if (needsUpdate(*node)) {
		osg::MatrixTransform *t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) { 
			osg::Vec3 translation = m_Parameters->getTimedTranslation(m_Value) * m_Parameters->getGain() 
								  + m_Parameters->getOffset();
			osg::Matrix m = t->getMatrix();
			m.setTrans(translation);
			t->setMatrix(m);
		}
	}
	traverse(node, nv);
}

void DrivenVectorialTranslation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	updateValue();
	if (needsUpdate(*node)) {
		osg::MatrixTransform *t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) {
			osg::Vec3 translation = m_Parameters->getDirection() * m_Parameters->getGain() * m_Value 
								  + m_Parameters->getOffset();
			osg::Matrix m = t->getMatrix();
			m.setTrans(translation);
			t->setMatrix(m);
		}
	}
	traverse(node, nv);
}

void DrivenSwitch::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	updateValue();
	if (needsUpdate(*node)) {
		assert(node);
		if (m_Value != 0.0)
			node->setNodeMask(0x3);
		else
			node->setNodeMask(0x1);
	}
	traverse(node, nv);
}

void TimedAnimationPath::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	updateValue();
	if (needsUpdate(*node)) {
		if (_animationPath.valid() && nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && nv->getFrameStamp()) {
			double time = nv->getFrameStamp()->getReferenceTime();
			_latestTime = time;
			//((_latestTime-_firstTime)-_timeOffset)*_timeMultiplier;
			setTimeOffset(_latestTime - _firstTime - m_Value);
			if (!_pause) {
				// Only update _firstTime the first time, when its value is still DBL_MAX
				if (_firstTime == DBL_MAX) _firstTime = time;
				update(*node);
			}
		}
	}
	traverse(node, nv);
}

