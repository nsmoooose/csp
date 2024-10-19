#include <osgGA/GUIEventAdapter>
#include <osgViewer/Viewer>
#include <SDL2/SDL.h>
#include "SDLViewer.h"

bool convertEvent(SDL_Event& event, osgGA::EventQueue& eventQueue)
{
    switch (event.type) {

        case SDL_MOUSEMOTION:
            eventQueue.mouseMotion(event.motion.x, event.motion.y);
            return true;

        case SDL_MOUSEBUTTONDOWN:
            eventQueue.mouseButtonPress(event.button.x, event.button.y, event.button.button);
            return true;

        case SDL_MOUSEBUTTONUP:
            eventQueue.mouseButtonRelease(event.button.x, event.button.y, event.button.button);
            return true;

        case SDL_KEYUP:
			if(event.key.keysym.sym == SDLK_LEFT)
				eventQueue.keyRelease(osgGA::GUIEventAdapter::KEY_Left);
			else if(event.key.keysym.sym == SDLK_UP)
				eventQueue.keyRelease(osgGA::GUIEventAdapter::KEY_Up);
			else if(event.key.keysym.sym == SDLK_DOWN)
				eventQueue.keyRelease(osgGA::GUIEventAdapter::KEY_Down);
			else if(event.key.keysym.sym == SDLK_RIGHT)
				eventQueue.keyRelease(osgGA::GUIEventAdapter::KEY_Right);
			else
				eventQueue.keyRelease( (osgGA::GUIEventAdapter::KeySymbol) event.key.keysym.unicode);
            return true;

        case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_LEFT)
				eventQueue.keyPress(osgGA::GUIEventAdapter::KEY_Left);
			else if(event.key.keysym.sym == SDLK_UP)
				eventQueue.keyPress(osgGA::GUIEventAdapter::KEY_Up);
			else if(event.key.keysym.sym == SDLK_DOWN)
				eventQueue.keyPress(osgGA::GUIEventAdapter::KEY_Down);
			else if(event.key.keysym.sym == SDLK_RIGHT)
				eventQueue.keyPress(osgGA::GUIEventAdapter::KEY_Right);
			else
				eventQueue.keyPress( (osgGA::GUIEventAdapter::KeySymbol) event.key.keysym.unicode);
            return true;

        case SDL_WINDOWEVENT_RESIZED:
            eventQueue.windowResize(0, 0, event.window.data1, event.window.data2 );
            return true;

        default:
            break;
    }
    return false;
}

namespace csp {

SDLViewer::SDLViewer() : m_Initialized(false) {
}

void SDLViewer::initialize() {
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

	SDL_Surface *m_SDLScreen = SDL_SetVideoMode(800, 600, 0, 
								   SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (m_SDLScreen == NULL) {
		exit(1);
	}
	SDL_WM_SetCaption("CSP example application", "");

    m_GraphicsWindow = m_Viewer.setUpViewerAsEmbeddedInWindow(0, 0, 800, 600);
    m_Viewer.realize();
	
	m_Initialized = true;
}

osgViewer::Viewer* SDLViewer::getViewer() {
	return &m_Viewer;
}	

void SDLViewer::run() {
	if(!m_Initialized) {
		initialize();
	}

    bool done = false;
    while( !done )
    {
        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            // pass the SDL event into the viewers event queue
            convertEvent(event, *(m_GraphicsWindow->getEventQueue()));

            switch (event.type) {

                case SDL_WINDOWEVENT_RESIZED:
                    SDL_SetVideoMode(event.window.data1, event.window.data2, 0, SDL_OPENGL | SDL_RESIZABLE);
                    m_GraphicsWindow->resized(0, 0, event.resize.w, event.resize.h );
                    break;

                case SDL_QUIT:
                    done = true;
            }
        }

        if (done) 
			break;

        m_Viewer.frame();
        SDL_GL_SwapBuffers();
    }
}

} // namespace csp
