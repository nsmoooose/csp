///////////////////////////////////////////////////////////////////////////
//
//	Perlin Noise Demo
//  CSP - Combat Simulator Project
//	http://csp.homeip.net
//
//  Header file of Class Noise.cpp
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	06/16/2002	file created	-	Stormbringer
//
//  The code provided here is derived from a Delphi Demo
//  by Tom Nuydens at http://www.delphi3d.net/
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


#if !defined(NOISE_H)
#define NOISE_H

#define		GRADIENT_TABLE_SIZE 256
#define		PI 3.1415

class CNoise
{
protected:
    float FGradients[GRADIENT_TABLE_SIZE*3];
    void InitGradients();
    unsigned int Permutate(unsigned int x);
    unsigned int Index(unsigned int ix, unsigned int iy, unsigned int iz);
    float Lattice(unsigned int ix, unsigned int iy, unsigned int iz, float fx, float fy, float fz);
    float Lerp(float t, float x0, float x1);
    float Smooth(float x);
public:
    // Only two public functions, so it should be easy enough to use!
    CNoise(unsigned int seed);
	~CNoise();
    float Noise(float x, float y, float z);
	float GetGradient(int index);
};

#endif