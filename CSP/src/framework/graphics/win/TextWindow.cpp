#include "TextWindow.h"
#include "Font.h"
#include "..\StandardGraphics.h"

TextWindow::TextWindow(StandardGraphics * parent, float left, float bottom, float right, float top)
{
//	parent->add(this);
	m_pParent = parent;
	m_text = new Font( "Font.bmp",1 );
	
	_Rect rect = m_pParent->GetCoordsRect();
	float width = right-left;
	float height = top-bottom;

	int clientWidth = rect.x2-rect.x1;
	int clientHeight = rect.y2-rect.y1;
	m_view->x1 = (int)(left*(float)clientWidth);
	m_view->y1 = (int)(top*(float)clientHeight);
	m_view->x2 = (int)(width*(float)clientWidth);
	m_view->y2 = (int)(height*(float)clientHeight);
	m_color = _Color(0,0,0);

}

void TextWindow::DrawText(const char * str)
{

//	glMatrixMode( GL_PROJECTION );
//	glPushMatrix();
//	glOrtho( m_view->x1, m_view->y1, m_view->x2, m_view->y2, -1.0f, 1.0f );
//	glLoadIdentity();
//	glMatrixMode( GL_MODELVIEW );
//	glPushMatrix();
	m_text->displayText( str, strlen(str), *m_view, 1, 
		&m_color );
//	glPopMatrix();
//	glMatrixMode( GL_PROJECTION );
//	glPopMatrix();

}

/** Begin.
 *  Make state changes needed for text. 
 */
void TextWindow::Begin()
{

}

/** End
 * Reset state
 */
void TextWindow::End()
{

}
