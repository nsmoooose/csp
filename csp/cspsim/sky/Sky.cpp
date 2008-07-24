// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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

#include <csp/cspsim/sky/Common.h>
#include <csp/cspsim/sky/OrbitalBodyModel.h>
#include <csp/cspsim/sky/Sky.h>
#include <csp/cspsim/sky/SkyDome.h>
#include <csp/cspsim/sky/SolarSystem.h>
#include <csp/cspsim/sky/StarDome.h>
#include <csp/csplib/util/osg.h>

#include <osg/Geode>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/MatrixTransform>

#include <iostream> //XXX

namespace csp {

Sky::Sky(double radius):
		m_Radius(radius),
		m_Latitude(0.0),
		m_Longitude(0.0),
		m_LastFullUpdate(0.0),
		m_LastMoonIntensity(-1) {
	m_Transform = new osg::MatrixTransform;
	m_Group = new osg::Group;
	m_ImposterGroup = new osg::Group;
	m_SolarSystem = new SolarSystem;
	m_SkyDome = new SkyDome(radius);
	m_StarDome = new StarDome(radius);
	// draw imposters first to occlude stars
	m_Transform->addChild(m_ImposterGroup.get());
	m_Transform->addChild(m_StarDome->makeGeode());
	// sky needs to be draw first
	m_Group->addChild(m_SkyDome->getDomeNode());
	m_Group->addChild(m_Transform.get());
}

Sky::~Sky() {
}

void Sky::setPosition(double latitude, double longitude) {
	m_Latitude = latitude;
	m_Longitude = longitude;
}

void Sky::addModel(OrbitalBodyModel *model) {
	assert(model);
	m_ImposterGroup->addChild(model->node());
	model->setSkyDomeRadius(m_Radius);
	m_Models.push_back(model);
	if (model->body() == m_SolarSystem->moon()) {
		m_Moon = model;
	}
}

osg::Group *Sky::group() {
	return m_Group.get();
}

void Sky::getSunPositionAndSkyMagnitude(double time, double &elevation, double &azimuth, double &magnitude) const {
	Vector3 earth = m_SolarSystem->earth()->getHeliocentricPosition(time);
	Vector3 earth_equatorial = OrbitalBodyModel::toEquatorial(earth);
	osg::Vec3 sun = equatorialToLocal(toOSG(-earth_equatorial));
	sun.normalize();
	elevation = asin(sun.z());
	azimuth = atan2(sun.y(), sun.x());
	//std::cout << "sun = " << toDegrees(elevation) << "\n";

	// ad-hoc sky light model.  the response is fairly flat above 30 degrees, at around -4.
	// at sunset, the magnitude is around -1, dropping to +2 at -8 degrees (twilight).
	// the brightness drops rapidly from there, and is capped at about +8 at -14 degrees.
	// the real sky (away from artificial lights) continues to darken until about -20 degrees,
	// but the current model is only concerned with the visibility of stars by eye (+6).
	//
	// todo: the moon position and phase should cap this value.  the viewer's altitude and
	// nearby light sources should also be considered.
	magnitude = 1.0 / sin(std::max((elevation + toRadians(22.0)), 0.14) * 0.5) - 6.0;
}

void Sky::update(double time) {
	double angle = -PI_2 - MeanSiderealTime(time, m_Longitude);
	m_Transform->setMatrix(osg::Matrixd::rotate(angle, 0, 0, 1) * osg::Matrixd::rotate(m_Latitude - PI_2, 1, 0, 0));
	const bool first_update = (m_LastFullUpdate == 0.0);
	if (time - m_LastFullUpdate > (10.0 / 86400.0) || first_update) {
		m_LastFullUpdate = time;
		m_SolarSystem->update(time);

		double elevation = 0.0;
		double azimuth = 0.0;
		double magnitude = 0.0;
		getSunPositionAndSkyMagnitude(time, elevation, azimuth, magnitude);

		for (unsigned i = 0; i < m_Models.size(); ++i) {
			m_Models[i]->update(time, magnitude, m_Transform->getMatrix());
		}
		if (m_SkyDome.valid()) m_SkyDome->setSunPosition(azimuth, elevation);
		if (m_StarDome.valid()) m_StarDome->updateLighting(magnitude);
		updateMoonlight(magnitude);
	}
	// force the sky shading to run to completion on the first update so that
	// the sky is colored correctly when the simulation starts.  afterward,
	// shading updates computed over multiple frames.
	const bool force_update = first_update;
	if (m_SkyDome.valid()) m_SkyDome->updateShading(force_update);
}

osg::Vec3 Sky::equatorialToLocal(osg::Vec3 const &vec) const {
	return m_Transform->getMatrix().preMult(vec);
}

void Sky::addSunlight(int num) {
	m_SkyDome->initSunlight(num);
	m_Sunlight = new osg::LightSource;
	m_Sunlight->setLight(m_SkyDome->getSunlight());
}

osg::LightSource *Sky::getSunlight() {
	return m_Sunlight.get();
}

void Sky::addMoonlight(int num) {
	assert(!m_Moonlight);
	if (!m_Moonlight) {
		m_Moonlight = new osg::LightSource;
		m_Moonlight->getLight()->setLightNum(num);
		m_Moonlight->getLight()->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	}
}

osg::LightSource *Sky::getMoonlight() {
	return m_Moonlight.get();
}


void Sky::updateMoonlight(double sky_magnitude) {
	if (m_Moonlight.valid() && m_Moon.valid()) {
		double azimuth = m_Moon->getAzimuth();
		double elevation = m_Moon->getElevation();
		double omega = m_Moon->getPhaseAngle();
		double sin_omega_2 = sin(0.5 * omega);
		double cos_omega_2 = cos(0.5 * omega);
		double lommel_seeler = 0.0;
		if (sin_omega_2 == 0.0) {
			lommel_seeler = 1.0;
		} else if (cos_omega_2 != 0.0) {
			lommel_seeler = 1.0 - sin_omega_2 * sin_omega_2 / cos_omega_2 * ::log(1.0 / tan(0.25 * omega));
		}
		// todo: earthshine irradiance peaks (at new moon) at about 9% of sunshine intensity (at full moon).
		// todo: distance attenuation
		//double distance = m_Moon->getEarthDistance();

		double attenuation = m_Moon->getAtmosphericAttenuation();  // atmospheric attenuation
		attenuation *= clampTo(toDegrees(elevation) + 0.5, 0.0, 1.0);  // block at horizon
		attenuation /= (1.0 +  pow(10.0, 2.0 - sky_magnitude));  // fade relative to sunlight
		// (0.1 at skymag +1, 0.5 at skymag +2, 0.9 at skymag +3)
	
		static const double peakIntensity = 128.0;  // ad-hoc
		int intensity = static_cast<int>(peakIntensity * lommel_seeler * attenuation);
		//std::cout << "moon intensity: " << intensity << ", ls: " << lommel_seeler << ", atten: " << attenuation << "\n";
		if (intensity != m_LastMoonIntensity) {
			m_LastMoonIntensity = intensity;
			//std::cout << "moon: " << attenuation << " " << lommel_seeler << "\n";
			static const osg::Vec3 color(0.92, 0.98, 1.0);
			m_Moonlight->getLight()->setAmbient(osg::Vec4(color * 0.00, 1.0));
			m_Moonlight->getLight()->setDiffuse(osg::Vec4(color * intensity / 255.0, 1.0));
			m_Moonlight->getLight()->setSpecular(osg::Vec4(color * intensity / 255.0, 1.0));
		}

		// it would be somewhat simpler to bind the moon lightsource to the moon model
		// using OrbitalBodyModel::bindLightSource, but this is inconvenient when the
		// scene is split into multiple views (e.g., CSP uses a very far view for the
		// sky, and far and near views for other scene elements).  instead, the light
		// position and direction is updated directly here so that the lightsource can
		// be placed at the top of the scene graph for each of the views.
		//
		// note that the sunlight is updated in a similar way by SkyDome; perhaps this
		// should be moved to Sky as well.
		const double cos_elevation = cos(elevation);
		const double sin_elevation = sin(elevation);
		osg::Vec3 dir(cos_elevation * cos(azimuth), cos_elevation * sin(azimuth), sin_elevation);
		m_Moonlight->getLight()->setPosition(osg::Vec4(dir, 0.0));
		m_Moonlight->getLight()->setDirection(-dir);
		//std::cout << "moon position, direction: " << dir.x() << " " << dir.y() << " " << dir.z() << "\n";
	}
}

} // namespace csp

