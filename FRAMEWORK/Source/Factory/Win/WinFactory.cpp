#include "Framework.h"

#include "WinEndian.h"
#include "WinBuff.h"
#include "WinFile.h"
#include "WinWindowText.h"
#include "WinWindow3D.h"
#include "GUI.h"
#include "WinInput.h"
#include "WinDXGraphics.h"
#include "GLGraphics.h"
#include "WinGLGraphics.h"
#include "WinDXAudio.h"
#include "WinNetwork.h"
#include "WinFramework.h"

#ifdef SDL
#include "SDLInput.h"
#include "SDLWindow3D.h"
#include "SDLGLGraphics.h"
#include "SDLWindowText.h"
#include "SDLFramework.h"
#endif

/**
 * StandardFactory()
 * Default Constructor for the StandardFactory class
 */
StandardFactory::StandardFactory()
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::StandardFactory()");
}

StandardFactory::~StandardFactory()
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::~StandardFactory()");

  Uninitialize();
}

short StandardFactory::Initialize()
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::Initialize()");

  return 0;
}

void StandardFactory::Uninitialize()
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::Uninitialize()");

  return;
}


void StandardFactory::CreateEndian(StandardEndian **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateEndian()");

  *standardinterface = new WinEndian;

  return;
}

void StandardFactory::CreateBuffer(StandardBuffer **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateBuffer()");

  *standardinterface = new WinBuff;

  return;
}

void StandardFactory::CreateFileAccess(StandardFile **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CteateFileAccess()");

  *standardinterface = new WinFile;

  return;
}

void StandardFactory::CreateWindowText(StandardWindowText **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateWindowText()");

#ifdef SDL
  *standardinterface = new SDLWindowText();
#else
  *standardinterface = new WinWindowText();
#endif

  return;
}

void StandardFactory::CreateWindow3D(StandardWindow3D **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateWindow3D()");

#ifdef SDL
  *standardinterface = new SDLWindow3D();
#else
  *standardinterface = new WinWindow3D();
#endif

  return;
}

void StandardFactory::CreateGUI(StandardGUI **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateGUI()");

  *standardinterface = new GUI();

  return;
}

void StandardFactory::CreateInput(StandardInput **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateInput()");

#ifdef SDL
  *standardinterface = new SDLInput();
#endif

#ifdef DirectInput
  *standardinterface = new WinInput();
#endif

  return;
}

void StandardFactory::CreateGraphics(StandardGraphics **standardinterface)
{

  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateGraphics()");	

  #ifdef D3D
   *standardinterface = new WinDXGraphics();
  #endif

#ifdef OGL
#ifdef SDL
    *standardinterface = new SDLGLGraphics();
#else
    *standardinterface = new WinGLGraphics();
#endif
#endif

  return;
}

void StandardFactory::CreateAudio(StandardAudio **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateAudio()");

    *standardinterface = NULL;

#ifdef DirectAudio
  *standardinterface = new WinDXAudio();
#endif

}

void StandardFactory::CreateNetwork(StandardNetwork **standardinterface)
{
  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateNetwork()");
	
  *standardinterface = new WinNetwork();
}

void StandardFactory::CreateFramework(StandardFramework **standardinterface)
{

  CSP_LOG(CSP_FACTORY, CSP_TRACE, "StandardFactory::CreateFramework()");

#ifdef SDL
  *standardinterface = new SDLFramework();
#else
  *standardinterface = new WinFramework();
#endif

}
