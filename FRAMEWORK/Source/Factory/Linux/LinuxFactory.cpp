#include "Framework.h"


#include "LinuxEndian.h"
#include "LinuxBuff.h"
#include "LinuxFile.h"
#include "SDLWindowText.h"
#include "SDLWindow3D.h"
#include "GUI.h"
#include "SDLInput.h"
#include "GLGraphics.h"
#include "SDLGLGraphics.h"
#include "LinuxAudio.h"
#include "LinuxNetwork.h"
#include "LinuxFramework.h"

StandardFactory::StandardFactory()
{
  fprintf(stdout, "StandardFactory::StandardFactory()\n");
}

StandardFactory::~StandardFactory()
{
  Uninitialize();
}

short StandardFactory::Initialize()
{
  return 0;
}

void StandardFactory::Uninitialize()
{

}


void StandardFactory::CreateEndian(StandardEndian **standardinterface)
{
  fprintf(stdout, "StandardFactory::CreateEndian()\n");
  *standardinterface = new LinuxEndian;

}

void StandardFactory::CreateBuffer(StandardBuffer **standardinterface)
{

  fprintf(stdout, "StandardFactory::CreateBuffer()\n");
  *standardinterface = new LinuxBuff;

}

void StandardFactory::CreateFileAccess(StandardFile **standardinterface)
{

  fprintf(stdout, "StandardFactory::CreateFileAccess()\n");
  *standardinterface = new LinuxFile;

}

void StandardFactory::CreateWindowText(StandardWindowText **standardinterface)
{

  fprintf(stdout, "StandardFactory::CreateWindowText()\n");
  *standardinterface = new SDLWindowText();

}

void StandardFactory::CreateWindow3D(StandardWindow3D **standardinterface)
{

  fprintf(stdout, "StandardFactory::CreateWindow3D()\n");
  *standardinterface = new SDLWindow3D();

}

void StandardFactory::CreateGUI(StandardGUI **standardinterface)
{

  fprintf(stdout, "StandardFactory::CreateGUI()\n");
  *standardinterface = new GUI();

}

void StandardFactory::CreateInput(StandardInput **standardinterface)
{

  fprintf(stdout, "StandardFactory::CreateInput()\n");
  *standardinterface = new SDLInput();

}

void StandardFactory::CreateGraphics(StandardGraphics **standardinterface)
{

  #ifdef OGL
    fprintf(stdout, "StandardFactory::CreateGraphics()\n");
    *standardinterface = new SDLGLGraphics();
  #endif

}

void StandardFactory::CreateAudio(StandardAudio **standardinterface)
{
  fprintf(stdout, "StandardFactory::CreateAudio()\n");
  *standardinterface = new LinuxAudio();
}

void StandardFactory::CreateNetwork(StandardNetwork **standardinterface)
{
  fprintf(stdout, "StandardFactory::CreateNetwork()\n");
  *standardinterface = new LinuxNetwork();
}

void StandardFactory::CreateFramework(StandardFramework **standardinterface)
{
  fprintf(stdout, "StandardFactory::CreateFramework()\n");
  *standardinterface = new LinuxFramework();
}
