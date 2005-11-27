// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file InputEventChannel.h
 *
 **/


#ifndef __INPUTEVENTCHANNEL_H__
#define __INPUTEVENTCHANNEL_H__

#include <Bus.h>
#include <InputInterface.h>


/** A specialized channel for accessing input events from the system bus.
 *  Provides a general signal mechanism similar to a push data channel, but
 *  unlike a DataChannel has no internal state.  The channel is bound to
 *  a specific input event which triggers the signal.  The signal may also
 *  be triggered directly using the signal() method.
 *
 *  An example that motivated the creation of this class is the ICP interface
 *  of the F16.  Each button has a corresponding input event, which needs to
 *  be handled by both the system that implements the ICP functionality and
 *  the animation callback that drives the button animation in the 3D cockpit.
 *  Similar dispatch must occur when the user clicks on the button in the 3D
 *  cockpit.
 *
 *  The system creates an InputEventChannel for each button that is bound to
 *  the corresponding input event (typically a keypress).  Both the system and
 *  the animation callback register handlers for the event.  When an animation
 *  callback detects a click on its button, it manually triggers the event
 *  signal.  From the perspective of the handlers, both event types are
 *  equivalent.
 */
class InputEventChannel: public ChannelBase, public sigc::trackable {
public:
	typedef simdata::Ref<InputEventChannel> Ref;
	typedef simdata::Ref<const InputEventChannel> CRef;

	/** Construct a new InputEventChannel bound to a specific input event.  The
	 *  channel name must be the same as the event name.  The input interface
	 *  will typically be the System subclass that registers the channnel.
	 */
	InputEventChannel(std::string const &name, InputInterface *input_interface): ChannelBase(name, ACCESS_SHARED) {
		input_interface->bindActionEvent(name, sigc::mem_fun(*this, &InputEventChannel::onInputEvent));
	}
	/** A convenience method to construct an InputEventChannel and bind a callback
	 *  method in one step.  Useful when the System creating and registering the
	 *  channel does not need to retain a private reference.  Note that the instance
	 *  class must be a subclass sigC::trackable (and that System is not).
	 */
	template <class C>
	InputEventChannel(std::string const &name, C* instance, void (C::*method)()): ChannelBase(name, ACCESS_SHARED) {
		connect(instance, method);
		instance->bindActionEvent(name, sigc::mem_fun(*this, &InputEventChannel::onInputEvent));
	}

	/** Connect a new signal handler to the channel.  The handler will be called
	 *  whenever the corresponding input event is received, and when the signal()
	 *  method is called.
	 */
	template <class C>
	sigc::connection connect(C *object, void (C::*callback)()) const {
		return m_Signal.connect(sigc::mem_fun(*object, callback));
	}

	/** Connect a new signal handler to the channel.  The handler will be called
	 *  whenever the corresponding input event is received, and when the signal()
	 *  method is called.
	 */
	sigc::connection connect(sigc::slot<void> const &functor) const {
		return m_Signal.connect(functor);
	}

	/** Signal all connected handlers.
	 */
	void signal() const { m_Signal.emit(); }

protected:
	virtual ~InputEventChannel() { }

private:
	void onInputEvent() { signal(); }
	mutable sigc::signal<void> m_Signal;
};


#endif // __INPUTEVENTCHANNEL_H__

