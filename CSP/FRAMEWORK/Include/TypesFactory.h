#ifndef TYPESFACTORYTAG

  #define TYPESFACTORYTAG

  #include "Framework.h"

  class StandardFactory
  {

    private:

    public:

      StandardFactory();
      ~StandardFactory();

      // functions
      short Initialize();
      void  Uninitialize();
      
      // -----

      void CreateEndian(StandardEndian **standardinterface);
      void CreateBuffer(StandardBuffer **standardinterface);
      void CreateFileAccess(StandardFile **standardinterface);
      void CreateWindowText(StandardWindowText **standardinterface);
      void CreateWindow3D(StandardWindow3D **standardinterface);
      void CreateGUI(StandardGUI **standardinterface);
      void CreateInput(StandardInput **standardinterface);
      void CreateGraphics(StandardGraphics **standardinterface);
	    void CreateAudio(StandardAudio **standardinterface);
	    void CreateNetwork(StandardNetwork **standardinterface);
      void CreateFramework(StandardFramework **standardinterface);
  };

#endif
