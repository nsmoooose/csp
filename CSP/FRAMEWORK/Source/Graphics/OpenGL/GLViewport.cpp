#ifdef OGL

#include "GLViewport.h"

GLViewport::GLViewport()
{
  m_left = 0;
  m_bottom = 0;
  m_width = 0;
  m_height = 0;

}


GLViewport::~GLViewport()
{

}

short GLViewport::Initialize(short left, short bottom, unsigned short Width, unsigned short Height)
{

  m_left = left;
  m_bottom = bottom;
  m_width = Width;
  m_height = Height;
  
  return 0;
}


  // functions
short  GLViewport::Initialize(StandardGraphics *Graphics, 
                              unsigned short Width, unsigned short Height)
{



 return 0;
}

void GLViewport::Uninitialize()
{

}

void GLViewport::SetViewport()
{
	glViewport(m_left, m_bottom, m_width, m_height);
}

void* GLViewport::GetSurface()
{
  return 0 ;
}


StandardViewport* GLViewport::CreateViewport()
{
  return 0;
}

void GLViewport::BeginScene()
{

}

void GLViewport::EndScene()
{

}

void GLViewport::Flip()
{

}
 
void GLViewport::Clear()
{

}

void GLViewport::DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, StandardColor*color )
{

}

#endif
