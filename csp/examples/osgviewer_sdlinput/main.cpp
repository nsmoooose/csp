// Combat Simulator Project
// Copyright (C) 2009 The Combat Simulator Project
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

#include "main.h"
#include <SDL/SDL.h>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

namespace csp {
namespace osgviewer_sdlinput {

class SdlError
{
	public:
		SdlError(const char * context)
		{
			osg::notify(osg::FATAL) << "SDL Error in " << context << ": " << SDL_GetError() << std::endl;
		}
};

class SdlApplication
{
	public:
		SdlApplication()
		{
		}

		~SdlApplication()
		{
			SDL_Quit();
		}

		void init()
		{
			if ( SDL_putenv( "SDL_VIDEODRIVER=dummy" ) == -1 ) throw SdlError("SDL_putenv");

			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) throw SdlError("SDL_Init");

			SDL_EnableUNICODE( 1 );
			SDL_JoystickEventState( SDL_ENABLE );
		}

		void pollEvent()
		{
			SDL_Event sdlEvent;

			while ( SDL_PollEvent( &sdlEvent ) )
			{
				switch( sdlEvent.type )
				{
					case SDL_JOYAXISMOTION:
					{
						osg::notify(osg::NOTICE)
							<< "Joystick #" << static_cast<unsigned int>(sdlEvent.jaxis.which)
							<< " Axes #" << static_cast<unsigned int>(sdlEvent.jaxis.axis)
							<< " = " << sdlEvent.jaxis.value << std::endl;
						break;
					}

					case SDL_MOUSEMOTION:
					{
						osg::notify(osg::NOTICE) << "Mouse x = " << sdlEvent.motion.x << ", y = " << sdlEvent.motion.y << std::endl;
						break;
					}

					case SDL_KEYDOWN:
					case SDL_KEYUP:
					{
						osg::notify(osg::NOTICE) << "Key " << sdlEvent.key.keysym.unicode << std::endl;
						break;
					}
				}
			}
		}
};

class SdlJoystick : public osg::Referenced
{
	public:
		SdlJoystick(int index)
			: m_index(index)
		{
			const char * name = SDL_JoystickName( m_index );
			if ( !name ) throw SdlError("SDL_JoystickName");

			osg::notify(osg::NOTICE) << "Opening joystick #" << m_index << ": " << name << std::endl;

			m_joystick = SDL_JoystickOpen( m_index );

			if ( !m_joystick ) throw SdlError("SDL_JoystickOpen");
		}

		~SdlJoystick()
		{
			osg::notify(osg::NOTICE) << "Closing joystick #" << m_index << std::endl;
			SDL_JoystickClose( m_joystick );
		}

	private:
		int m_index;
		SDL_Joystick * m_joystick;
};

class SdlAllJoysticks
{
	public:
		SdlAllJoysticks()
		{
			osg::notify(osg::NOTICE) << "SDL_NumJoysticks=" << SDL_NumJoysticks() << std::endl;

			for (int joystickIndex = 0; joystickIndex < SDL_NumJoysticks(); ++joystickIndex )
			{
				m_allJoysticks.push_back( new SdlJoystick(joystickIndex) );
			}
		}

	private:
		std::list< osg::ref_ptr<SdlJoystick> > m_allJoysticks;
};

void Appli::run(const char * searchPath, const char * model)
{
	SdlApplication sdlApplication;
	sdlApplication.init();
	SdlAllJoysticks allJoysticks;

	osgDB::setDataFilePathList(searchPath);
	osg::ref_ptr<osg::Node> scene = osgDB::readNodeFile(model);
	if (!scene)
	{
		osg::notify(osg::FATAL) << "Model " << model << " not loaded" << std::endl;
		return;
	}

	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(100, 100, 400, 300);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.setSceneData( scene.get() );

	while (!viewer.done())
	{
		sdlApplication.pollEvent();
		viewer.frame();
	}
}

}	// end namespace osgviewer_sdlinput
}	// end namespace csp
