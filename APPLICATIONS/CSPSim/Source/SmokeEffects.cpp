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
 * @file SmokeEffects.cpp
 *
 **/


/*
 * need to clean up the ParticleEffect interface, and allow multiple operators
 *
 * need to provide access to the particle system after creation to allow
 * continuous variations (color, thickness, rate, velocity, lifetime, etc).
 *
 * need classes to drive the basic particle systems through scripted routine
 * such as explosion -> fire -> smoke -> smolder.
 *
 * explosion class, flare class, chaff class, etc.
 * white smoke, fire, black smoke, dust, water plume, splash, wingtip votices,
 * strake vortices, exhaust (w/thickness curve tied to throttle), contrails
 *
 * can we use a global particle system for all FX?  probably not a good idea
 * since it would be best to disable particle systems that are far from the
 * camera.
 *
 *
 * flare class: ***** several overlapping, emitting particles + halo? 
 * can probably implement as a subclass of particle.
 *
 */

# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#include <cassert>
# endif

#include "SmokeEffects.h"
#include "CSPSim.h"
#include "VirtualScene.h"

#include <osg/Geode>
#include <osg/Group>
#include <osgParticle/LinearInterpolator>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/SegmentPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/Operator>

#include <SimData/Random.h>

#include <cstdio>
#include <algorithm>


namespace fx {



void VortexExpander::operate(osgParticle::Particle *p, double dt)
{
	static int x = 0;
	if (p && isEnabled()) {
		float age = p->getAge();
		if (age > 1.0 && age < 2.0) {
			float dt = age-1.0;
			float r = 0.25+dt*dt;
			p->setSizeRange(osgParticle::rangef(0.0, r * 5.0 / age));
		} else
		if (age > 2.0) {
			float dt = age-2.0;
			float r = 1.25 + sqrt(dt);
			p->setSizeRange(osgParticle::rangef(0.0, r * 5.0 / age));
		}
		x++;
		if (age > 1.0 && (x % 300)==0) p->setLifeTime(age);
	}
}

void SmokeThinner::operate(osgParticle::Particle *p, double dt)
{
	float lifetime = p->getRadius();
	// check if we've already operated on this particle
	if (lifetime > 1000.0) {
		float x = simdata::g_Random.newRand();
		float old_lifetime = p->getLifeTime();
		// this needs to be made into a member variable, set by
		// a constructor argument
		int max = 10;
		x *= x;
		x *= x;
		// adjust the lifetime, weighted toward small t
		lifetime = x * x * (max-0.5) + 0.5;
		p->setLifeTime(lifetime);
		// mark this particle as "thinned"
		p->setRadius(1.0);
		// rescale the size range to match the new lifetime
		float scale = 1.0;
		if (old_lifetime > 0.0) scale = lifetime / (1.0 + old_lifetime); // can old_lifetime be < 1.0?
		osgParticle::rangef const &r = p->getSizeRange();
		p->setSizeRange(osgParticle::rangef(r.minimum, r.maximum * scale));
	} 
}



ParticleEffect::ParticleEffect() {
	m_Created = false;
	m_Parent = NULL;
	setDefault();
}

ParticleEffect::~ParticleEffect() {
	if (m_Created) {
		VirtualScene *scene = CSPSim::theSim->getScene();
		assert(scene);
		scene->removeParticleSystem(m_Geode.get(), m_Program.get());
	}
	if (m_Parent.valid()) {
		m_Parent->removeChild(m_Emitter.get());
	}
}

void ParticleEffect::setDefault() {
	m_Prototype.setShape(osgParticle::Particle::QUAD);
	m_Prototype.setLifeTime(1.0);
	m_Prototype.setAlphaRange(osgParticle::rangef(1,0));
	m_Prototype.setSizeRange(osgParticle::rangef(0.2f, 0.2f));
	m_Prototype.setPosition(osg::Vec3(0.0f,0.0f,0.0f));
	m_Prototype.setVelocity(osg::Vec3(0.0,0.0,0.0));
	m_Prototype.setColorRange(osgParticle::rangev4(osg::Vec4(1,1,1,1), osg::Vec4(1,1,1,1)));
	m_Prototype.setRadius(10000.0);
	m_Emissive = false;
	m_Light = true;
}

void ParticleEffect::setTexture(const std::string & TextureFile) { 
	m_TextureFile = TextureFile; 
}

void ParticleEffect::setColorRange(const osg::Vec4 &colorMin, const osg::Vec4 &colorMax) {
	m_Prototype.setColorRange(osgParticle::rangev4(colorMin, colorMax));
}

void ParticleEffect::setAlphaRange(float alpha_0, float alpha_1) {
	m_Prototype.setAlphaRange(osgParticle::rangef(alpha_0, alpha_1));
}

void ParticleEffect::setSizeRange(float size_0, float size_1) {
	m_Prototype.setSizeRange(osgParticle::rangef(size_0, size_1));
}

void ParticleEffect::setLifeTime(float t) {
	m_Prototype.setLifeTime(t);
}

void ParticleEffect::setShape(osgParticle::Particle::Shape const &shape) {
	m_Prototype.setShape(shape);
}

void ParticleEffect::setEmissive(bool emissive) { 
	m_Emissive = emissive; 
}

void ParticleEffect::setLight(bool light) { 
	m_Light = light; 
}

void ParticleEffect::setParent(osg::Group *parent) { 
	// cannot reassign after creation
	assert(!m_Created);
	m_Parent = parent;
}

void ParticleEffect::setEnabled(bool on) {
	if (m_Emitter.valid()) m_Emitter->setEnabled(on);
}

void ParticleEffect::create() {
	if (m_Created) return;

	setDefaultAttributes(m_TextureFile, m_Emissive, m_Light, 0);
	setDefaultParticleTemplate(m_Prototype);
	setFreezeOnCull(false);

	m_Emitter = new osgParticle::ModularEmitter;
	m_Emitter->setParticleSystem(this);
	m_Emitter->setPlacer(getPlacer());
	m_Emitter->setShooter(getShooter());
	m_Emitter->setCounter(getCounter());

	m_Program = new osgParticle::ModularProgram;
	m_Program->setParticleSystem(this);

	OperatorList::iterator iter;
	for (iter = m_Operators.begin(); iter != m_Operators.end(); iter++) {
		m_Program->addOperator(iter->get());
	}

	m_Geode = new osg::Geode;
	m_Geode->addDrawable(this);

	VirtualScene *scene = CSPSim::theSim->getScene();
	assert(scene);
	scene->addParticleSystem(m_Geode.get(), m_Program.get());

	if (m_Parent.valid()) {
		m_Parent->addChild(m_Emitter.get());
	} 

	m_Created = true;
}

void ParticleEffect::addOperator(osgParticle::Operator* op) {
	if (m_Created) {
		m_Program->addOperator(op);
	}
	m_Operators.push_back(op);
}

void ParticleEffect::removeOperator(osgParticle::Operator* op) {
	OperatorList::iterator at = std::find(m_Operators.begin(), m_Operators.end(), op);
	if (at != m_Operators.end()) {
		m_Operators.erase(at);
	}
	if (m_Created) {
		// XXX is there a simple way to remove an operator from the program
		//     with only the pointer?
		//m_Program->removeOperator(op);
	}
}


ParticleEffectUpdater::ParticleEffectUpdater() { 
	m_Enabled = false;
	m_InScene = false;
}

ParticleEffectUpdater::~ParticleEffectUpdater() { 
	destroy(); 
}


void ParticleEffectUpdater::addParticleSystem(ParticleEffect *effect) {
	effect->create();
	osgParticle::ParticleSystemUpdater::addParticleSystem(effect);
}

void ParticleEffectUpdater::setEnabled(bool on) {
	if (m_Enabled == on) return;
	m_Enabled = on;
	if (on && !m_InScene) {
		VirtualScene *scene = CSPSim::theSim->getScene();
		assert(scene);
		scene->addEffectUpdater(this);
		m_InScene = true;
	}
}

void ParticleEffectUpdater::destroy() {
	if (m_InScene) {
		VirtualScene *scene = CSPSim::theSim->getScene();
		if (scene) scene->removeEffectUpdater(this);
		m_InScene = false;
	}
}


SmokeTrail::SmokeTrail(): ParticleEffect() {
	m_Speed = 0;
}

SmokeTrail::~SmokeTrail() {
}

void SmokeTrail::setExpansion(float speed) {
	m_Speed = speed;
}

void SmokeTrail::setOffset(simdata::Vector3 const &offset) {
	m_Offset = offset;
}

osgParticle::Counter* SmokeTrail::getCounter() {
	if (!m_Counter) m_Counter = new osgParticle::RandomRateCounter();
	// XXX this should be adjustable, even if these are the defaults
	m_Counter->setRateRange(2000, 2400);
	return m_Counter.get();
}

osgParticle::Shooter* SmokeTrail::getShooter() {
	if (!m_Shooter) m_Shooter = new osgParticle::RadialShooter();
	// XXX this should be adjustable, even if these are the defaults
	m_Shooter->setPhiRange(-0.21f, 1.78f);
	m_Shooter->setThetaRange(0.0f, 1.57f);
	m_Shooter->setInitialSpeedRange(0, m_Speed);
	return m_Shooter.get();
}

osgParticle::Placer* SmokeTrail::getPlacer() {
	if (!m_Placer) m_Placer = new osgParticle::SegmentPlacer();
	return m_Placer.get();
}

inline osg::Vec3 toVec3(simdata::Vector3 const &v) {
	return osg::Vec3(v.x, v.y, v.z); 
}

void SmokeTrail::update(simdata::Vector3 const &motion, simdata::Quaternion const &attitude) {
	simdata::Vector3 lastbody = attitude.GetInverseRotated(m_LastPlace - motion);
	m_Placer->setVertexA(toVec3(m_Offset));
	m_Placer->setVertexB(toVec3(lastbody));
	m_LastPlace = attitude.GetRotated(m_Offset);
}

SmokeTrailSystem::SmokeTrailSystem() {
	m_Updater = NULL;
}

SmokeTrailSystem::~SmokeTrailSystem() {
}

void SmokeTrailSystem::addSmokeTrail(SmokeTrail *trail) {
	if (!m_Updater) {
		m_Updater = new ParticleEffectUpdater;
		m_Updater->setEnabled(true);
	}
	assert(trail);
	m_Trails.push_back(trail);
	m_Updater->addParticleSystem(trail);
}

void SmokeTrailSystem::setEnabled(bool on) {
	TrailList::iterator iter;
	for (iter = m_Trails.begin(); iter != m_Trails.end(); iter++) {
		(*iter)->setEnabled(on);
	}
}

void SmokeTrailSystem::update(simdata::Vector3 const &motion, simdata::Quaternion const &attitude) {
	TrailList::iterator iter;
	for (iter = m_Trails.begin(); iter != m_Trails.end(); iter++) {
		(*iter)->update(motion, attitude);
	}
}

} // fx

