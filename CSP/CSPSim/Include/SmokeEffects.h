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
 * @file SmokeEffects.h
 *
 **/


#ifndef __SMOKEEFFECTS_H__
#define __SMOKEEFFECTS_H__

#include <osg/ref_ptr>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/Operator>

#include <SimData/Quat.h>
#include <SimData/Vector3.h>

#include <vector>
#include <string>

using std::string;


namespace osg {
	class Geode;
	class Group;
}

namespace osgParticle {
	class Particle;
	class ModularEmitter;
	class ModularProgram;
	class SegmentPlacer;
	class RadialShooter;
	class RandomRateCounter;
	class Placer;
	class Counter;
	class Shooter;
	class Emitter;
}

namespace fx {


class WindShooter;
class WindEmitter;

	/*
class SmokeSegments {
	typedef std::vector<osg::ref_ptr<osgParticle::SegmentPlacer> > PlacerArray;
	PlacerArray m_Placers;
	std::vector<simdata::Vector3> m_Sources;
	std::vector<simdata::Vector3> m_LastPlace;
public:
	SmokeSegments() {
	}
	~SmokeSegments() {
	}
	int addSource(simdata::Vector3 const &v);
	osgParticle::SegmentPlacer *getSegment(int i);
	void update(simdata::Vector3 const &motion, simdata::Quat const &attitude);
};
*/


class VortexExpander: public osgParticle::Operator
{
public:
	META_Object(csp, VortexExpander);
	VortexExpander() {}
	VortexExpander(const VortexExpander &copy, const osg::CopyOp &copyop): Operator(copy, copyop) {}
	virtual void operate(osgParticle::Particle *p, double dt);
};


class SmokeThinner: public osgParticle::Operator
{
public:
	META_Object(csp, SmokeThinner);
	SmokeThinner() {}
	SmokeThinner(const SmokeThinner &copy, const osg::CopyOp &copyop): Operator(copy, copyop) {}
	virtual void operate(osgParticle::Particle *p, double dt);
	int m_X;
};


class ParticleEffect: public osgParticle::ParticleSystem {
	friend class ParticleEffectUpdater;
public:
	ParticleEffect();
	virtual ~ParticleEffect();

	virtual void setDefault();
	virtual void setTexture(const std::string & TextureFile);
	virtual void setColorRange(const osg::Vec4 &colorMin, const osg::Vec4 &colorMax);
	virtual void setAlphaRange(float alpha_0, float alpha_1);
	virtual void setSizeRange(float size_0, float size_1);
	virtual void setLifeTime(float t);
	virtual void setShape(osgParticle::Particle::Shape const &shape);
	virtual void setEmissive(bool emissive);
	virtual void setLight(bool light);
	virtual void addOperator(osgParticle::Operator* op);
	virtual void removeOperator(osgParticle::Operator* op);
	virtual void setEnabled(bool on);

protected:

	virtual osgParticle::Counter* getCounter() { return NULL; }
	virtual osgParticle::Shooter* getShooter() { return NULL; }
	virtual osgParticle::Placer* getPlacer()  { return NULL; } 
	virtual osgParticle::Emitter* getEmitter();
	virtual void create();

	typedef std::vector<osg::ref_ptr<osgParticle::Operator> > OperatorList;

	OperatorList m_Operators;

	osgParticle::Particle m_Prototype;
	osg::ref_ptr<osgParticle::Emitter> m_Emitter;
	osg::ref_ptr<osgParticle::ModularProgram> m_Program;
	osg::ref_ptr<osg::Geode> m_Geode;

	std::string m_TextureFile;
	bool m_Emissive;
	bool m_Light;
	bool m_Created;
};


class SmokeTrail: public ParticleEffect {
public:
	SmokeTrail();
	virtual ~SmokeTrail();

	void setExpansion(float speed);
	void setOffset(simdata::Vector3 const &offset);
	void update(double dt, simdata::Vector3 const &motion, simdata::Quat const &attitude);

protected:

	virtual osgParticle::Emitter* getEmitter();
	/*
	virtual osgParticle::Counter* getCounter();
	virtual osgParticle::Shooter* getShooter();
	virtual osgParticle::Placer* getPlacer();

	osg::ref_ptr<osgParticle::RandomRateCounter> m_Counter;
	osg::ref_ptr<osgParticle::SegmentPlacer> m_Placer;
	osg::ref_ptr<WindShooter> m_Shooter;
	*/

	simdata::Vector3 m_Offset;
	//simdata::Vector3 m_LastPlace;
	//simdata::Vector3 m_LastWind;
	//float m_Speed;
};


class ParticleEffectUpdater: public osgParticle::ParticleSystemUpdater {
public:
	ParticleEffectUpdater();
	virtual ~ParticleEffectUpdater();

	// XXX note that this is NOT a virtual method!
	void addParticleSystem(ParticleEffect *effect);

	void setEnabled(bool);
	void destroy();

protected:
	bool m_Enabled;
	bool m_InScene;
};


class SmokeTrailSystem: public osg::Referenced {
public:
	SmokeTrailSystem();
	virtual void addSmokeTrail(SmokeTrail *);
	virtual void update(double dt, simdata::Vector3 const &motion, simdata::Quat const &attitude);
	virtual void setEnabled(bool);
protected:
	virtual ~SmokeTrailSystem();
	typedef std::vector< osg::ref_ptr<SmokeTrail> > TrailList;
	TrailList m_Trails;
	osg::ref_ptr<ParticleEffectUpdater> m_Updater;
};


} // fx 


#endif // __SMOKEEFFECTS_H__

