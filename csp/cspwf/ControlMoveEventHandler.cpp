#include <csp/cspwf/ControlMoveEventHandler.h>

csp::wf::ControlMoveEventHandler::ControlMoveEventHandler(Control* control) : m_Active(false) {
	m_Control = control;
	control->MouseDown.connect(sigc::mem_fun(this, &ControlMoveEventHandler::handleMouseDown));
	control->MouseMove.connect(sigc::mem_fun(this, &ControlMoveEventHandler::handleMouseMove));
	control->MouseUp.connect(sigc::mem_fun(this, &ControlMoveEventHandler::handleMouseUp));
}

void csp::wf::ControlMoveEventHandler::handleMouseDown(MouseButtonEventArgs& event) {
	if(event.button == 1 && m_Control.valid()) {
		m_Active = true;
		m_StartDrag.x = event.x;
		m_StartDrag.y = event.y;
		m_ControlLocation = m_Control->getLocation();
		event.handled = true;
	}
}

void csp::wf::ControlMoveEventHandler::handleMouseMove(MouseEventArgs& event) {
	if(m_Active && m_Control.valid()) {
		Ref<Style> style = m_Control->getStyle();
		style->setLeft(optional<Style::UnitValue>(Style::UnitValue(Style::Pixels, m_ControlLocation.x + (event.x - m_StartDrag.x))));
		style->setTop(optional<Style::UnitValue>(Style::UnitValue(Style::Pixels, m_ControlLocation.y + (event.y - m_StartDrag.y))));
		m_Control->updateMatrix();
		
		event.handled = true;
	}
}

void csp::wf::ControlMoveEventHandler::handleMouseUp(MouseButtonEventArgs& event) {
	if(m_Active && m_Control.valid()) {
		m_Active = false;
		
		event.handled = true;
	}
}
