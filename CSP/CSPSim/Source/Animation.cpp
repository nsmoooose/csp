// Combat Simulator Project - CSPSim
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
 * @file Animation.cpp
 *
 **/


#include "Animation.h"
#include "Config.h"

#include <osg/MatrixTransform>

#include <SimCore/Util/Log.h>
#include <SimData/Math.h>


SIMDATA_REGISTER_INTERFACE(Animation)
SIMDATA_REGISTER_INTERFACE(DrivenRotation)
SIMDATA_REGISTER_INTERFACE(DrivenMagnitudeTranslation)
SIMDATA_REGISTER_INTERFACE(DrivenVectorialTranslation)
SIMDATA_REGISTER_INTERFACE(DrivenSwitch)

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

Animation::Animation():
	m_LOD(0),
	m_Default(0.0f)
{
}

Animation::~Animation()
{
}

DrivenRotation::DrivenRotation():
	m_Axis(0.0f, 0.0f, 0.0f),
	m_Limit0(-simdata::PI),
	m_Limit1(simdata::PI),
	m_Gain(1.0f),
	m_Offset(0.0f)
{
}

DrivenRotation::~DrivenRotation()
{
}

void DrivenRotation::postCreate()
{
}

void DrivenRotation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	if (m_Channel.valid()) {
		float value = m_Channel->value();
		if (value != m_Value) {
			m_Value = value;
			dirty();
		}
	}
	if (needsUpdate(*node)) {
		osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) {
			osg::Matrix m = osg::Matrix::rotate(m_Parameters->getGain()*m_Value, m_Parameters->getAxis());
			m.setTrans(t->getMatrix().getTrans());
			t->setMatrix(m);
		}
	}
	else if (needsUpdate(*node->getUpdateCallback())) {
			osg::MatrixTransform* t = dynamic_cast<osg::MatrixTransform*>(node);
			if (t) {
				osg::Matrix m = osg::Matrix::rotate(m_Parameters->getGain()*m_Value, m_Parameters->getAxis());
				osg::Vec3 translation = t->getMatrix().getTrans();
				m.preMult(t->getMatrix());
				m.setTrans(translation);
				t->setMatrix(m);
			}
		}
	traverse(node, nv);
}

DrivenMagnitudeTranslation::DrivenMagnitudeTranslation():
	m_Direction(simdata::Vector3::ZERO),
	m_Offset(simdata::Vector3::ZERO),
	m_Limit0(-1),
	m_Limit1(1),
	m_Gain(1.0f) {
}

DrivenMagnitudeTranslation::~DrivenMagnitudeTranslation() {
}

void DrivenMagnitudeTranslation::postCreate() {
	m_OSGOffset = simdata::toOSG(m_Offset);
}

void DrivenMagnitudeTranslation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	if (m_Channel.valid()) {
		float value = m_Channel->value();
		if (value != m_Value) {
			m_Value = value;
			dirty();
		}
	}
	if (needsUpdate(*node)) {
		osg::MatrixTransform *t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) {
			osg::Vec3 translation = m_Parameters->getDirection()*m_Value + m_Parameters->getOffset();
			osg::Matrix m = t->getMatrix();
			m.setTrans(translation);
			t->setMatrix(m);
		}
	}
	traverse(node, nv);
}

DrivenVectorialTranslation::DrivenVectorialTranslation():
	m_Direction(simdata::Vector3::ZERO),
	m_Offset(simdata::Vector3::ZERO),
	m_Limit0(-1),
	m_Limit1(1),
	m_Gain(1.0f) {
}

DrivenVectorialTranslation::~DrivenVectorialTranslation() {
}

void DrivenVectorialTranslation::postCreate() {
	m_OSGOffset = simdata::toOSG(m_Offset);
	m_Direction.normalized();
}

void DrivenVectorialTranslation::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	if (m_Channel.valid()) {
		double value = m_Parameters->getDirection() * simdata::toOSG(m_Channel->value());
		if (value != m_Value) {
			m_Value = value;
			dirty();
		}
	}
	if (needsUpdate(*node)) {
		osg::MatrixTransform *t = dynamic_cast<osg::MatrixTransform*>(node);
		assert(t);
		if (t) {
			osg::Vec3 translation = m_Parameters->getDirection()*m_Value + m_Parameters->getOffset();
			osg::Matrix m = t->getMatrix();
			m.setTrans(translation);
			t->setMatrix(m);
		}
	}
	traverse(node, nv);
}

DrivenSwitch::DrivenSwitch() {
}

DrivenSwitch::~DrivenSwitch() {
}

void DrivenSwitch::postCreate() {
}

void DrivenSwitch::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	if (m_Channel.valid()) {
		double value = m_Channel->value();
		if (value != m_Value) {
			m_Value = value;
			dirty();
		}
	}
	if (needsUpdate(*node)) {
		assert(node);
		if (m_Value != 0.0)
			node->setNodeMask(0x3);
		else
			node->setNodeMask(0x1);
	}
	traverse(node, nv);
}

