
#ifndef	__MATERIALFORMAT_H__
#define __MATERIALFORMAT_H__

#include "graphics/TypesGraphics.h"
#include "MaterialFormat.h"
#include "GraphicsFormat.h"

class MaterialFormat : public GraphicsFormat
{
public:
	MaterialFormat();
	MaterialFormat( _Material * );

	virtual ~MaterialFormat();

	virtual void Apply();

	void SetMaterial( _Material * );

private:
	_Material				*mat;

	// Use this to get the current material
	// Unimplemented until storage is finalized
	_Material * GetCurrentMaterial();

};

#endif