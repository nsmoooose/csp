#ifndef __CSPSIM_WF_CONTROLMOVEEVENTHANDLER_H__
#define __CSPSIM_WF_CONTROLMOVEEVENTHANDLER_H__

#include <csp/cspwf/Control.h>

namespace csp {
namespace wf {

class CSPWF_EXPORT ControlMoveEventHandler : public Referenced, public sigc::trackable {
public:
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

#endif // __CSPSIM_WF_CONTROLMOVEEVENTHANDLER_H__
