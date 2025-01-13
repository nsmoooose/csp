#include <csp/cspwf/ControlMoveEventHandler.h>
#include <iostream>

csp::wf::ControlMoveEventHandler::ControlMoveEventHandler(Control* control) : m_MouseDown(false), m_Drag(false) {
	m_Control = control;
	if(m_Control.valid()) {
		control->MouseDown.connect(sigc::mem_fun(*this, &ControlMoveEventHandler::handleMouseDown));
		control->MouseMove.connect(sigc::mem_fun(*this, &ControlMoveEventHandler::handleMouseMove));
		control->MouseUp.connect(sigc::mem_fun(*this, &ControlMoveEventHandler::handleMouseUp));
	}
}

csp::wf::ControlMoveEventHandler::~ControlMoveEventHandler() {
}

void csp::wf::ControlMoveEventHandler::handleMouseDown(MouseButtonEventArgs& event) {
	if(event.button == 1 && m_Control.valid()) {
		m_MouseDown = true;
		m_StartDrag.x = event.x;
		m_StartDrag.y = event.y;
		m_ControlLocation = m_Control->getLocation();
		event.handled = true;
	}
}

void csp::wf::ControlMoveEventHandler::handleMouseMove(MouseEventArgs& event) {
	if(m_MouseDown && m_Control.valid()) {
		if(m_Drag == false) {
			const int diff = 4;
			if(event.x < (m_StartDrag.x - diff) || event.x > (m_StartDrag.x + diff) ||
			   event.y < (m_StartDrag.y - diff) || event.y > (m_StartDrag.y + diff))
			{
				m_Drag = true;
			}
		}

		if(m_Drag) {
			Ref<Style> style = m_Control->getStyle();
			style->setLeft(optional<Style::UnitValue>(Style::UnitValue(Style::Pixels, m_ControlLocation.x + (event.x - m_StartDrag.x))));
			style->setTop(optional<Style::UnitValue>(Style::UnitValue(Style::Pixels, m_ControlLocation.y + (event.y - m_StartDrag.y))));
			m_Control->updateMatrix();
		
			event.handled = true;
		}
	}
}

void csp::wf::ControlMoveEventHandler::handleMouseUp(MouseButtonEventArgs& event) {
	if(m_MouseDown && m_Drag && m_Control.valid()) {
		event.handled = true;
	}

	m_MouseDown = false;
	m_Drag = false;
}
