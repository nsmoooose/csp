#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "graphics/win/openglgraphics.h"

void main(int argc, char * argv[])
{
	OpenGLGraphics graphics1;
	graphics1.Create3DWindow("Test Window", 0, 0, 320,240, 32, false);
//	OpenGLGraphics graphics2;
//	graphics2.Create3DWindow("Test Window", 0, 400, 320,240, 32, false);
	while(1)
	{
		graphics1.BeginScene();
		graphics1.SetIdentity();
		graphics1.Translate(0.0f, 0.0f, -5.0f);
		graphics1.SetColor(1.0f, 0.0f, 0.0f);
		graphics1.DrawRect(-2.0f, -2.0f, 2.0f, 2.0f);
		graphics1.Flip();
		graphics1.EndScene();

//		graphics2.BeginScene();
//		graphics2.SetIdentity();
//		graphics1.Translate(0.0f, 0.0f, -15.0f);
//		graphics1.SetColor(0.0f, 0.0f, 1.0f);
//		graphics1.DrawRect(-2.0f, -2.0f, 2.0f, 2.0f);
//		graphics2.Flip();
//		graphics2.EndScene();
	}
}