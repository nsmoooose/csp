#ifndef STANDARDTEXTURE_H_
#define STANDARDTEXTURE_H_

  #include "Framework.h"

  class StandardTexture
  {
    public:
      //StandardTexture() {}
      virtual ~StandardTexture() {}

      virtual short Initialize(const char *filename, int filetype) = 0;
      virtual short Initialize(const char *filename) = 0;
      virtual short Initialize(const char *filename, _MIPMAP Levels, StandardColor *ColorKey) = 0;

      virtual void Uninitialize() = 0;
      virtual void Apply() = 0;

  };


#endif
