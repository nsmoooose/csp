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
 * need to clean up the BaseSystem interface, and allow multiple operators
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

# if defined(_MSC_VER) && (_MSC_VER <= 1200)
#include <assert.h>
# endif

#include "SmokeEffects.h"

#include <osg/Transform>
#include <osgParticle/LinearInterpolator>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/SegmentPlacer>
#include <osgParticle/RadialShooter>

#include <cstdio>
#include <SimData/Random.h>


namespace fx {
namespace smoke {

int SmokeSegments::addSource(simdata::Vector3 const &v) {
	m_Sources.push_back(v);
	m_LastPlace.push_back(simdata::Vector3());
	m_Placers.push_back(new osgParticle::SegmentPlacer);
	return m_Sources.size()-1;
}

osgParticle::SegmentPlacer *SmokeSegments::getSegment(int i) { return m_Placers[i].get(); }

inline osg::Vec3 toVec3(simdata::Vector3 const &v) {
	return osg::Vec3(v.x, v.y, v.z); 
}

void SmokeSegments::update(simdata::Vector3 const &motion, simdata::Matrix3 const &to_scene, simdata::Matrix3 const &to_body) {
	size_t i, n;
	n = m_Placers.size();
	std::vector<simdata::Vector3>::iterator source = m_Sources.begin();
	std::vector<simdata::Vector3>::iterator lastplace = m_LastPlace.begin();

	for (i = 0; i < n; i++) {
		simdata::Vector3 lastbody = to_body * (*lastplace - motion);
		m_Placers[i]->setVertexA(toVec3(*source));
		m_Placers[i]->setVertexB(toVec3(lastbody));
		*lastplace = to_scene * *source;
		lastplace++;
		source++;
	}
}

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


BaseSystem::BaseSystem() {
	setDefault();
}

BaseSystem::~BaseSystem() {}

void BaseSystem::setDefault() {
	m_Prototype.setShape(osgParticle::Particle::QUAD);
	m_Prototype.setLifeTime(1.0);
	m_Prototype.setAlphaRange(osgParticle::rangef(1,0));
	m_Prototype.setSizeRange(osgParticle::rangef(0.2, 0.2));
	m_Prototype.setPosition(osg::Vec3(0,0,0));
	m_Prototype.setVelocity(osg::Vec3(0,0,0));
	m_Prototype.setColorRange(osgParticle::rangev4(osg::Vec4(1,1,1,1), osg::Vec4(1,1,1,1)));
	m_Prototype.setRadius(10000.0);
	m_Emissive = false;
	m_Light = false;
}

void BaseSystem::setTexture(const std::string & TextureFile) { m_TextureFile = TextureFile; }

void BaseSystem::setColorRange(const osg::Vec4 &colorMin, const osg::Vec4 &colorMax) {
	m_Prototype.setColorRange(osgParticle::rangev4(colorMin, colorMax));
}

void BaseSystem::setAlphaRange(float alpha_0, float alpha_1) {
	m_Prototype.setAlphaRange(osgParticle::rangef(alpha_0, alpha_1));
}

void BaseSystem::setSizeRange(float size_0, float size_1) {
	m_Prototype.setSizeRange(osgParticle::rangef(size_0, size_1));
}

void BaseSystem::setLifeTime(float t) {
	m_Prototype.setLifeTime(t);
}

void BaseSystem::setShape(osgParticle::Particle::Shape const &shape) {
	m_Prototype.setShape(shape);
}

void BaseSystem::setEmissive(bool emissive) { m_Emissive = emissive; }
void BaseSystem::setLight(bool light) { m_Light = light; }

void BaseSystem::setCounter(osgParticle::Counter *counter) {
	m_UserCounter = counter;
}

osgParticle::Counter *BaseSystem::getCounter() {
	if (!m_UserCounter) return NULL;
	return m_UserCounter.get();
}

void BaseSystem::setShooter(osgParticle::Shooter *shooter) {
	m_UserShooter = shooter;
}

osgParticle::Shooter *BaseSystem::getShooter() {
	if (!m_UserShooter) return NULL;
	return m_UserShooter.get();
}

void BaseSystem::setPlacer(osgParticle::Placer *placer) {
	m_UserPlacer = placer;
}

osgParticle::Placer *BaseSystem::getPlacer() {
	if (!m_UserPlacer) return NULL;
	return m_UserPlacer.get();
}

void BaseSystem::setOperator(osgParticle::Operator *op) {
	m_UserOperator = op;
}

osgParticle::Operator *BaseSystem::getOperator() { 
	if (!m_UserOperator) return NULL;
	return m_UserOperator.get();
}

osgParticle::ModularEmitter *BaseSystem::create(osg::Transform *base, 
					    osgParticle::ParticleSystemUpdater *&psu) {
	osgParticle::ParticleSystem *ps = new osgParticle::ParticleSystem;
	ps->setDefaultAttributes(m_TextureFile, m_Emissive, m_Light, 0);
	ps->setDefaultParticleTemplate(m_Prototype);
	ps->setFreezeOnCull(false);
	osgParticle::ModularEmitter *me = new osgParticle::ModularEmitter;
	me->setParticleSystem(ps);
	m_Placer = getPlacer();
	assert(m_Placer.valid());
	me->setPlacer(m_Placer.get());
	m_Counter = getCounter();
	assert(m_Counter.valid());
	me->setCounter(m_Counter.get());
	m_Shooter = getShooter();
	assert(m_Shooter.valid());
	me->setShooter(m_Shooter.get());
	base->addChild(me);
	osgParticle::ModularProgram *mp = new osgParticle::ModularProgram;
	mp->setParticleSystem(ps);
	// need a better setup to allow multiple operators
	m_Operator = getOperator();
	if (m_Operator.valid()) {
		mp->addOperator(m_Operator.get());
	}
	base->addChild(mp);
	if (!psu) {
		psu = new osgParticle::ParticleSystemUpdater;
	}
	psu->addParticleSystem(ps);
	return me;
}

void Thinner::operate(osgParticle::Particle *p, double dt)
{
	float lifetime = p->getRadius();
	// check if we've already operated on this particle
	if (lifetime > 1000.0) {
		float x = simdata::g_Random.NewRand();
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
		if (old_lifetime > 0.0) scale = lifetime / old_lifetime;
		osgParticle::rangef const &r = p->getSizeRange();
		p->setSizeRange(osgParticle::rangef(r.minimum, r.maximum * scale));
	} 
}

osgParticle::Shooter *Trail::getShooter() {
	osgParticle::Shooter *user = BaseSystem::getShooter();
	if (user) return user;
	osgParticle::RadialShooter *rs = new osgParticle::RadialShooter;
	rs->setPhiRange(-0.21, 1.78);
	rs->setThetaRange(0.0, 1.57);
	rs->setInitialSpeedRange(0, m_Speed);
	return rs;
}

osgParticle::Counter *Trail::getCounter() {
	osgParticle::Counter *user = BaseSystem::getCounter();
	if (user) return user;
	osgParticle::RandomRateCounter *rrc = new osgParticle::RandomRateCounter;
	// this should be adjustable, even if these are the defaults
	rrc->setRateRange(2000, 2400);
	return rrc;
}

void Trail::setExpansion(float speed) {
	m_Speed = speed;
}


} // smoke
} // fx

