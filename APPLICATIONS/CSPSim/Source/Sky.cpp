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
 * @file Sky.cpp
 *
 **/

#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include "Sky.h"
#include "Tools.h"
#include "CSPSim.h"
#include "Config.h"
#include "Colorspace.h"

#include <cmath>

#include <SimData/Date.h>
#include <SimData/Path.h>
#include <SimData/Object.h>
#include <SimData/InterfaceRegistry.h>
#include <SimData/DataArchive.h>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Fog>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/Light>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/ColorMatrix>
#include <osg/AlphaFunc>
#include <osg/Billboard>


using namespace osg;

const float TURBIDITY = 4.0f;

/*
 * TODO
 *
 * tie global colormatrix to light level
 * fix moon light
 * accelerate skyshade code for fixed sun position
 * add earthshine effects
 * organize/cleanup code
 * dim the stars when the moon is full
 * 
 */


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


/*
 * based on a model by Preetham, Shirley, and Smits.
 * see http://www.cs.utah.edu/vissim/papers/sunsky/ for more info
 *
 */


class StarCatalog: public simdata::Object {

public:
	struct Star {
		float Ra;
		float De;
		float I;
		float r;
		float g;
		float b;
		void pack(simdata::Packer &p) const {
			p.pack(Ra);
			p.pack(De);
			p.pack(I);
			p.pack(r);
			p.pack(g);
			p.pack(b);
		}
		void unpack(simdata::UnPacker &p) {
			p.unpack(Ra);
			p.unpack(De);
			p.unpack(I);
			p.unpack(r);
			p.unpack(g);
			p.unpack(b);
		}
		bool parse(StringTokenizer &t) {
			if (t.size() < 6) return false;
			Ra = atof(t.front().c_str());
			t.pop_front();
			De = atof(t.front().c_str());
			t.pop_front();
			I = atof(t.front().c_str());
			t.pop_front();
			r = atof(t.front().c_str());
			t.pop_front();
			g = atof(t.front().c_str());
			t.pop_front();
			b = atof(t.front().c_str());
			t.pop_front();
			return true;
		}
	};

	SIMDATA_OBJECT(StarCatalog, 0, 0)
	BEGIN_SIMDATA_XML_INTERFACE(StarCatalog)
		SIMDATA_XML("source", StarCatalog::m_Source, false)
		SIMDATA_XML("magnitude_cutoff", StarCatalog::m_MagnitudeCutoff, false)
	END_SIMDATA_XML_INTERFACE

	virtual void pack(simdata::Packer &p) const {
		Object::pack(p);
		p.pack(m_Source);
		p.pack(m_MagnitudeCutoff);
		p.pack((int)_stars.size());
		std::vector<Star>::const_iterator i;
		for (i = _stars.begin(); i != _stars.end(); i++) {
			i->pack(p);
		}
	}
	virtual void unpack(simdata::UnPacker &p) {
		Object::unpack(p);
		p.unpack(m_Source);
		p.unpack(m_MagnitudeCutoff);
		int n;
		p.unpack(n);
		_stars.resize(n);
		std::vector<Star>::iterator i;
		for (i = _stars.begin(); i != _stars.end(); i++) {
			i->unpack(p);
		}
	}
	virtual void parseXML(const char *cdata) {
		StringTokenizer t(cdata, " \t\n\r");
		Star s;
		_stars.clear();
		while (s.parse(t)) {
			_stars.push_back(s);
		}
	}

	std::string m_Source;
	float m_MagnitudeCutoff;
	std::vector<Star> _stars;
};

SIMDATA_REGISTER_INTERFACE(StarCatalog)


class StarSystem: public osg::Drawable {
public:
	META_Object(csp, StarSystem);

	StarSystem(): osg::Drawable() {
		rx = rz = 0.0;
		simdata::DataArchive *archive = CSPSim::theSim->getDataArchive();	
		assert(archive);
		_catalog = archive->getObject("environment.stars");
		assert(_catalog.valid());
		int n = _catalog->_stars.size();
		std::cout << "Using " << _catalog->m_Source << std::endl;
		std::cout << "Magnitude " << _catalog->m_MagnitudeCutoff << " cutoff, " << n << " stars\n";
		_positionList = new Vec3Array(n);
		_colorList = new Vec4Array(n);
		Vec3Array &p = *(_positionList.get());
		Vec4Array &c = *(_colorList.get());
		int i;
		// FIXME this all needs *extensive* cleaning.  just hacking for now...
		float R = 1000000.0;
		float phi, theta;
		for (i=0; i<n; i++) {
			float I = _catalog->_stars[i].I;
			// C is used to reduce star color saturation as an 
			// ad hoc approximation to human color vision response
			// at low light levels
			float C = I - 0.8;
			if (C > 1.0) C = 1.0;
			if (C < 0.0) C = 0.0;
// further desaturation...
//			C *= C;
//			C *= C;
			if (I > 1.0) I = 1.0;
			c[i][0] = _catalog->_stars[i].r;
			c[i][1] = _catalog->_stars[i].g;
			c[i][2] = _catalog->_stars[i].b;
			c[i][3] = I;
			float avg = (0.213*c[i][0]+0.715*c[i][1]+0.072*c[i][2]);
			c[i][0] = c[i][0] + (1.0-C) * (avg-c[i][0]);
			c[i][1] = c[i][1] + (1.0-C) * (avg-c[i][1]);
			c[i][2] = c[i][2] + (1.0-C) * (avg-c[i][2]);
			avg = (0.213*c[i][0]+0.715*c[i][1]+0.072*c[i][2]);
			// this increases the apperent intensity of stongly colored
			// stars by reducing saturation (also ad hoc).
			if (avg > 0.0) {
				c[i][0] /= avg;
				c[i][1] /= avg;
				c[i][2] /= avg;
				if (c[i][0] > 1.0) c[i][0] = 1.0;
				if (c[i][1] > 1.0) c[i][1] = 1.0;
				if (c[i][2] > 1.0) c[i][2] = 1.0;
			}
			phi = _catalog->_stars[i].Ra;
			theta = _catalog->_stars[i].De;
			p[i][0] = R*cos(phi)*cos(theta);
			p[i][1] = R*sin(phi)*cos(theta);
			p[i][2] = R*sin(theta);
		}
	}
	StarSystem(const StarSystem &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY): osg::Drawable(copy, copyop) {
	}

	bool computeBound() const {
		_bbox._min = Vec3(-1000100.0, -1000100.0, 0.0);
		_bbox._max = Vec3( 1000100.0,  1000100.0, 1000100.0);
		return true;
	}

	//const bool computeBound() const { return true; }
	virtual void drawImplementation(osg::State &state) const {
		int i = _positionList->size();
		Vec3Array const &p = *(_positionList.get());
		Vec4Array const &c = *(_colorList.get());
		glPushMatrix();
		glRotatef(rx, 1, 0, 0);
		glRotatef(rz, 0, 0, 1);
		glBegin(GL_POINTS);
		glEnable(GL_BLEND);
		while (i-- > 0) {
			glColor4f(c[i][0], c[i][1], c[i][2], c[i][3]*alpha);
			glVertex3f(p[i][0], p[i][1], p[i][2]);
		}
		glEnd();
		glPopMatrix();
	}

	void setObserver(double lat, double lmst, float ambient) {
		rz = 270.0 - lmst * R2D;
		rx = - 90.0 + lat * R2D;
		if (ambient < 0.0) ambient = 0.0;
		if (ambient > 1.0) ambient = 1.0;
		alpha = 1.0 - ambient;
		alpha *= alpha * alpha;
		dirtyDisplayList();
	}

private:
	ref_ptr<Vec3Array> _positionList;
	ref_ptr<Vec4Array> _colorList;
	simdata::Pointer<StarCatalog> _catalog;
	double rx, rz;
	double alpha;
};


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

const double AstronomicalBody::_earth_radius = 6370000.0;
const double AstronomicalBody::_earth_tilt = 23.4397 * D2R;
const double AstronomicalBody::_cos_earth_tilt = cos(_earth_tilt);
const double AstronomicalBody::_sin_earth_tilt = sin(_earth_tilt);
const double AstronomicalBody::_epoch = simdata::SimDate(2000,1,1,0,0,0).getJulianDate();

AstronomicalBody::AstronomicalBody() {
	_alpha = _beta = _delta = _lambda = _pi = 1.0;
	_radius = 1.0;
	_stale = true;
}

double AstronomicalBody::refract(double h) const {
	double f = 0.000305 / (0.000305 + h * h); // localize effect at horizon (semi-arbitrary approx)
	return h + 0.57 * f * D2R;
}

void AstronomicalBody::updateEquatorial() const {
	_alpha = atan2(sin(_lambda)*_cos_earth_tilt - tan(_beta)*_sin_earth_tilt, cos(_lambda));
	_delta = asin(sin(_beta)*_cos_earth_tilt + cos(_beta)*sin(_lambda)*_sin_earth_tilt);
	_distance = _earth_radius / _pi;
	// apparent angular radius from earth's surface
	_angular_radius = 6.2832 * _radius / (_distance - _earth_radius);
	_stale = false;
}

void AstronomicalBody::toObserver(double lat, double lmst, double &h, double &A) const {
	if (_stale) updateEquatorial();	
	double H = lmst - _alpha;
	h = asin(sin(lat)*sin(_delta) + cos(lat)*cos(_delta)*cos(H));
	A = atan2(sin(H), cos(H)*sin(lat) - tan(_delta)*cos(lat));
}

void AstronomicalBody::getEliptic(double &beta, double &lambda) const {
	beta = _beta;
	lambda = _lambda;
}

void AstronomicalBody::getEquatorial(double &delta, double &alpha) const {
	if (_stale) updateEquatorial();	
	delta = _delta;
	alpha = _alpha;
}

void AstronomicalBody::initLight(int n) {
	assert(!m_Light);
	m_LightNum = n;
	m_Light = new osg::Light;
	m_Light->setLightNum(n);
	m_Light->setAmbient(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	m_Light->setDiffuse(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	m_Light->setSpecular(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
}
	

Sun::Sun(): AstronomicalBody() {
	_radius = 695508000.0;
}

void Sun::updateScene(double h, double A, Color const &color, float intensity, float background) {
	float x, y, z;
	// sun direction
	z = sin(h);
	y = -cos(h);
	x = y*sin(A);
	y *= cos(A);
	_updateLighting(x, y, z, h, color, intensity, background);
}

void Sun::_updateLighting(float x, float y, float z, float h, Color const &color, float intensity, float background) {
	float specular_scale = 1.0;
	float ambient = background;
	if (ambient > 0.1) ambient = 0.1;

/* the first calculation of scale is somewhat physically based on
   a plot of sunlight intensity versus angle in Preetham.  the
   ad-hoc one below seems better for compensating the limited
   dynamic range of the monitor though, so this is currently
   disabled.  also, it has not been tested or tweaked.
	float scale = 1.0;
	{
		// angle from zenith
		float a = 1.0 - h / 0.5*G_PI;
		a *= a;
		scale = 1.0 - a*a;
		float horizon_light = intensity*intensity * 25.0;
		if (horizon_light > 1.0) horizon_light = 1.0;
		if (horizon_light > scale) scale = horizon_light;
	}
*/
	// ad-hoc additional darkening once the sun has set... the gl light
	// representing the sun should not be as bright as the horizion.
	float scale = intensity * intensity * 25.0;
	if (scale > 1.0) scale = 1.0;
	float light_r = color.getA() * scale;
	float light_g = color.getB() * scale;
	float light_b = color.getC() * scale;

	// below horizon?
	if (z < 0.0) {
		// fade out specular faster as sun drops below the horizon
		specular_scale = 1.0 + 9.0*z;  
		if (specular_scale < 0.0) specular_scale = 0.0;
		// get the "sun color" from the horizion
		z = 0.0;
	}

	// sunlight direction (if using spot)
	m_Light->setDirection(osg::Vec3(-x, -y, -z));
	
	// place the sun far from the scene
	m_Light->setPosition(osg::Vec4(x*1e+6, y*1e+6, z*1e+6, 1.0));

	// set the various light components
	m_Light->setAmbient(osg::Vec4(0.3f * light_r + ambient,
	                              0.3f * light_g + ambient,
	                              0.3f * light_b + ambient,
	                              1.0f));
	m_Light->setDiffuse(osg::Vec4(light_r,light_g,light_b,1.0f));
	m_Light->setSpecular(osg::Vec4(specular_scale*light_r,
	                               specular_scale*light_g,
	                               specular_scale*light_b,
	                               1.0f));
}

void Sun::updatePosition(double julian_date) {
	double dt = julian_date - getEpoch();
	double M = -3.18 + 0.98560 * dt;
	double L = M - 77.11;
	M = M * D2R;
	double C = 1.915 * sin(M) + 0.0201 * sin(2*M);
	// in AU = 1.496x10^11 m (23455 R_earth) 
	// double R = 1.00140 - 0.016708*cos(M) - 0.000141*cos(2*M) 
	_beta = 0.0;
	_lambda = (L + C) * D2R;
	_pi = 0.00004263; // just taking an average orbital radius of the earth
	_stale = true;
}



void Moon::updatePosition(double julian_date) {
	double dt = julian_date - getEpoch();
        double T = dt * 2.73785078713e-5; // julian centuries

	double l = 3.8104 + 8399.7091 * T;
	double m = 6.2300 + 628.3019 * T;
	double f = 1.6280 + 8433.4663 * T;
	double mp = 2.3554 + 8328.6911 * T;
	double d = 5.1985 + 7771.3772 * T;

	_lambda = \
		l
		+ 0.1098*sin(mp) 
		+ 0.0222*sin(2*d-mp) 
		+ 0.0115*sin(2*d) 
		+ 0.0037*sin(2*mp) 
		- 0.0032*sin(m) 
		- 0.0020*sin(2*f) 
		+ 0.0010*sin(2*d-2*mp) 
		+ 0.0010*sin(2*d-m-mp) 
		+ 0.0009*sin(2*d+mp) 
		+ 0.0008*sin(2*d-m) 
		+ 0.0007*sin(mp-m) 
		- 0.0006*sin(d) 
		- 0.0005*sin(m+mp);
	_beta = \
		+ 0.0895*sin(f) 
		+ 0.0049*sin(mp+f) 
		+ 0.0048*sin(mp-f) 
		+ 0.0030*sin(2*d-f) 
		+ 0.0010*sin(2*d+f-mp) 
		+ 0.0008*sin(2*d-f-mp) 
		+ 0.0006*sin(2*d+f);
	/*
	_pi = \
		+ 0.016593 
		+ 0.000904 * cos(mp) 
		+ 0.000166 * cos(2*d - mp) 
		+ 0.000137 * cos(2*d) 
		+ 0.000049 * cos(2*mp) 
		+ 0.000015 * cos(2*d + mp) 
		+ 0.000009 * cos(2*d - m);
	*/
	// save some time: for now no distance scaling variation
	_pi = 0.016593;
	_stale = true;
}

bool AstroBillboard::computeMatrix(Matrix& modelview, const Vec3& eye_local, const Vec3& pos_local) const {
	Matrix matrix;
	Vec3 to_eye(eye_local-pos_local);
	Vec3 to_eye_projection = to_eye^_axis;
	if (to_eye_projection.normalize() > 0.0) {
		float rotation_angle = G_PI+acos(to_eye_projection*_onormal);
		Vec3 pivot = _onormal^to_eye_projection;
		matrix.makeRotate(rotation_angle, pivot);
	}
	matrix = _orient * matrix;
	matrix.setTrans(pos_local);
	modelview.preMult(matrix);
	return true;
}

void AstroBillboard::setAxes(const Vec3& axis, const Vec3 &up, const Vec3 &normal) {
	_axis = axis;
	_axis.normalize();
	updateCache();
	_normal = normal;
	_normal.normalize();
	Vec3 up_ = up;
	up_.normalize();
	_normal = up_ ^ (_normal ^ up_);
	Vec3 rot = _axis ^ up_;
	float angle = acos(up_*_axis);
	_orient.makeRotate(-angle, rot);
	Matrix inv;
	inv.makeRotate(angle, rot);
	_normal = inv * _normal;
	_onormal = _normal ^ _axis;
	_onormal.normalize();
}


Moon::Moon(): AstronomicalBody() {
	_radius = 1738000.0;
	m_Latitude = -100.0;
	m_DirtyImage = true;
	m_Phase = 0.0;
	m_ApparentBrightness = 0.0;
	m_MoonShine = 0.0;
	m_EarthShine = 0.0;
	m_SunShine = 0.0;
	
	m_RenderDistance = 950000.0;
	float x, y;
	x = y = 1.0; 
	m_Image = osgDB::readImageFile("moon.png");
	assert(m_Image.valid());

	// set up the texture.
	m_Texture = new osg::Texture2D;
	m_Texture->setImage(m_Image.get());

	// set up the drawstate.
	osg::StateSet* dstate = new osg::StateSet;
	dstate->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
	dstate->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	dstate->setTextureAttributeAndModes(0, m_Texture.get(),osg::StateAttribute::ON);
	dstate->setMode(GL_CLIP_PLANE0, osg::StateAttribute::OFF);
	dstate->setMode(GL_CLIP_PLANE1, osg::StateAttribute::OFF);
	dstate->setMode(GL_CLIP_PLANE2, osg::StateAttribute::OFF);
	dstate->setMode(GL_CLIP_PLANE3, osg::StateAttribute::OFF);
	dstate->setMode(GL_CLIP_PLANE4, osg::StateAttribute::OFF);
	dstate->setMode(GL_CLIP_PLANE5, osg::StateAttribute::OFF);
	dstate->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	osg::BlendFunc *bf = new osg::BlendFunc;
	bf->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	dstate->setAttributeAndModes(bf, osg::StateAttribute::ON);
	osg::Depth *depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 2000000.0);
	dstate->setAttributeAndModes(depth, osg::StateAttribute::OFF);

	// set up the geoset.
	Geometry* geom = new Geometry;
	geom->setStateSet(dstate);

	Vec3Array* coords = new Vec3Array(4);
	(*coords)[0].set(-x,0.0f,y);
	(*coords)[1].set(-x,0.0f,-y);
	(*coords)[2].set(x,0.0f,-y);
	(*coords)[3].set(x,0.0f,y);
	geom->setVertexArray(coords);

	Vec2Array* tcoords = new Vec2Array(4);
	(*tcoords)[0].set(0.0f,1.0f);
	(*tcoords)[1].set(0.0f,0.0f);
	(*tcoords)[2].set(1.0f,0.0f);
	(*tcoords)[3].set(1.0f,1.0f);
	geom->setTexCoordArray(0,tcoords);

	osg::Vec4Array* colours = new osg::Vec4Array(1);
	(*colours)[0].set(1.0f,1.0f,1.0,1.0f);
	geom->setColorArray(colours);
	geom->setColorBinding(Geometry::BIND_OVERALL);

	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::QUADS,0,4));

	m_Moon = geom;
	
	//dstate->setMode(GL_FOG, osg::StateAttribute::OFF);
	//dstate->setRenderBinDetails(-2,"RenderBin");
	m_Transform = new osg::MatrixTransform;

	m_Billboard = new AstroBillboard;
	m_Billboard->setCullingActive(false);
	m_Billboard->setStateSet(dstate);
	m_Billboard->addDrawable(m_Moon.get());
	m_Billboard->setAxes(osg::Vec3(0.0,1.0,0.0), osg::Vec3(0.0,0.0,1.0), osg::Vec3(0.0,1.0,0.0));
	m_Transform->addChild(m_Billboard.get());
	m_Transform->setStateSet(dstate);
	m_Transform->setCullingActive(false);
//	moon->setStateSet(dstate);
}

void Moon::_updateShading(float h, float intensity) {
	static int xx = 0;
	if (xx++ % 5) return;
	if (h < -0.2) return;
	if (h < 0.0) h = 0.0;
	float hs = 2.0 - 2.0*sqrt(h);
	if (hs > 2.0) hs = 2.0;
	if (hs < 1.0) hs = 1.0;
	float DAY = 1.0-2.0*intensity*intensity*hs;
	//float DAY = 1.0-1.5*intensity*hs;
	if (DAY < 0.0) DAY = 0.0;
	float gibbous = 1.0 - h * 8.0;
	if (gibbous < 0.0) gibbous = 0.0;
	float R = 1.0 - gibbous * 0.412 * 0.5;
	float G = 1.0 - gibbous * 0.717 * 0.5;
	float B = 1.0 - gibbous * 0.999 * 0.5;
	osg::Vec4Array* colours = dynamic_cast<Vec4Array*>(m_Moon->getColorArray());
	assert(colours);
	(*colours)[0].set(R, G, B, DAY);
	m_Moon->setColorArray(colours);
	m_Moon->dirtyDisplayList();
}

void Moon::_updateLighting(double x, double y, double z, double h, float intensity) {
	float brightness = 0.0;
	float horizon = 0.0;
	if (h > 0.0) {
	 	horizon = 1.2 * h / (0.4 + h);
	}
	// moon direction
	if (z < 0.0) {
		z = 0.0;
		brightness = 0.0;
	}
	// moonlight direction (if using spot)
	m_Light->setDirection(osg::Vec3(-x, -y, -z));
	// place the sun far from the scene
	m_Light->setPosition(osg::Vec4(x*1e+6, y*1e+6, z*1e+6, 1.0));
	// brightess should reach a maximum of approximately 1.0 during a fullmoon.
	brightness = 6.5e+2 * m_MoonShine * (1.0 - 40.0 * intensity * intensity * intensity) * horizon;
	//cout << brightness << ", shine=" << m_MoonShine << ", I=" << intensity << ", hor=" << horizon << endl;
	if (brightness < 0.0) brightness = 0.0;
	if (brightness > 1.0) brightness = 1.0;
	float a_r, a_g, a_b;
	//a_r = 0.17 * brightness; a_g = 0.18 * brightness; a_b = 0.20 * brightness;
	a_r = 0.25 * brightness; a_g = 0.27 * brightness; a_b = 0.28 * brightness;
	// set the various light components
	//cout << brightness << ":" << a_b << endl;
	m_Light->setAmbient(osg::Vec4(a_r*0.2, a_g*0.2, a_b*0.2, 1.0));
	m_Light->setDiffuse(osg::Vec4(a_r, a_g, a_b, 1.0));
	m_Light->setSpecular(osg::Vec4(0,0,0,1));
	m_ApparentBrightness = brightness;
}

void Moon::updateScene(double lat, double lmst, Sun const &sun, float intensity) {
	double h, A;
	double x, y, z;
	if (lat != m_Latitude) {
		m_Billboard->setAxes(osg::Vec3(0.0,cos(lat), sin(lat)), osg::Vec3(0.0,0.0,1.0), osg::Vec3(0.0,1.0,0.0));
		m_Latitude = lat;
	}
	toObserver(lat, lmst, h, A);
	if (m_DirtyImage && h > -0.2) {
		m_DirtyImage = false;	
		_updateIllumination(sun);
	} else
	if (h < -0.2 && !m_DirtyImage) {
		m_DirtyImage = true;
	}
	_updateShading(h, intensity);
	y = -cos(h);
	x = y*sin(A);
	y *= cos(A);
	z = sin(h);
	double scale = m_RenderDistance * _radius / _distance;
	osg::Matrix S = osg::Matrix::scale(scale, scale, scale);
	osg::Matrix T = osg::Matrix::translate(x*m_RenderDistance, y*m_RenderDistance, z*m_RenderDistance);
	m_Transform->setMatrix(S*T);
	_updateLighting(x, y, z, h, intensity);
	
}

void Moon::_updateIllumination(Sun const &sun) {
	m_Phase = (sun.getLambda() - getLambda());
	float sun_angle = getBeta() * 0.38/159.6; // (distance ratio moon:sun)
	float a0 = 0.5 * m_Phase;
	float a1 = 0.5 * (G_PI - m_Phase);
	float f0 = fabs(tan(0.5*a0));
	float f1 = fabs(tan(0.5*a1));
	m_EarthShine = 0.19 * 0.5;
	if (f0 > 0.0) {
		m_EarthShine *= 1.0 - sin(a0)*tan(a0)*log(1.0/f0);
	}
	m_SunShine = 1905.0;
	if (f1 > 0.0) {
		m_SunShine *= 1.0 - sin(a1)*tan(a1)*log(1.0/f1);
	}
	float rd = _radius / _distance;
	m_MoonShine = 0.67 * 0.072 * rd * rd * (m_EarthShine + m_SunShine);
	//cout << a0 << ":" << a1 << "|" << m_EarthShine << ":" << m_SunShine << ":" << m_MoonShine << endl;
	_maskPhase(m_Phase, sun_angle);
}


/**
 * Generate a texture with the correct lunar phase.
 *
 * The original full moon image is masked with alpha equal to 1 in the 
 * shadow and a soft terminus.  Beta, the angle of the moon above the
 * ecliptic plane is not used, since the effect is relatively small and
 * the computation much simpler without it.
 */
void Moon::_maskPhase(float phi, float beta) {
	{
		int n = (int) (phi / (2.0*G_PI));
		phi -= n * 2.0 * G_PI;
		cout << "MOON PHASE = " << phi << endl;
	}
	osg::Image *phased = new osg::Image(*(m_Image.get()), CopyOp::DEEP_COPY_IMAGES);
	int i, j;
	int width = phased->s();
	int height = phased->t();
	unsigned char* data = phased->data();
	// alpha component
	data += 3;
	// adhoc, gives a maximum value of 10 for a full earth (new moon)
	float dark = m_EarthShine * 105.0; 
	if (dark < 0.0) dark = 0.0;
	if (dark > 10.0) dark = 10.0;
	unsigned char darkalpha = (unsigned char) dark;
	float terminus = 1000.0;
	float sz = 1.9999 / height;
	float sx = 1.9999 / width;
	assert(phased->getPixelFormat() == GL_RGBA);
	// beta has an extremely small effect on the shading (most likely sub-pixel)
	// and since i'm not certain whether to use + or - beta, i'm just setting it
	// equal to zero for now.
	beta = 0.0;
	float cos_phi = cos(phi);
	float A = sin(phi);
	float B = cos_phi * cos(beta);
	float C = cos_phi * sin(beta);
	for (j = 0; j < height; j++) {
		float z = j*sz-1.0;
		float edge = 1.0-z*z;
		for (i = 0; i < width; i++) {
			float x = i*sx-1.0;
			float x2 = x*x;
			int c = int(*data);
			if (x2 > edge) { 
				// outside the disc
				c = 0;
			} else {
				// inside the disc
				float y = sqrt(edge - x2);
				float projection = A*x + B*y + C*z;
				// angle away from the terminus (perpendular to illumination)
				float alpha = -asin(projection);
				int shade = int(alpha * terminus) + 128;
				if (shade < 0) shade = 0; else
				if (shade > 255) shade = 255;
				if (shade < c) c = shade;
				// FIXME relate to earthshine... i'm guessing something like
				// 10-15 would be a good upper bound.
				if (c < darkalpha) c = darkalpha;
			}
			*data = (unsigned char) c;
			data += 4;
		}
	}
	m_Phased = phased;
	m_Texture->setImage(m_Phased.get());
}

osg::Node* Moon::getNode() {
	return m_Transform.get();
}



////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


#define CUSTOM


#if 0
// CUSTOMIZATION PARAMETERS
// --------------------------------------------------------------------------
// RGB addative sky values for midnight sky color under a full moon)
#define FULLMOON_RED   0.008
#define FULLMOON_GREEN 0.035
#define FULLMOON_BLUE  0.140
// CIE luminesence (Y) looking into the sun at the zenith.
#define OVER_LUMINESCENCE 1.2 //1.5
// parameters for the attenuation function which cuts the intensity
// as the sun nears the horizon.  Large values of SUNSET_SHARPNESS localizes
// the drop at the horizon.  NIGHT_BASE is the scaling when the sun is 90 
// degrees below the horizon.
#define SUNSET_SHARPNESS 4.0
#define NIGHT_BASE 0.02
// sharpness of the halo around the sun (0.1 to 2.0)
#define HALO_SHARPNESS 0.5
// --------------------------------------------------------------------------
#endif

SkyShader::SkyShader() {
	m_OverLuminescence = 1.2;
	m_HaloSharpness = 0.5;
	m_SunsetSharpness = 4.0;
	m_NightBase = 0.02;
	m_FullMoonColor = Color(0.008, 0.035, 0.140, Color::RGB);
	setTurbidity(4.0);
	setSunElevation(0.0);
}

void SkyShader::getSkyCoefficients(float T, sky_c &skylight) {
	skylight.x[0] = -0.0193 * T - 0.2592;
	skylight.x[1] = -0.0665 * T + 0.0008;
	skylight.x[2] = -0.0004 * T + 0.2125;
	skylight.x[3] = -0.0641 * T - 0.8989;
	skylight.x[4] = -0.0033 * T + 0.0452;
	skylight.y[0] = -0.0167 * T - 0.2608;
	skylight.y[1] = -0.0950 * T + 0.0092;
	skylight.y[2] = -0.0079 * T + 0.2102;
	skylight.y[3] = -0.0441 * T - 1.6537;
	skylight.y[4] = -0.0109 * T + 0.0529;
	skylight.Y[0] =  0.1787 * T - 1.4630;
	skylight.Y[1] = -0.3554 * T + 0.4275;
	skylight.Y[2] = -0.0227 * T + 5.3251;
	skylight.Y[3] =  0.1206 * T - 2.5771;
	skylight.Y[4] = -0.0670 * T + 0.3703;
}

float SkyShader::F(float theta, float gamma, coeff &p) {
	float cos_g = cos(gamma);
#ifdef CUSTOM
	float cos_t = fabs(cos(theta)) + 0.09;
#else
	float cos_t = cos(theta);
#endif
	return (1.0 + p[0]*exp(p[1]/cos_t))*(1.0+p[2]*exp(p[3]*gamma)+p[4]*cos_g*cos_g);
}

float SkyShader::FastPerez(float theta, float gamma, float factor, float z, coeff &p) {
	if (theta > 1.57) theta = 1.57;
	return factor * z * F(theta, gamma, p);
}

float SkyShader::Perez(float theta, float gamma, float theta_s, float z, coeff &p) {
	if (theta > 1.57) theta = 1.57;
	if (theta_s >  1.50) theta_s = 1.50 + ((theta_s-1.50) * 0.5);
	float denom = F(0.0, theta_s, p);
	if (denom != 0.0) z /= denom;
	return z * F(theta, gamma, p);
}

Color SkyShader::getZenith(float T, float theta_s) {
	float s = theta_s;
	float s2 = s*s;
	float s3 = s2*s;
	float t = T;
	float t2 = t*t;
	float xa =  0.00166 * s3 - 0.00375 * s2 + 0.00209 * s;
	float xb = -0.02903 * s3 + 0.06377 * s2 - 0.03202 * s + 0.00394;
	float xc =  0.11693 * s3 - 0.21196 * s2 + 0.06052 * s + 0.25886;
	float xz = xa * t2 + xb * t + xc;
	float ya =  0.00275 * s3 - 0.00610 * s2 + 0.00317 * s;
	float yb = -0.04214 * s3 + 0.08970 * s2 - 0.04153 * s + 0.00516;
	float yc =  0.15346 * s3 - 0.26756 * s2 + 0.06670 * s + 0.26688;
	float yz = ya * t2 + yb * t + yc;
	float chi = (0.4444444 - T * 0.008333333) * (G_PI - 2.0*s);
	float Yz = (4.0453 * T - 4.9710) * tan(chi) - 0.2155 * T + 2.4192;
	// Yz is in units of kcd/m^2

#ifdef CUSTOM
	// FIXME
	// Ad hoc function for post-sunset sky.
	if (Yz < 0.2) Yz = 0.2 - (0.2-Yz)*(0.2-Yz)*0.05;
	if (Yz < 0.05) Yz = 0.05;
#endif
	
	return Color(xz, yz, Yz, Color::CIExyY, false);
}


void SkyShader::setTurbidity(float T) {
	m_Turbidity = T;
	m_Dirty = true;
}

void SkyShader::setSunElevation(float h) {
	m_AzimuthCorrection = 0.0;
	if (h < -G_PI) {
		h += 2.0 * G_PI * int((G_PI - h) / (2.0 * G_PI));
	} else
	if (h > G_PI) {
		h -= 2.0 * G_PI * int((G_PI + h) / (2.0 * G_PI));
	}
	if (h > 0.5 *G_PI) {
		h = G_PI - h;
		m_AzimuthCorrection = G_PI;
	} else
	if (h < -0.5*G_PI) {
		h = -G_PI - h;
		m_AzimuthCorrection =  -G_PI;
	}
	m_SunElevation = h;
	m_Dirty = true;
}

void SkyShader::_computeBase() {
	m_Dirty = false;

	m_SunTheta = 0.5*G_PI - m_SunElevation;
	m_SunVector[0] = 0.0;
	m_SunVector[1] = sin(m_SunTheta);
	m_SunVector[2] = cos(m_SunTheta);

	getSkyCoefficients(m_Turbidity, m_Coefficients);
	
	float theta = m_SunTheta;
	if (theta >  1.50) theta = 1.50 + ((m_SunTheta-1.50) * 0.5);
	float denom;
	denom = F(0.0, theta, m_Coefficients.x);
	if (denom == 0.0) denom = 1.0;
	m_PerezFactor.x = 1.0 / denom;
	denom = F(0.0, theta, m_Coefficients.y);
	if (denom == 0.0) denom = 1.0;
	m_PerezFactor.y = 1.0 / denom;
	denom = F(0.0, theta, m_Coefficients.Y);
	if (denom == 0.0) denom = 1.0;
	m_PerezFactor.Y = 1.0 / denom;

	m_Zenith = getZenith(m_Turbidity, m_SunTheta);
	m_MaxY = 1.0;

#ifdef CUSTOM
	// only rescale down to the horizon, bad things happen if we go further
	if (m_SunTheta >= 0.5*G_PI) {
		m_MaxY = Perez(0.5*G_PI, 0.0, 0.5*G_PI, m_Zenith.getC(), m_Coefficients.Y);
	} else {
		m_MaxY = Perez(m_SunTheta, 0.0, m_SunTheta, m_Zenith.getC(), m_Coefficients.Y);
	}
#endif

#ifdef CUSTOM
	// F(sun_h) is my own attenuation function which cuts the intensity
	// as the sun nears the horizon.  Large values of SHARPNESS localize
	// the drop at the horizon.  BASE is scaling with the sun 90 degrees
	// below the horizon.
	m_F =(atan(m_SunElevation*m_SunsetSharpness)/G_PI+0.5)*(1.0-m_NightBase) + m_NightBase;
#endif

}

Color SkyShader::SkyColor(float elevation, float azimuth, float dark, float &intensity) {
	if (m_Dirty) _computeBase();
	float theta = 0.5*G_PI - elevation;
	float A = azimuth + m_AzimuthCorrection;
	float v[3] = {sin(A)*sin(theta), cos(A)*sin(theta), cos(theta)};
	float dot = v[0]*m_SunVector[0]+v[1]*m_SunVector[1]+v[2]*m_SunVector[2];
	// numerical error can cause abs(dot) to exceed 1... 
	if (dot < -1.0) dot = -1.0; else
	if (dot >  1.0) dot =  1.0;
	float gamma = acos(dot);
	float ciex = FastPerez(theta, gamma, m_PerezFactor.x, m_Zenith.getA(), m_Coefficients.x);
	float ciey = FastPerez(theta, gamma, m_PerezFactor.y, m_Zenith.getB(), m_Coefficients.y);
	float cieY = FastPerez(theta, gamma, m_PerezFactor.Y, m_Zenith.getC(), m_Coefficients.Y);

#ifdef CUSTOM
	//  scale to Y = 1 looking directly at the sun
	if (m_MaxY > 0.0) cieY = m_OverLuminescence * cieY / m_MaxY;
	
	// model for cloudy days:
	//cieY = zenith.getC() * (1+2*cos(theta))/3.0;

	// taking sqrt(Y) softens the sky a bit, spreading the blue more
	// unformly.  this should probably be done instead by blending with
	// a single blue translucent surface across the field of view, where
	// the transparency depends on altitude 
	//float oldY = cieY;
	if (cieY < 0.0) cieY = 0.0;
	cieY = pow((double)cieY, (double)m_HaloSharpness) * m_F;
	if (cieY > m_OverLuminescence) cieY = m_OverLuminescence;
#endif // CUSTOM

	intensity = cieY;

	Color xyY(ciex, ciey, cieY, Color::CIExyY, false);
	Color rgb = xyY.toRGB();
#ifdef CUSTOM
	rgb.composite(m_FullMoonColor, 1.0, dark);
	rgb.check();
#endif // CUSTOM
	return rgb;
}


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

Sky::Sky(): Group() {
	m_LastMoonFullUpdate = -1e+10;
	m_Latitude = 0.0;
	m_LMST = 0.0;
	m_JD = 0.0;
	m_SpinTheWorld = 0.0;
	_init();
}

Sky::~Sky() {
	delete[] m_lev;
}

void Sky::_init()
{
	initColor();
	_initLights();
	m_lev = new float[90];
	float elev = -10.0;
	m_nlev = 0;
	m_HorizonIndex = 0;
	while (elev <= 90.0 && m_nlev < 90) {
		m_lev[m_nlev] = elev;
		m_nlev++;
		if (elev < -5.0 || elev >= 5.0) {
			elev += 5.0; 
		} else
		if (elev < -1.0) {
			elev = -1.0;
		} else
		if (elev <  1.0) {
			elev = 1.0; 
			m_HorizonIndex = m_nlev;
		} else {
			elev = 5.0;
		}
	}
	m_nseg = 37;
	
	int i, j;
	float x, y, z;
	float alpha, theta;
	float radius = 1100000.0f;
	m_SkyDome = new Geometry;
	//m_SkyDome->setSupportsDisplayList(false);

	m_Colors = new Vec4Array(m_nseg*m_nlev);
	Vec3Array& coords = *(new Vec3Array(m_nseg*m_nlev));
	Vec4Array& colors = *m_Colors;
	Vec2Array& tcoords = *(new Vec2Array(m_nseg*m_nlev));
	m_HorizonColors = new Vec4Array(m_nseg);

	int ci, ii;
	ii = ci = 0;

	for( i = 0; i < m_nlev; i++ ) {
		for( j = 0; j < m_nseg; j++ ) {
			alpha = osg::DegreesToRadians(m_lev[i]);
			theta = osg::DegreesToRadians((float)(j*360.0/(m_nseg-1)));

			x = radius * cosf( alpha ) * cosf( theta );
			y = radius * cosf( alpha ) * -sinf( theta );
			z = radius * sinf( alpha );

			coords[ci][0] = x;
			coords[ci][1] = y;
			coords[ci][2] = z;

			colors[ci][0] = 0.0;
			colors[ci][1] = 0.0;
			colors[ci][2] = 0.0;
			colors[ci][3] = 1.0;

			tcoords[ci][0] = (float)j/(float)(m_nseg-1);
			tcoords[ci][1] = (float)i/(float)(m_nlev-1);

			ci++;
		}
	}

	DrawElementsUShort* drawElements = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP);
	drawElements->reserve(m_nlev*m_nseg*2);
	for( i = 0; i < m_nlev-1; i++ ) {
		//DrawElementsUShort* drawElements = new DrawElementsUShort(PrimitiveSet::TRIANGLE_STRIP);
		//drawElements->reserve(m_nseg*2);
		for( j = 0; j < m_nseg; j++ ) {
			drawElements->push_back((i+1)*m_nseg+j);
			drawElements->push_back((i+0)*m_nseg+j);
		}
		//m_SkyDome->addPrimitiveSet(drawElements);
	}
	m_SkyDome->addPrimitiveSet(drawElements);

	m_SkyDome->setVertexArray( &coords );
	m_SkyDome->setTexCoordArray( 0, &tcoords );
	m_SkyDome->setColorArray( &colors );
	m_SkyDome->setColorBinding( Geometry::BIND_PER_VERTEX );

	Texture2D *tex = new Texture2D;
	tex->setImage(osgDB::readImageFile("CSP_Splash.tga"));

	StateSet *dstate = new StateSet;

	dstate->setTextureAttributeAndModes(0, tex, StateAttribute::OFF);
	dstate->setTextureAttributeAndModes(0, new TexEnv);
	dstate->setMode(GL_LIGHTING, StateAttribute::OFF);
	dstate->setMode(GL_CULL_FACE, StateAttribute::OFF);
	dstate->setMode(GL_DEPTH_TEST, StateAttribute::OFF);

	// clear the depth to the far plane.
	osg::Depth* depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0, 1.0);   
	dstate->setAttributeAndModes(depth,StateAttribute::OFF);
	dstate->setMode(GL_FOG, osg::StateAttribute::OFF);
	dstate->setRenderBinDetails(-2,"RenderBin");
	m_SkyDome->setStateSet(dstate);

	m_StarDome = new StarSystem();
	m_StarDome->setStateSet(dstate);

	osg::Geode *geode = new osg::Geode();
	geode->addDrawable(m_SkyDome.get());
	geode->addDrawable(m_StarDome.get());
	addChild(geode);
	addChild(m_Moon.getNode());

	setName("Sky");
};


void Sky::_initLights() {
	m_Sun.initLight(0);
	m_Moon.initLight(1);
}


osg::Light *Sky::getSunLight() {
	return m_Sun.getLight();
}

osg::Light *Sky::getMoonLight() {
	return m_Moon.getLight();
}

// TODO
// take altitide into account for turbidity
// relate night sky color to moon brighness (and altitude)
// maybe modify SkyColor it consider moon position:
//   if moon is up and sun is down (or nearly so), add in a glow and background color
//   or just use an additive textured quad in front of/behind the moon?

void Sky::_updateMoon(bool quick) {
	if (!quick) {
		m_Moon.updatePosition(m_JD);
	}
	m_Moon.updateScene(m_Latitude, m_LMST, m_Sun, m_AverageIntensity);
}

void Sky::_updateSun() {
	double sun_h, sun_A;
	m_Sun.updatePosition(m_JD);
	m_Sun.toObserver(m_Latitude, m_LMST, sun_h, sun_A);
	
	double light_h = sun_h;
	// below horizon
	if (sin(light_h) < 0.0) {
		light_h = 0.0;
	}
	m_SkyShader.setSunElevation(sun_h);
	m_SunColor = m_SkyShader.SkyColor(light_h, 0.0, 0.0, m_SunIntensity);

	// update the skydome
	_updateShading(sun_h, sun_A);
	
	m_Sun.updateScene(sun_h, sun_A, m_SunColor, m_SunIntensity, m_AverageIntensity);
}

void Sky::_updateStars() {
	m_StarDome->setObserver(m_Latitude, m_LMST, m_AverageIntensity);
}

void Sky::_updateShading(double sun_h, double sun_A) {
	Vec4Array& colors = *(dynamic_cast<Vec4Array*>(m_SkyDome->getColorArray()));
	double da = 2.0 * G_PI / (m_nseg - 1);
	double min_a = 0.5*da;
	double jitter = 0.0;
	int i, j, ci = 0;
	m_AverageIntensity = 0.0;
	float dark = m_Moon.getApparentBrightness();
	for (i = 0; i < m_nlev; i++) {
		double elev = m_lev[i] * D2R;
		double azimuth = -sun_A - 0.5 * G_PI;
		bool at_vertex = fabs(elev - sun_h) < min_a; // this is only a rough measure
		for (j = 0; j < m_nseg; j++) {
			float intensity;
			// if the sun lines up very close to a vertex, the vertex will be
			// pure white and create a noticible artifact on the surrounding
			// triangles.  
			if (at_vertex && (fabs(azimuth) < min_a)) {
				jitter = 0.0; //da;
			} else {
				jitter = 0.0;
			}
			Color c = m_SkyShader.SkyColor(elev, azimuth+jitter, dark, intensity);
			azimuth += da;
			colors[ci][0] = c.getA();
			colors[ci][1] = c.getB();
			colors[ci][2] = c.getC();
			colors[ci][3] = 1.0;
			if (i == m_HorizonIndex) {
				(*m_HorizonColors)[j] = colors[ci];
			}
			ci++;
			m_AverageIntensity += intensity;
		}
	}
	m_AverageIntensity /= m_nlev*m_nseg;
	m_SkyDome->dirtyDisplayList();
}


void Sky::spinTheWorld(bool noreset) {
	if (noreset) {
		m_SpinTheWorld += 0.00034722; // 30 seconds
	} else {
		m_SpinTheWorld = 0.0;
	}
}

void Sky::update(double lat, double lon, simdata::SimDate const &t) {
	m_LMST = t.getMST(lon) + m_SpinTheWorld * 2.0 * G_PI;
	m_JD = t.getJulianDate() + m_SpinTheWorld;
	m_Latitude = lat;
	_updateSun();
	// do a full moon update every 300 seconds (orbital position and lighting)
	if (fabs(m_JD - m_LastMoonFullUpdate) > 1.15741e-5 * 5.0) { // 300.0) {
		m_LastMoonFullUpdate = m_JD;
		_updateMoon(false);
	} else {
		_updateMoon(true);  // quick update
	}
	_updateStars();
	
}


// this is a quick hack to explore the effect of dynamically
// altering fog color to match the sky shade at the horizon
// near the center of the field of view.  it is much better
// than a fixed fog color, but produces bad effects around
// sunrise/set for wide fields of view.  2 point interpolation
// (avg=0) seems to look better than the multipoint averaging 
// implemented below for avg > 0.
osg::Vec4 Sky::getHorizonColor(float angle) {
	static int avg = 0;
	angle = -angle;
	if (angle < 360.0) {
		angle -= 360.0 * (int(angle / 360.0)-2);
	}
	angle *= m_nseg / 360.0;
	int idx = int(angle);
	float da = angle - idx;
	assert(m_HorizonColors);
	osg::Vec4 color;
	color = (*m_HorizonColors)[(idx-avg)%m_nseg] * (1.0-da);
	for (int i = 1-avg; i <= avg; i++) {
		color += (*m_HorizonColors)[(idx+i)%m_nseg];
	}
	color += (*m_HorizonColors)[(idx+avg+1)%m_nseg] * da;
	return color/(2*avg+1);
}

