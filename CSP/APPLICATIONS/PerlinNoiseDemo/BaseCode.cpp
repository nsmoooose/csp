///////////////////////////////////////////////////////////////////////////
//
//	Perlin Noise Demo
//  CSP - Combat Simulator Project
//	http://csp.homeip.net
//
//  BaseCode.cpp - contains basic drawing functions 
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

#include "sdl.h"
#include "sdl_opengl.h"
#include "Noise.h"

#ifndef TEX_SIZE 
#define TEX_SIZE 128
#endif

extern GLuint tex;
extern char texImage[3*TEX_SIZE*TEX_SIZE];
extern CNoise* Noize;

void InitOGL()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TEX_SIZE, TEX_SIZE, 0, GL_RGB,
				 GL_UNSIGNED_BYTE, &texImage[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return;
}

void UpdateAndDraw(float z)
{
	int x, y;
	unsigned char n;
	float nf, nf2;

  // Update the noise texture.
  for(y=0;y<TEX_SIZE;y++)
  {
    for(x=0;x<TEX_SIZE;x++)
    {
		// Take 4 octaves of noise and add them.
		nf = Noize->Noise((float)x/32, (float)y/32, z);
		nf = nf + Noize->Noise((float)x/16, (float)y/16, z)/2;
		nf = nf + Noize->Noise((float)x/8, (float)y/8, z)/4;
		nf = nf + Noize->Noise((float)x/4, (float)y/4, z)/8;
		// Write the result to the texture image.
		nf2 = 255 * (nf+1)/2;
		n = (unsigned char)nf2;
		texImage[(y*TEX_SIZE*3)+(x*3)] = n;
		texImage[(y*TEX_SIZE*3)+(x*3)+1] = n/2;
		texImage[(y*TEX_SIZE*3)+(x*3)+2] = 255 - n;
    }
  }

  // Update the texture.
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_SIZE, TEX_SIZE, GL_RGB,
                  GL_UNSIGNED_BYTE, &texImage[0]);

  // Render.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);    glVertex2f(-1, -1);
    glTexCoord2f(1, 0);    glVertex2f(1, -1);
    glTexCoord2f(1, 1);    glVertex2f(1, 1);
    glTexCoord2f(0, 1);    glVertex2f(-1, 1);
  glEnd();

  SDL_GL_SwapBuffers();
  return;
}
