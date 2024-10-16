#pragma once
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

namespace csp {
namespace input {

namespace RawEvent {

	struct Keyboard {
		enum Type {
			RELEASED,
			PRESSED
		};

		Keyboard(Type aType, unsigned int aKey, unsigned int aModifierMask)
			: type(aType), key(aKey), modifierMask(aModifierMask) {}

		Type type;
		unsigned int key;
		unsigned int modifierMask;
	};

	struct JoystickButton {
		enum Type {
			RELEASED,
			PRESSED
		};

		JoystickButton(Type aType, unsigned int aJoystick, unsigned int aButton)
			: type(aType), joystick(aJoystick), button(aButton) {}

		Type type;
		unsigned int joystick;
		unsigned int button;
	};

	struct JoystickAxis {
		JoystickAxis(unsigned int aJoystick, unsigned int aAxis, double aValue)
			: joystick(aJoystick), axis(aAxis), value(aValue) {}

		unsigned int joystick;
		unsigned int axis;
		double value;
	};

	struct MouseButton {
		enum Type {
			RELEASED,
			PRESSED,
			DOUBLECLICK
		};

		/** See csp/tools/hid/maps/mouse.map */
		enum WheelButtons {
			WHEELUP   = 1<<3,
			WHEELDOWN = 1<<4
		};

		MouseButton(Type aType, unsigned int aButton, unsigned int aButtonMask, unsigned int aModifierMask, float aX, float aY, bool aDrag)
			: type(aType), button(aButton), buttonMask(aButtonMask), modifierMask(aModifierMask), x(aX), y(aY), drag(aDrag) {}

		Type type;
		unsigned int button;
		unsigned int buttonMask;
		unsigned int modifierMask;
		float x;
		float y;
		bool drag;
	};

	struct MouseMotion {
		MouseMotion(unsigned int aButtonMask, unsigned int aModifierMask, float aX, float aY, float aDx, float aDy)
			: buttonMask(aButtonMask), modifierMask(aModifierMask), x(aX), y(aY), dx(aDx), dy(aDy) {}

		unsigned int buttonMask;
		unsigned int modifierMask;
		float x;
		float y;
		float dx;
		float dy;
	};

} // namespace RawEvent

} // namespace input
} // namespace csp
