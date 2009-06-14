// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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

#include <csp/cspsim/SDLEventHandler.h>
#include <csp/cspsim/CSPSim.h>
#include <csp/cspsim/BaseScreen.h>
#include <SDL/SDL.h>

namespace csp {

SDLEventHandler::SDLEventHandler()
	: m_drag(false)
{
}

bool SDLEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	return false; //onEvent(ea);
}

bool SDLEventHandler::handle(const SDL_Event& sdlEvent)
{
	switch (sdlEvent.type)
	{
		case SDL_MOUSEMOTION:
		{
			unsigned int modifierMask = SDL_GetModState();
			m_drag = (sdlEvent.motion.state != 0); // Mimic osgGA drag detection
			input::RawEvent::MouseMotion mouseMotionEvent(sdlEvent.motion.state, modifierMask & 0xFFF, sdlEvent.motion.x, sdlEvent.motion.y, sdlEvent.motion.xrel, sdlEvent.motion.yrel);
			return onEvent(mouseMotionEvent);
		}

		case SDL_MOUSEBUTTONUP:
		{
			unsigned int buttonMask = SDL_GetMouseState(0, 0);
			unsigned int modifierMask = SDL_GetModState();
			input::RawEvent::MouseButton mouseButtonEvent(input::RawEvent::MouseButton::RELEASED, sdlEvent.button.button, buttonMask, modifierMask & 0xFFF, sdlEvent.button.x, sdlEvent.button.y, m_drag);
			if ( buttonMask == 0 ) m_drag = false;
			return onEvent(mouseButtonEvent);
		}

		case SDL_MOUSEBUTTONDOWN:
		{
			unsigned int buttonMask = SDL_GetMouseState(0, 0);
			unsigned int modifierMask = SDL_GetModState();
			input::RawEvent::MouseButton mouseButtonEvent(input::RawEvent::MouseButton::PRESSED, sdlEvent.button.button, buttonMask, modifierMask & 0xFFF, sdlEvent.button.x, sdlEvent.button.y, m_drag);
			return onEvent(mouseButtonEvent);
		}

		case SDL_KEYUP:
		{
			input::RawEvent::Keyboard keyboardEvent(input::RawEvent::Keyboard::RELEASED, sdlEvent.key.keysym.sym, sdlEvent.key.keysym.mod & 0xFFF);
			input::HID::translate(keyboardEvent);
			return onEvent(keyboardEvent);
		}

		case SDL_KEYDOWN:
		{
			input::RawEvent::Keyboard keyboardEvent(input::RawEvent::Keyboard::PRESSED, sdlEvent.key.keysym.sym, sdlEvent.key.keysym.mod & 0xFFF);
			input::HID::translate(keyboardEvent);
			return onEvent(keyboardEvent);
		}

		case SDL_JOYAXISMOTION:
		{
			input::RawEvent::JoystickAxis JoystickAxisEvent(sdlEvent.jaxis.which, sdlEvent.jaxis.axis, sdlEvent.jaxis.value / 32768.0);
			return onEvent(JoystickAxisEvent);
		}

		case SDL_JOYBUTTONUP:
		{
			input::RawEvent::JoystickButton joystickButtonEvent(input::RawEvent::JoystickButton::RELEASED, sdlEvent.jbutton.which, sdlEvent.jbutton.button);
			return onEvent(joystickButtonEvent);
		}

		case SDL_JOYBUTTONDOWN:
		{
			input::RawEvent::JoystickButton joystickButtonEvent(input::RawEvent::JoystickButton::PRESSED, sdlEvent.jbutton.which, sdlEvent.jbutton.button);
			return onEvent(joystickButtonEvent);
		}
	}

	return false;
}

void SDLEventHandler::runInputScripts()
{
	runCurrentScreenInputScript();
	runActiveObjectInputScript();
}

void SDLEventHandler::runCurrentScreenInputScript()
{
	Ref<input::HID> currentScreenInterface = getCurrentScreenInterface();
	if ( !currentScreenInterface ) return;

	currentScreenInterface->onUpdate( CSPSim::theSim->getFrameTime() );
}

void SDLEventHandler::runActiveObjectInputScript()
{
	Ref<input::HID> activeObjectInterface = getActiveObjectInterface();
	if ( !activeObjectInterface ) return;

	activeObjectInterface->onUpdate( CSPSim::theSim->getFrameTime() );
}

Ref<input::HID> SDLEventHandler::getCurrentScreenInterface()
{
	Ref<BaseScreen> currentScreen = CSPSim::theSim->getCurrentScreen();
	if ( !currentScreen ) return 0;

	return currentScreen->getInterface();
}

Ref<input::HID> SDLEventHandler::getActiveObjectInterface()
{
	return CSPSim::theSim->getActiveObjectInterface();
}

} // namespace csp
