///////////////////////////////////////////////////////////////////////////
//
//	Perlin Noise Demo
//  CSP - Combat Simulator Project
//	http://csp.homeip.net
//
//  main.cpp
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

#include "SDL.h"   /* All SDL App's need this */
#include "sdl_opengl.h"
#include <stdio.h>
#include <stdlib.h>
#include "noise.h"
#include "main.h"
#include "time.h"

#define TEX_SIZE 128

#pragma comment( lib, "opengl32.lib" )									// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )										// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )										// Search For GLaux.lib While Linking

void UpdateAndDraw(float z);

CNoise* Noize;
GLuint tex;
char texImage[3*TEX_SIZE*TEX_SIZE];
int z=0;

static void quit_tutorial( int code )
{
    /*
     * Quit SDL so we can release the fullscreen
     * mode and restore the previous video settings,
     * etc.
     */
    SDL_Quit( );

    /* Exit program. */
    exit( code );
}

static void handle_key_down( SDL_keysym* keysym )
{
    switch( keysym->sym ) {
    case SDLK_ESCAPE:
        quit_tutorial( 0 );
        break;
    default:
        break;
    }
}

static void process_events()
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
        case SDL_KEYDOWN:
            /* Handle key presses. */
            handle_key_down( &event.key.keysym );
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            quit_tutorial( 0 );
            break;
        }

    }

}

int main(int argc, char* argv[]) 
{
	GLuint tex;
	char texImage[3*TEX_SIZE*TEX_SIZE];
	float z=0.0f;

    SDL_Surface *screen;

    printf("Initializing SDL.\n");
    
    /* Initialize the SDL library */
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    printf("SDL initialized.\n");


    /* Clean up on exit */
    atexit(SDL_Quit);

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    if( SDL_SetVideoMode(512, 512, 16, SDL_OPENGL) == 0) 
	{
        /* 
         * This could happen for a variety of reasons,
         * including DISPLAY not being set, the specified
         * resolution not being available, etc.
         */
        fprintf(stderr, "Video mode set failed: %s\n",SDL_GetError());
		exit(1);
	}
        
	time_t ltime;
	time( &ltime );

	Noize = new CNoise(ltime % 99 * 3);
	InitOGL();

    while(1) 
	{
        /* Process incoming events. */
        process_events();
        /* Draw the screen. */
        UpdateAndDraw(z);
		z += .1f;
    }

    printf("Quiting SDL.\n");
    
    /* Shutdown all subsystems */
    SDL_Quit();
    
    printf("Quiting....\n");

    return 0;
}

