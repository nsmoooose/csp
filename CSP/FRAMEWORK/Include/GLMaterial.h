#ifndef __GLMATERIAL_H_
#define __GLMATERIAL_H_


#include "Framework.h"

/** \class GLMaterial
 * This class implements the Material class
 * for the OpenGL api.
 */
class GLMaterial : public StandardMaterial
{
  public:
	  GLMaterial();

	  virtual ~GLMaterial();
	  void Initialize(StandardColor * emission,
					          StandardColor * specular,
					          StandardColor * diffuse,
					          StandardColor * ambient,
						        float shininess);
	  void Uninitialize();
	  void Apply();
	  StandardMaterial * GetCurrentMaterial();

  private:
	  StandardColor emission;
	  StandardColor specular;
	  StandardColor diffuse;
	  StandardColor ambient;

	  float  shininess;

	  static GLMaterial * m_currMat;

};

#endif
