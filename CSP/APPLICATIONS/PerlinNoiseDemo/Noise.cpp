///////////////////////////////////////////////////////////////////////////
//
//	Perlin Noise Demo
//  CSP - Combat Simulator Project
//	http://csp.homeip.net
//
//  Implementation of Class Noise.cpp
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	06/16/2002	file created	-	Stormbringer
//
//  The code provided here is derived from a Delphi Demo
//  by ...
//
//  It was converted to C++ for use in CSP, but the general structure 
//	remained the same. All the comments are from the original distribution
//
///////////////////////////////////////////////////////////////////////////
//
//  I used the following references for my implementation:
//  http://students.vassar.edu/mazucker/code/perlin-noise-math-faq.html
//  Darwin Peachey's chapter in "Texturing & Modeling: A Procedural Approach"
//  Another good resource is
//  http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
//
//  ******************************************************************************
//
//  This class generates 3D Perlin noise. The demo that comes with this is 2D, but
//  uses the 3rd dimension to create animated noise. The noise does not tile,
//  although it could be made to do so with a few small modifications to the
//  algorithm.
//
//  Perlin noise can be used as a starting point for all kinds of things,
//  including terrain generation, cloud rendering, procedural textures, and more.
//  Most of these techniques involve rendering multiple "octaves" of noise. This
//  means you generate multiple noise values for every pixel (each with different
//  X, Y and/or Z coordinates), and then sum them. There's an example of this in
//  the accompanying demo.
//
//////////////////////////////////////////////////////////////////////////////////

#include "Noise.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"


//    Borrowed from Darwyn Peachey (see references above).
//    The gradient table is indexed with an XYZ triplet, which is first turned
//    into a single random index using a lookup in this table. The table simply
//    contains all numbers in [0..255] in random order.

unsigned char PERM[GRADIENT_TABLE_SIZE] = 
{	  
      225,155,210,108,175,199,221,144,203,116, 70,213, 69,158, 33,252,
        5, 82,173,133,222,139,174, 27,  9, 71, 90,246, 75,130, 91,191,
      169,138,  2,151,194,235, 81,  7, 25,113,228,159,205,253,134,142,
      248, 65,224,217, 22,121,229, 63, 89,103, 96,104,156, 17,201,129,
       36,  8,165,110,237,117,231, 56,132,211,152, 20,181,111,239,218,
      170,163, 51,172,157, 47, 80,212,176,250, 87, 49, 99,242,136,189,
      162,115, 44, 43,124, 94,150, 16,141,247, 32, 10,198,223,255, 72,
       53,131, 84, 57,220,197, 58, 50,208, 11,241, 28,  3,192, 62,202,
       18,215,153, 24, 76, 41, 15,179, 39, 46, 55,  6,128,167, 23,188,
      106, 34,187,140,164, 73,112,182,244,195,227, 13, 35, 77,196,185,
       26,200,226,119, 31,123,168,125,249, 68,183,230,177,135,160,180,
       12,  1,243,148,102,166, 38,238,251, 37,240,126, 64, 74,161, 40,
      184,149,171,178,101, 66, 29, 59,146, 61,254,107, 42, 86,154,  4,
      236,232,120, 21,233,209, 45, 98,193,114, 78, 19,206, 14,118,127,
       48, 79,147, 85, 30,207,219, 54, 88,234,190,122, 95, 67,143,109,
      137,214,145, 93, 92,100,245,  0,216,186, 60, 83,105, 97,204, 52
};

CNoise::CNoise(unsigned int Seed)
{
  // Initialize the random gradients before we start.
  srand(Seed);
  InitGradients();
}

CNoise::~CNoise()
{
	return;
}

void CNoise::InitGradients()
{
	int i, interval, irnd;
	float a, r, z, theta, frnd;
	
	interval=10000;
	// Generate random gradient vectors.
	for(i=0;i<GRADIENT_TABLE_SIZE;i++)
	{
		irnd = rand() % interval;
		frnd = (float)irnd/interval;

		z = 1-2*frnd;
		a = 1.0f - (z*z);
		r = (float)sqrt(a);
		theta = 2*PI*frnd;
		FGradients[i*3] = (float)r*cos(theta);
		FGradients[i*3 + 1] = (float)r*sin(theta);
		FGradients[i*3 + 2] = (float)z;
	}
}

float CNoise::GetGradient(int index)
{
	return FGradients[index];
}

unsigned int CNoise::Permutate(unsigned int x)
{
	const unsigned char MASK = GRADIENT_TABLE_SIZE - 1;
	unsigned int Result;

	// Do a lookup in the permutation table.
	Result = PERM[(unsigned char)x & MASK];

	return Result;
}

unsigned int CNoise::Index(unsigned int ix, unsigned int iy, unsigned int iz)
{
	int Result;

	// Turn an XYZ triplet into a single gradient table index.
	Result = Permutate(ix + Permutate(iy + Permutate(iz)));

	return Result;
}

float CNoise::Lattice(unsigned int ix, unsigned int iy, unsigned int iz, 
					  float fx, float fy, float fz)
{
	unsigned int g;
	float Result;

	// Look up a random gradient at [ix,iy,iz] and dot it with the [fx,fy,fz] vector.
	g = Index(ix, iy, iz)*3;
	Result = FGradients[g]*fx + FGradients[g+1]*fy + FGradients[g+2]*fz;
	return Result;
}

float CNoise::Lerp(float t, float x0, float x1)
{
	float Result;

	// Simple linear interpolation.
	Result = x0 + t*(x1-x0);

	return Result;
}

float CNoise::Smooth(float x)
{
	float Result;
	
	//	Smoothing curve. This is used to calculate interpolants so that the noise
	//	doesn't look blocky when the frequency is low.
	Result = x*x*(3 - 2*x);

	return Result;
}

float CNoise::Noise(float x, float y, float z)
{
	unsigned int ix, iy, iz;
	float fx0, fx1, fy0, fy1, fz0, fz1;
	float wx, wy, wz;
	float vx0, vx1, vy0, vy1, vz0, vz1;
	float Result;

	// The main noise function. Looks up the pseudorandom gradients at the nearest
	// lattice points, dots them with the input vector, and interpolates the
	// results to produce a single output value in [0, 1] range. }
	ix = (unsigned int)x;
	fx0 = x - ix;
	fx1 = fx0 - 1;
	wx = Smooth(fx0);
	iy = (unsigned int)y;
	fy0 = y - iy;
	fy1 = fy0 - 1;
	wy = Smooth(fy0);

	iz = (unsigned int)z;
	fz0 = z - iz;
	fz1 = fz0 - 1;
	wz = Smooth(fz0);

	vx0 = Lattice(ix, iy, iz, fx0, fy0, fz0);
	vx1 = Lattice(ix+1, iy, iz, fx1, fy0, fz0);
	vy0 = Lerp(wx, vx0, vx1);

	vx0 = Lattice(ix, iy+1, iz, fx0, fy1, fz0);
	vx1 = Lattice(ix+1, iy+1, iz, fx1, fy1, fz0);
	vy1 = Lerp(wx, vx0, vx1);

	vz0 = Lerp(wy, vy0, vy1);

	vx0 = Lattice(ix, iy, iz+1, fx0, fy0, fz1);
	vx1 = Lattice(ix+1, iy, iz+1, fx1, fy0, fz1);
	vy0 = Lerp(wx, vx0, vx1);

	vx0 = Lattice(ix, iy+1, iz+1, fx0, fy1, fz1);
	vx1 = Lattice(ix+1, iy+1, iz+1, fx1, fy1, fz1);
	vy1 = Lerp(wx, vx0, vx1);

	vz1 = Lerp(wy, vy0, vy1);

	Result = Lerp(wz, vz0, vz1);

	return Result;
}

