///////////////////////////////////////////////////////////////////////////
//
//   GLGraphics  - CSP - http://csp.homeip.net
//
//   coded by Wolverine and GKW
//
///////////////////////////////////////////////////////////////////////////
//   
//   This is the top level class of the OpenGL renderer.
//   This class implements the StandardGraphics abstract interface.
//   All other graphics objects (Mesh, Textures..) will
//   be created by this class. Also graphics state changes
//   From the application and upper level libraries will
//   come through this class.
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
///////////////////////////////////////////////////////////////////////////





#ifdef OGL

#include "GLGraphics.h"
#include "GLFont.h"
#include "GLGlobals.h"
#include "GLMaterial.h"
#include "GLViewport.h"
#include "GLTexture.h"
#include "Bitmap.h"

//extern GLStateMachine	statesGL;

extern void CheckGLAndSDLErrors();

/** 
 * \fn GLGraphics::GLGraphics()
 * \brief Contructor for GLGraphics
 */
GLGraphics::GLGraphics()
{
	CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::GLGraphics()");

	m_text = NULL;
	p_Camera = NULL;
}

/** 
 * \fn GLGraphics::~GLGraphics()
 * \brief Destructor for GLGraphics
 */
GLGraphics::~GLGraphics()
{
	CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::~GLGraphics()");

}

///////////////////////////////////////////
// whole buffer routines

/**
 * \fn int GLGraphics::GetRenderState(StateTypes stateType)
 * \brief Queries a specific stateType. Current not working.
 */
int GLGraphics::GetRenderState(StateTypes stateType)
{
	CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::GetRenderState() - "
		<< stateType);

	return 0;
}

/**
 * \fn void GLGraphics::SetRenderState(StateTypes stateType, int state)
 * \brief Set a RenderState.
 * \param stateType The state type to set.
 * \param state The new state to set. This meaning depends on the stateType.
 *
 * The current supported states to set are
 * FW_WINDING - This determines the polygon winding (clockwise or counterclockwise).
 * FW_SHADING - This determine the shading model to use.
 * FW_LIGHTING - This activates or deactivates lighting.
 * FW_ZBUFFER - This activates or deactivates zbuffering.
 * FW_CULLFACE - This sets the polygon face to cull (remove). Can be front or back 
 *               the meaning of these depends on the polygon winding (FW_WINDING).
 * FW_TEXTURE - This activates or deactivates texture mapping. To use texturing 
 *              a texture object must also be set.
 * FW_BLENDING - This activates or deactivates blending.
 * FW_WIREFRAME - This activates or deactivates wireframe mode.
 * FW_FOG - This activate or deactivates fog mode.
 * FW_FOGMODE - Sets the Fog Mode. Possible states are FW_FOG_EXP, FW_FOG_EXP2, and FW_FOG_LINEAR
 * FW_FOGSTART - The distance to start fogging.
 * FW_FOGEND - The distance to end fogging.
 * FW_FOGDENSITY - The density of the fog.
 * FW_FOGCOLOR - The color of the fog.
 */
void GLGraphics::SetRenderState(StateTypes stateType, int state)
{
	CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetRenderState() - "
		<< stateType << ", " << state);


	switch (stateType)
	{
	case FW_WINDING:
		if (state == FW_CW)
			glFrontFace(GL_CW);
		else if (state == FW_CCW)
			glFrontFace(GL_CCW);
	break;
	
	case FW_SHADING:
		if (state == FW_SMOOTH)
		{
			glShadeModel(GL_SMOOTH);
		}
		else if (state == FW_FLAT)
		{
			glShadeModel(GL_FLAT);
		}
	break;

	case FW_LIGHTING:
		if (state == (int)true)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
	break;
	
	case FW_ZBUFFER:
		{
			GLStateMachine::SetZBuffer(state);
		}
	break;
	case FW_CULLFACE:
		if (state == FW_FRONTFACE)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
		else if (state == FW_BACKFACE)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
	break;
	case FW_TEXTURE:
		if (state == true)
		{
			glEnable( GL_TEXTURE_2D );
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}
	break;
	case FW_BLENDING:
		if (state == true)
		{
			glEnable( GL_BLEND );
		}
		else
		{
			glDisable (GL_BLEND);
		}
	break;
	case FW_WIREFRAME:
		{
		GLStateMachine::SetWireframe(state);
		}
		break;
	case FW_FOG:
		if (state = true)
		{
			glEnable(GL_FOG);
		}
		else
		{
			glDisable(GL_FOG);
		}
	break;
	case FW_FOGMODE:
		{
			switch (state)
			{
			case FW_FOG_EXP:
				{
					glFogi(GL_FOG_MODE, GL_EXP);
				}
			break;
				
			case FW_FOG_EXP2:
				{
					glFogi(GL_FOG_MODE, GL_EXP2);
				}
			break;

			case FW_FOG_LINEAR:
				{
					glFogi(GL_FOG_MODE, GL_LINEAR);
				}
			break;
			
			}

		}
	break;
	case FW_FOGSTART:
		{
			float fogStart = *(float*)&state;
			glFogf(GL_FOG_START, fogStart);
		}
	break;
	case FW_FOGEND:
		{
			float fogEnd = *(float*)&state;
			glFogf(GL_FOG_END, fogEnd);
		}
	break;
	case FW_FOGDENSITY:
		{
			float fogDensity = *(float*)&state;
			glFogf(GL_FOG_DENSITY, fogDensity);
		}
	break;
	case FW_FOGCOLOR:
		{
			StandardColor * color = (StandardColor*)state;
			glFogfv(GL_FOG_COLOR, (float*)color);

		}
	break;
	}

	
}

/**
 * \fn void GLGraphics::SetFogColor(StandardColor *color)
 * \brief Sets the fog color.
 * \param color the color of the fog to use.
 */
void GLGraphics::SetFogColor(StandardColor *color)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetFogColor() "
		<< color);

  glFogfv(GL_FOG_COLOR, (float*)color);
}

/**
 * \fn void GLGraphics::Clear()
 * \brief Clears the graphics display, also clears the Depth (Z) Buffer if it is use.
 */
void GLGraphics::Clear()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::Clear()");

  glClear(GL_COLOR_BUFFER_BIT | (m_depthFlag ? GL_DEPTH_BUFFER_BIT : 0));

}

/**
 * \fn StandardViewport * GLGraphics::CreateViewport() 
 * \brief Creates a new viewport class.
 */
StandardViewport * GLGraphics::CreateViewport() 
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::CreateViewport()");

  GLViewport * pViewport = new GLViewport(); 
  return pViewport;
}

/**
 * \fn void GLGraphics::ModifyViewport(float left, float top, float width, float height) 
 * \brief Modifies a viewport. Not currently working.
 */
void GLGraphics::ModifyViewport(float left, float top, float width, float height) 
{
}

/**
 * \fn void GLGraphics::SetCamera(StandardCamera *Camera)
 * \brief Sets the active camera for the graphics display.
 * \param Camera The camera object to set. The camera must already be positioned.
 */
void GLGraphics::SetCamera(StandardCamera *Camera)
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetCamera()");

  p_Camera = Camera;
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((float*)p_Camera->GetProjectionMatrix());
  glMatrixMode(GL_MODELVIEW);

}

/** 
 * \fn void GLGraphics::SetViewport(StandardViewport* pViewport)
 * \brief Sets the active viewport.
 * \param pViewport The viewport to set.
 */
void GLGraphics::SetViewport(StandardViewport* pViewport)
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetViewport()");

  GLViewport * pGLViewport = (GLViewport*)pViewport;
  if (pGLViewport)
    pGLViewport->SetViewport();
}

/**
 * \fn void GLGraphics::DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, short TopCenterBottom, StandardColor *color )
 * \brief Draws out text to the graphics display.
 * \param string The string to print out.
 * \param rect The surrounding rectangle around the text.
 * \param horiz_center a flag specifying the horiz centering.
 * \param vert_center a flag specifying the vert centering.
 * \param textColor The color of the text.
 */
void GLGraphics::DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, short TopCenterBottom, StandardColor *color )
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GL_Graphics::DrawTextF - " << buffer);

  // get the screen rect to use to display the text.
  rect.x1 = (rect.x1 * (float)p_Init.Width);
  rect.x2 = (rect.x2 * (float)p_Init.Width);
  rect.y1 = (rect.y1 * (float)p_Init.Height);
  rect.y2 = (rect.y2 * (float)p_Init.Height);

  // call the font to actually draw the text.
  m_text->DrawTextF( buffer, rect, LeftCenterRight, TopCenterBottom, color );

}

/**
 * \fn StandardPolygonMesh * GLGraphics::CreatePolygonMesh() 
 * \brief Create a new PolygonMesh object for use with the OpenGL renderer.
 */
StandardPolygonMesh * GLGraphics::CreatePolygonMesh() 
{ 
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "StandardPolygonMesh * GLGraphics::CreatePolygonMesh()");

  GLPolygonMesh * pObject = new GLPolygonMesh(); 
  return pObject;
}

/** 
 * \fn StandardPolygonFlat* GLGraphics::CreatePolygonFlat()
 * \brief Create a new PolygonFlat object for use with the OpenGL renderer.
 */
StandardPolygonFlat* GLGraphics::CreatePolygonFlat()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "StandardPolygonFlat* GLGraphics::CreatePolygonFlat()" );

  GLPolygonFlat *polygonMesh = new GLPolygonFlat(this);

  return polygonMesh;
}

/**
 * \fn StandardPolygonBillboard* GLGraphics::CreatePolygonBillboard()
 * \brief Create a new PolygonBillboard object for use with the OpenGL renderer.
 */
StandardPolygonBillboard* GLGraphics::CreatePolygonBillboard()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::CreatePolygonBillboard()");

  GLPolygonBillboard *polygonBillboard = new GLPolygonBillboard();
    return polygonBillboard;

}

/**
 * \fn StandardMaterial * GLGraphics::CreateMaterial()
 * \brief Create a new Material object for use with the OpenGL renderer.
 */
StandardMaterial * GLGraphics::CreateMaterial()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::CreateMaterial()");

  GLMaterial * pObject = new GLMaterial();
  return pObject;

}

/** 
 * \fn StandardCamera * GLGraphics::CreateCamera()
 * \brief Create a new Camera object for use with the OpenGL renderer.
 */
StandardCamera * GLGraphics::CreateCamera()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::CreateCamera()");

  GLCamera * pObject = new GLCamera();
  return pObject;
}

/**
 * \fn StandardTexture * GLGraphics::CreateTexture()
 * \brief Create a new Texture object to be used with the OpenGL renderer.
 */
StandardTexture * GLGraphics::CreateTexture()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::CreateTexture()");

  GLTexture * pObject = new GLTexture();
  return pObject;
}

/**
 * \fn unsigned long GLGraphics::DrawPolygonMesh(StandardPolygonMesh * mesh)
 * \brief Draw a PolygonMesh to the graphics rendering surface.
 * \param mesh The mesh object to draw.
 * \return The number of polygon drawn.
 */
unsigned long GLGraphics::DrawPolygonMesh(StandardPolygonMesh * mesh)
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::DrawPolygonMesh(StandardPolygonMesh * mesh)");

  if (mesh)
    {
      unsigned long count = mesh->Draw();
      return count;
    }

  return 0;
}

/**
 * \fn void GLGraphics::DrawTriangle(StandardVector3 v1, StandardVector3 v2, StandardVector3 v3)
 * \brief Draws an individual triangle.
 * \param v1 - The first point of the triangle.
 * \param v2 - The second point of the triangle.
 * \param v3 - The third point of the thiangle.
 */
void GLGraphics::DrawTriangle(StandardVector3 v1, StandardVector3 v2, 
			      StandardVector3 v3)
{
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
}

/**
 * \fn void GLGraphics::BeginTriangles()
 * \brief Begin drawing a group of triangles.
 */
void GLGraphics::BeginTriangles()
{
	glBegin(GL_TRIANGLES);
}

/**
 * \fn void GLGraphics::EndTriangles()
 * \brief End drawing a group of triangles.
 */
void GLGraphics::EndTriangles()
{
	glEnd();
}

/**
 * \fn void GLGraphics::DrawViewport()
 * \brief Draws a viewport. Not currently working.
 */
void GLGraphics::DrawViewport()
{

}

/**
 * \fn void GLGraphics::SetTexture(StandardTexture * texture)
 * \brief Sets the current texture on the rendering device.
 * \param texture The texture object to set. Can be null which clears the texture.
 */
void GLGraphics::SetTexture(StandardTexture * texture)
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetTexture()");

  GLTexture * pGLTexture = (GLTexture*)texture;

  // set the current texture.
  if (pGLTexture)
    pGLTexture->Apply();
  else
    glBindTexture(GL_TEXTURE_2D, 0);

}

/** 
 * \fn void GLGraphics::SetMaterial(StandardMaterial * material)
 * \brief Sets the Current Material object on the graphics rendering device.
 * \param material The Material object to set on the graphics rendering device.
 */

void GLGraphics::SetMaterial(StandardMaterial * material)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetMaterial()");

  GLMaterial * pGLMaterial = (GLMaterial*)material;
  if (pGLMaterial)
    pGLMaterial->Apply();
}

/**
 * \fn void GLGraphics::SetBackgroundColor(StandardColor *  color)
 * \brief Sets the Current background color on the graphics rendering device.
 * \param color The Color object to set as the background.
 */
void GLGraphics::SetBackgroundColor(StandardColor *  color)
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetBackgroundColor()");

  glClearColor((float)color->r/255.0f, (float)color->g/255.0f, 
		(float)color->b/255.0f, (float)color->a/255.0f);
}

/**
 * \fn _GRAPHICS_INIT_STRUCT*    GLGraphics::GetInit()
 * \brief returns a pointer to the Graphics Init structure.
 */
_GRAPHICS_INIT_STRUCT*    GLGraphics::GetInit()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::GetInit()");
    
  return &p_Init;
}

/**
 * \fn void* GLGraphics::GetDevice()
 * \brief This is a dummy function for compatibility with the DirectX Graphics
 */
void* GLGraphics::GetDevice()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::GetDevice()");
  return 0;
}

/**
 * \fn void* GLGraphics::GetFont()
 * \brief Not current working. 
 */
void* GLGraphics::GetFont()
{
  return 0;
}

/**
 * \fn void* GLGraphics::GetZSurfaceFormat()
 * \brief Not current working. 
 */
void * GLGraphics::GetZSurfaceFormat()
{
  return 0;
}

/**
 * \fn void* GLGraphics::GetSurfaceFormat()
 * \brief Not current working. 
 */
void* GLGraphics::GetSurfaceFormat()
{
  return 0;
}

/** 
 * \fn void GLGraphics::SetAmbientColor(StandardColor *color)
 * \brief Sets the Ambient color on the graphics rendering device.
 * \param color The color to set as the ambient color.
 */
void GLGraphics::SetAmbientColor(StandardColor *color)
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLGraphics::SetAmbientColor() - color " << color);

  int c[4];
  if (color) 
    {
      c[0] = color->r; c[1] = color->g; c[2] = color->b; c[3] = color->a;
      glMaterialiv(GL_FRONT, GL_AMBIENT, c);
    }
}

/**
 * \fn void* GLGraphics::SetGamma(StandardColor *color)
 * \brief Not current working. 
 */
void GLGraphics::SetGamma(StandardColor *color)
{


}


#endif
