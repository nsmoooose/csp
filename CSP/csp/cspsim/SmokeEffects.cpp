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
 * TODO
 *
 *    * Find the clipping bug.  It shows up when the view is close
 *      to the emitter, and only at particular angles.  I've tried
 *      turning culling off on every node I could think off, to no
 *      avail.  I also shifted the global origin to be very close
 *      to the eye point, thinking that is was simply a floating
 *      point precision problem, but that didn't help.
 *
 */

/*
 * FROM AN OLD TODO LIST (some of this may already be implementend, some
 * may now be irrelevant).
 *
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
#include "Atmosphere.h"

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

#include <csp/lib/util/Random.h>
#include <csp/lib/util/osg.h>

#include <cstdio>
#include <algorithm>


namespace fx {


class WindEmitter: public osgParticle::Emitter {
	Atmosphere const *m_Atmosphere;
	simdata::Vector3 m_VertexA;
	simdata::Vector3 m_VertexB;
	simdata::Vector3 m_WindA;
	simdata::Vector3 m_WindB;
	float m_Count;
	float m_Density;
	int m_Min, m_Max;
public:
	WindEmitter() {
		m_Atmosphere = CSPSim::theSim->getAtmosphere();
		m_Min = 1;
		m_Max = 100;
		m_Density = 1.0;
	}
	void setSource(simdata::Vector3 const &x) {
		m_VertexB = m_VertexA;
		m_VertexA = x;
		m_WindB = m_WindA;
		if (m_Atmosphere != 0) {
			m_WindA = m_Atmosphere->getWind(x);
		}
	}
	void setDensity(float density, int min, int max) {
		m_Density = density;
		m_Min = min;
		m_Max = max;
	}
	void emit(double dt) {
		simdata::Vector3 wind, d_wind;
		simdata::Vector3 place, d_place;
		float distance = (m_VertexB - m_VertexA).length();
		int count = int(distance * m_Density);
		if (count > m_Max) count = m_Max;
		d_wind = (m_WindB - m_WindA) / count;
		d_place = (m_WindB * dt + m_VertexB - m_VertexA) / count;
		wind = m_WindA;
		place = m_VertexA;
		osgParticle::rangev3 push;
		push.set(osg::Vec3(-0.4, -0.4, -0.4), osg::Vec3(0.4, 0.4, 0.4));
		for (int i = 0; i < count; ++i) {
			osgParticle::Particle *P = getParticleSystem()->createParticle(getUseDefaultTemplate()? 0: &getParticleTemplate());
			if (P) {
				P->setPosition(simdata::toOSG(place));
				P->setVelocity(simdata::toOSG(wind) + push.get_random());
				place += d_place;
				wind += d_wind;
				if (getReferenceFrame() == RELATIVE_RF) {
					P->transformPositionVelocity(getLocalToWorldMatrix());
					//P->transformPositionVelocity(getWorldToLocalMatrix());
				}
			}
		}
	}
};

class WindShooter: public osgParticle::RadialShooter {
	Atmosphere const *m_Atmosphere;
	simdata::Vector3 m_Wind;
	simdata::Vector3 m_LastWind;
	osgParticle::rangev3 m_WindRange;
public:
	WindShooter() {
		m_Atmosphere = CSPSim::theSim->getAtmosphere();
	}

	void updateWind(simdata::Vector3 const &position) {
		if (m_Atmosphere) {
			m_LastWind = m_Wind;
			m_Wind = m_Atmosphere->getWind(position);
			m_WindRange.set(simdata::toOSG(m_LastWind), simdata::toOSG(m_Wind));
		}
	}

	simdata::Vector3 const &getWind() { return m_Wind; }

	// XXX this is a bit slow (6 random generations per frame),
	// and still results in visible stepping of the smoke trail
	// during gusts.  not sure of a better approach though.
	inline void shoot(osgParticle::Particle *P) const {
	        float theta = getThetaRange().get_random();
		float phi = getPhiRange().get_random();
		float speed = getInitialSpeedRange().get_random();
		osg::Vec3 wind = m_WindRange.get_random();
		P->setVelocity(osg::Vec3(
			speed * sinf(theta) * cosf(phi),
			speed * sinf(theta) * sinf(phi),
			speed * cosf(theta)
			) + wind);
	}
};



//////////////////////////////////////////////////////////////////////////////////
// VortexExpander


void VortexExpander::operate(osgParticle::Particle *p, double /*dt*/)
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



//////////////////////////////////////////////////////////////////////////////////
// SmokeThinner


void SmokeThinner::operate(osgParticle::Particle *p, double /*dt*/)
{
	float radius = p->getRadius();
	// check if we've already operated on this particle
	if (radius > 1000.0) {
		float x = simdata::g_Random.unit();
		float old_lifetime = p->getLifeTime();
		// this needs to be made into a member variable, set by
		// a constructor argument
		int max = 10;
		x *= x;
		x *= x;
		// adjust the lifetime, weighted toward small t
		float lifetime = x * x * (max-0.5) + 0.5;
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



//////////////////////////////////////////////////////////////////////////////////
// ParticleEffect


ParticleEffect::ParticleEffect() {
	m_Created = false;
	setDefault();
}

ParticleEffect::~ParticleEffect() {
	if (m_Created) {
		CSPSim::theSim->getScene()->removeParticleSystem(m_Geode.get(), m_Program.get());
		CSPSim::theSim->getScene()->removeParticleEmitter(m_Emitter.get());
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

void ParticleEffect::setEnabled(bool on) {
	if (m_Emitter.valid()) m_Emitter->setEnabled(on);
}

osgParticle::Emitter* ParticleEffect::getEmitter() {
	osgParticle::ModularEmitter* emitter = new osgParticle::ModularEmitter;
	emitter->setParticleSystem(this);
	emitter->setPlacer(getPlacer());
	emitter->setShooter(getShooter());
	emitter->setCounter(getCounter());
	return emitter;
}

void ParticleEffect::create() {
	if (m_Created) return;

	setDefaultAttributes(m_TextureFile, m_Emissive, m_Light, 0);
	setDefaultParticleTemplate(m_Prototype);
	setDoublePassRendering(false);
	setFreezeOnCull(false);

	m_Emitter = getEmitter();

	m_Program = new osgParticle::ModularProgram;
	m_Program->setParticleSystem(this);

	OperatorList::iterator iter;
	for (iter = m_Operators.begin(); iter != m_Operators.end(); iter++) {
		m_Program->addOperator(iter->get());
	}

	m_Geode = new osg::Geode;
	m_Geode->addDrawable(this);

	CSPSim::theSim->getScene()->addParticleSystem(m_Geode.get(), m_Program.get());
	CSPSim::theSim->getScene()->addParticleEmitter(m_Emitter.get());

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



//////////////////////////////////////////////////////////////////////////////////
// ParticleEffectUpdater


ParticleEffectUpdater::ParticleEffectUpdater() {
	m_Enabled = false;
	m_InScene = false;
}

ParticleEffectUpdater::~ParticleEffectUpdater() {
}


void ParticleEffectUpdater::addParticleSystem(ParticleEffect *effect) {
	effect->create();
	osgParticle::ParticleSystemUpdater::addParticleSystem(effect);
	m_InScene = true;
}

// XXX not currently used...
void ParticleEffectUpdater::setEnabled(bool on) {
	if (m_Enabled == on) return;
	m_Enabled = on;
}

void ParticleEffectUpdater::destroy() {
	m_InScene = false;
}



//////////////////////////////////////////////////////////////////////////////////
// SmokeTrail


SmokeTrail::SmokeTrail(): ParticleEffect() {
	//m_Speed = 0;
}

SmokeTrail::~SmokeTrail() {
}

osgParticle::Emitter* SmokeTrail::getEmitter() {
	WindEmitter *emitter = new WindEmitter();
	//emitter->setDensity(20.0, 10, 1000);
	emitter->setDensity(4.0, 4, 8);
	emitter->setParticleSystem(this);
	return emitter;
}

void SmokeTrail::setOffset(simdata::Vector3 const &offset) {
	m_Offset = offset;
}


void SmokeTrail::setExpansion(float /*speed*/) {
//	m_Speed = speed;
}

/*
osgParticle::Counter* SmokeTrail::getCounter() {
	if (!m_Counter) m_Counter = new osgParticle::RandomRateCounter();
	// XXX this should be adjustable, even if these are the defaults
	m_Counter->setRateRange(2000, 2400);
	return m_Counter.get();
}

osgParticle::Shooter* SmokeTrail::getShooter() {
	if (!m_Shooter) m_Shooter = new WindShooter(); //osgParticle::RadialShooter();
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

void SmokeTrail::update(double dt, simdata::Vector3 const &position, simdata::Quat const &attitude) {
	simdata::Vector3 place = position + attitude.rotate(m_Offset);
	m_Placer->setVertexA(toOSG(place));
	m_Placer->setVertexB(toOSG(m_LastPlace+m_LastWind*dt));
	m_Shooter->updateWind(place);
	m_LastPlace = place;
	m_LastWind = m_Shooter->getWind();
}
*/

void SmokeTrail::update(double /*dt*/, simdata::Vector3 const &position, simdata::Quat const &attitude) {
	WindEmitter *emitter = dynamic_cast<WindEmitter*>(m_Emitter.get());
	if (emitter) {
		simdata::Vector3 place = position + attitude.rotate(m_Offset);
		emitter->setSource(place);
	}
}



//////////////////////////////////////////////////////////////////////////////////
// SmokeTrailSystem


SmokeTrailSystem::SmokeTrailSystem() {
	m_Updater = NULL;
}

SmokeTrailSystem::~SmokeTrailSystem() {
	if (m_Updater.valid()) {
		VirtualScene *scene = CSPSim::theSim->getScene();
		assert(scene != 0);
		scene->removeEffectUpdater(m_Updater.get());
	}
}

void SmokeTrailSystem::addSmokeTrail(SmokeTrail *trail) {
	if (!m_Updater) {
		m_Updater = new ParticleEffectUpdater;
		VirtualScene *scene = CSPSim::theSim->getScene();
		assert(scene != 0);
		scene->addEffectUpdater(m_Updater.get());
		m_Updater->setEnabled(true);
	}
	assert(trail);
	m_Trails.push_back(trail);
	m_Updater->addParticleSystem(trail);
}

void SmokeTrailSystem::setEnabled(bool on) {
	TrailList::iterator iter = m_Trails.begin();
	TrailList::const_iterator iEnd = m_Trails.end();
	for (; iter != iEnd; ++iter)
		(*iter)->setEnabled(on);
}

void SmokeTrailSystem::update(double dt, simdata::Vector3 const &position, simdata::Quat const &attitude) {
	TrailList::iterator iter = m_Trails.begin();
	TrailList::const_iterator iEnd = m_Trails.end();
	for (; iter != iEnd; ++iter)
		(*iter)->update(dt, position, attitude);
}

} // fx

