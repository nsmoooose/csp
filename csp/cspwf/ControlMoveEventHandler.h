#pragma once

#include <csp/cspwf/Control.h>

namespace csp {
namespace wf {

/** Provides functionality to move a Control with the mouse. This
 * class can be instanciated with any Control as a constructor parameter.
 * All the needed events to support the drag n drop operation is attached.
 * Common usage is to use this class together with windows to support
 * window movement.
 */
class CSPWF_EXPORT ControlMoveEventHandler : public Referenced, public sigc::trackable {
public:
	/** Attach this event handler to the control.
	 */
	ControlMoveEventHandler(Control* control);
	virtual ~ControlMoveEventHandler();

protected:
	bool m_MouseDown;
	bool m_Drag;
	Point m_StartDrag;
	Point m_ControlLocation;
	WeakRef<Control> m_Control;

	virtual void handleMouseDown(MouseButtonEventArgs& event);
	virtual void handleMouseMove(MouseEventArgs& event);
	virtual void handleMouseUp(MouseButtonEventArgs& event);
};

} // namespace wf
} // namespace csp
