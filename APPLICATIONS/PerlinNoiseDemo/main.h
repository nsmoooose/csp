///////////////////////////////////////////////////////////////////////////
//
//	Perlin Noise Demo
//  CSP - Combat Simulator Project
//	http://csp.homeip.net
//
//  Header file of main.cpp
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

void InitOGL();