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
 * @file LandingGear.cpp
 *
 **/

/*
 * TODO
 *
 *  Aerodynamic drag
 *  Wind damage
 *  Extension/retract modelling + 3D hooks
 *  Rough terrain damage
 *  Generic damage modelling, gear collapse,  etc.
 *
 */

#include "LandingGear.h"
#include "LogStream.h"
#include <SimData/Math.h>

#include <cstdio>
#include <iostream>

using simdata::DegreesToRadians;
using simdata::Dot;
using simdata::RadiansToDegrees;
using simdata::QVRotate;
using simdata::Vector3;


SIMDATA_REGISTER_INTERFACE(LandingGear)
SIMDATA_REGISTER_INTERFACE(LandingGearSet)


LandingGear::LandingGear() {
	m_Chained = false;
	m_K = 0.0;
	m_Beta = 0.0;
	m_BrakeLimit = 0.0;
	m_StaticFriction = 0.3;
	m_DynamicFriction = 0.2;
	m_SteeringLimit = 0.0;
	m_TireK = 200000.0;
	m_TireBeta = 500.0;
	m_Damage = 0.0;
	m_Extended = true;
	m_TireShiftX = 0.0;
	m_TireShiftY = 0.0;
	m_Compression = 0.0;
	m_CompressionLimit = 0.0;
	m_WOW = false;
	m_Touchdown = false;
	m_Skidding = 0.0;
	m_Brake = 0.0;
	m_BrakeSetting = 0.0;
	m_ABS = true;
	m_SteerAngle = 0.0;
	m_Steer = Vector3::YAXIS;
}

#if 0
LandingGear::LandingGear(LandingGear const &g) {
	*this = g;
}

LandingGear const &LandingGear::operator=(LandingGear const &g) {
	if (&g != this) {
		m_MaxPosition = g.m_MaxPosition;
		m_Position = g.m_Position;
		m_Motion = g.m_Motion;
		m_Chained = g.m_Chained;
		m_K = g.m_K;
		m_Beta = g.m_Beta;
		m_BrakeLimit = g.m_BrakeLimit;
		m_StaticFriction = g.m_StaticFriction;
		m_DynamicFriction = g.m_DynamicFriction;
		m_SteeringLimit = g.m_SteeringLimit;
		m_TireK = g.m_TireK;
		m_TireBeta = g.m_TireBeta;
		m_Damage = g.m_Damage;
		m_Extended = g.m_Extended;
		m_TireShift = g.m_TireShift;
		m_WOW = g.m_WOW;
		m_Compression = g.m_Compression;
		m_CompressionLimit = g.m_CompressionLimit;
		m_Touchdown = g.m_Touchdown;
		m_Skidding = g.m_Skidding;
		m_Steer = g.m_Steer;
		m_Brake = g.m_Brake;
		m_BrakeSetting = g.m_BrakeSetting;
	}
	return g;
}
#endif

void LandingGear::pack(simdata::Packer& p) const {
	Object::pack(p);
	p.pack(m_MaxPosition);
	p.pack(m_Motion);
	p.pack(m_CompressionLimit);
	p.pack(m_DamageLimit);
	p.pack(m_K);
	p.pack(m_Beta);
	p.pack(m_Chained);
	p.pack(m_BrakeLimit);
	p.pack(m_StaticFriction);
	p.pack(m_DynamicFriction);
	p.pack(m_SteeringLimit);
	p.pack(m_TireK);
	p.pack(m_TireBeta);
}

void LandingGear::unpack(simdata::UnPacker& p) {
	Object::unpack(p);
	p.unpack(m_MaxPosition);
	p.unpack(m_Motion);
	p.unpack(m_CompressionLimit);
	p.unpack(m_DamageLimit);
	p.unpack(m_K);
	p.unpack(m_Beta);
	p.unpack(m_Chained);
	p.unpack(m_BrakeLimit);
	p.unpack(m_StaticFriction);
	p.unpack(m_DynamicFriction);
	p.unpack(m_SteeringLimit);
	p.unpack(m_TireK);
	p.unpack(m_TireBeta);
}

void LandingGear::postCreate() {
	Object::postCreate();
	m_Motion.Normalize();
	m_Position = m_MaxPosition;
}

simdata::Vector3 LandingGear::simulate(simdata::Quaternion const &q, 
                                       simdata::Vector3 const &velocity, float h, simdata::Vector3 const &normal, float dt) 
{
	static int n = 0, j = 0;
	if (!m_Extended) return Vector3::ZERO;
	Vector3 extension = QVRotate(q, m_MaxPosition);
	Vector3 motion = QVRotate(q, m_Motion);
	float mn = Dot(motion, normal);
	if (mn == 0.0) mn = 0.0001;
	float compression = - (Dot(extension, normal) + h) / mn;
	// TODO: add in air drag
	if (compression <= 0.0) {
		m_Compression = 0.0;
		m_Position = m_MaxPosition;
		m_WOW = false;
		if (n>1000 && j++ > 10) n = 0;
		return Vector3::ZERO;
	}
	if (!m_WOW) {
		// TODO should also store global position, but this isn't currently available
		m_Touchdown = true;
	}
	m_WOW = true;
	if (compression >= m_CompressionLimit) {
		m_Compression = m_CompressionLimit;
		// TODO: break the gear!
	} else {
		m_Compression = compression;
	}

	Vector3 v_local = QVRotate(q, velocity);
	
	// FIXME only normal force is taken into account, but other components
	// can matter if m_Motion isn't vertical. (e.g. when brakes are applied)
	// calculate strut compression speed
	Vector3 v_normal_local = Dot(v_local, normal) * normal;
	Vector3 v_normal_body = QVRotate(q.Bar(), v_normal_local);
	float v = - Dot(v_normal_body, m_Motion);
	// restrict v to reasonable limits
	if (v > 10.0) v = 10.0;
	if (v < -10.0) v = -10.0;
	
	Vector3 v_tangent_local = v_local - v_normal_local;
	Vector3 v_tangent_body = QVRotate(q.Bar(), v_tangent_local);

	// ground support
	float normal_force = (m_K * m_Compression + m_Beta * v) * mn;
	if (normal_force < 0.0) normal_force = 0.0; // wheel hop
	Vector3 normal_local = normal * normal_force;
	Vector3 normal_body = QVRotate(q.Bar(), normal_local);
	
	// wheel direction in local coordinates projected to ground
	Vector3 y_local = QVRotate(q, m_Steer);
	Vector3 y_tangent_local = y_local - Dot(y_local, normal)*normal;
	y_tangent_local.Normalize();
	Vector3 x_tangent_local = y_tangent_local ^ normal;

	double forward_speed = Dot(v_tangent_local, y_tangent_local);
	Vector3 v_y_local = forward_speed * y_tangent_local;
	Vector3 v_x_local = v_tangent_local - v_y_local;

	// drag
	Vector3 drag_body = QVRotate(q.Bar(), y_tangent_local);
	float abs_forward_speed = fabs(forward_speed);
	
	// update brake setting
	float f = dt*5.0;
	if (f > 1.0) f = 1.0;
	m_Brake = m_Brake * (1.0-f) + (m_BrakeSetting - m_Brake) * f;

	float brake_limit = m_Brake * 25000.0; // arbitrary... move to xml
	float brake_force = m_TireShiftY * m_TireK;
	
	if (fabs(brake_force) > brake_limit) {
		// arbitrary reduced slip friction (FIXME move to XML)
		float slip_factor = 0.8;
		if (brake_force < 0.0) slip_factor = -slip_factor;
		brake_force = brake_limit * slip_factor; 
		if (m_TireShiftY * forward_speed < 0.0) {
			m_TireShiftY += forward_speed * dt;
		}
	} else {
		m_TireShiftY += forward_speed * dt;
	}

	float base_drag = 1000.0 / (1.0 + abs_forward_speed);
	float drag_force = brake_force + (base_drag + abs_forward_speed)*forward_speed;
	
	drag_body *= -drag_force;


	// side
	Vector3 side_body = QVRotate(q.Bar(), x_tangent_local);
	float side_speed = v_x_local.Length();
	// slip angle approximation
	//side_speed /= (abs_forward_speed + 1.0);
	// are we slipping to the left or right in wheel coordinates?
	if (Dot(v_x_local, x_tangent_local) < 0.0) side_speed = -side_speed;
	float side_damping = 0.0;
	// don't keep deforming past the point we lose traction
	if (!m_Skidding || m_TireShiftX*side_speed > 0.0 || m_TireShiftX == 0.0) {
		m_TireShiftX += - side_speed * dt;
		side_damping = - side_speed * m_TireBeta;
	}
	// limit deformation to a reasonable amount
	if (m_TireShiftX > 0.1) {
		m_TireShiftX = 0.1;
		side_damping = 0.0;
	} else
	if (m_TireShiftX < -0.1) {
		m_TireShiftX = -0.1;
		side_damping = 0.0;
	}
	float side_force =  (m_TireK * m_TireShiftX) + side_damping;
	if (m_SteeringLimit > 0.0) {
		// partial (approx) castering of steerable wheels to reduce instability
		side_force *= (0.1 + 0.9 * fabs(m_SteerAngle/m_SteeringLimit));
	}
	side_body *= side_force;

	// skid test
	float mu = m_StaticFriction;
	if (m_Skidding > 0.0) mu = m_DynamicFriction;
	float total_force = side_force*side_force + drag_force*drag_force;
	float friction = normal_force * mu;
	if (total_force > friction * friction) {
		float scale = friction / sqrt(total_force + 0.001);
		side_body *= scale;
		drag_body *= scale;
		if (m_ABS) {
			m_Brake *= scale;
			m_TireShiftY *= scale;
		}
		m_Skidding = 1.0 - scale;
	} else {
		m_Skidding = 0.0;
	}
	if (0 && m_Skidding && (n++ % 1000) == 0) {
		std::cout << "SKID" << std::endl;
	}
	if (0 && (n++ % 10000) == 0) {
		//if (m_Skidding > 0.0) std::cout << "SKID " << m_Skidding << ", FRIC " << friction << ", SIDE " << side_force << ", DRAG " << drag_force << ", S " << m_Steer << std::endl;
		std::cout << "SIDE SPEED: " << side_speed << std::endl;
		std::cout << "SIDE BODY : " << side_body << std::endl;
		std::cout << "DRAG BODY : " << drag_body << std::endl;
		std::cout << "NORM BODY : " << normal_body << std::endl;
		std::cout << "VELO BODY : " << velocity << std::endl;
		std::cout << "  X   Y   : " << m_MaxPosition << std::endl;
	}

	// sum forces
	Vector3 F = normal_body + drag_body + side_body;
	m_Position = m_MaxPosition + m_Motion * m_Compression;
	
	j=0;
	if (0 && (n++ % 1000) == 0) {
		//std::cout << "WOW " << compression << " -> " << F << " V = " << v_tangent_body << " d = " << drag_body << " s = " << side_body << "\n";
		std::cout << side_speed << ":" << m_TireShiftX << " vtb = " << v_tangent_body << " vxl = " << v_x_local<< " sb = " << side_body << "\n";
		          //<< "  v = " << v << ", h = " << h << ", gs = " << ground_speed << "\n";
	}
	if (0 && n++ < 1000 && (n % 10) == 0) {
		std::cout << "WOW " << compression << " -> " << F 
		          << "  v = " << v << ", h = " << h << ", max.z = " << extension.z << "\n";
	}

	return F;
}


float LandingGear::setSteering(float setting) {
	if (setting > 1.0) setting = 1.0;
	if (setting < -1.0) setting = -1.0;
	m_SteerAngle = setting * m_SteeringLimit;
	float rad = DegreesToRadians(m_SteerAngle);
	m_Steer = Vector3(sin(rad), cos(rad), 0.0); 
	return m_SteerAngle;
}

