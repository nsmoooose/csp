#ifndef __STANDARDMATERIAL_H_
#define __STANDARDMATERIAL_H_

#include "Framework.h"

/** \class StandardMaterial
 * This class servers as a base class for a material which
 * is a generized color format.
 */
class StandardMaterial
{
  public:
    StandardMaterial() {}

    virtual ~StandardMaterial() {}

    virtual void Initialize(StandardColor * emission,
		                        StandardColor * specular,
		                        StandardColor * diffuse,
		                        StandardColor * ambient,
		                        float shininess) = 0;
    virtual void Uninitialize() = 0;

    virtual void Apply() = 0;

    virtual StandardMaterial * GetCurrentMaterial() = 0;

};

#endif
