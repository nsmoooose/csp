#ifndef __TEXTWINDOW_H__
#define __TEXTWINDOW_H__
#include "Font.h"
#include "ChildWindow.h"

class TextWindow : public ChildWindow
{

public:
	TextWindow(StandardGraphics * parent, float left, float bottom, float right, float top);
	void Begin();
	void End();

	void DrawText(const char * str);

private:
	Font	*m_text;
	_Rect * m_view;
	_Color m_color;


};

#endif 