///////////////////////////////////////////////////////////////////////////
//
//   GLMaterial  - CSP - http://csp.homeip.net
//
//   coded by Wolverine and GKW
//
///////////////////////////////////////////////////////////////////////////
//   
//  This class is used to represent a Graphics Material.
//  Which allows a mesh to be colored.
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
///////////////////////////////////////////////////////////////////////////



#ifdef OGL

#include "GLMaterial.h"

GLMaterial * GLMaterial::m_currMat=NULL;

/** 
 * \fn GLMaterial::GLMaterial()
 * \brief Constructor for GLMaterial, Also initializes the global currentMaterial.
 */
GLMaterial::GLMaterial() 
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLMaterial::GLMaterial()");      

    if (GLMaterial::m_currMat == NULL)
	{
		// this is a hack but set the current material
		// equal to this objects, then create a new one.
		// this stops infinite recursion.
		GLMaterial::m_currMat = this;
		GLMaterial::m_currMat = new GLMaterial();
	}
}

/**
 * \fn GLMaterial::~GLMaterial()
 * \brief Destructor for GLMaterial.
 */
GLMaterial::~GLMaterial()
{

}

/**
 * \fn void GLMaterial::Initialize(StandardColor * pEmission, StandardColor * pSpecular, StandardColor * pDiffuse, StandardColor * pAmbient, float _shininess)
 * \brief Initializes a Material object.
 * \param pEmission The emission color of the material
 * \param pSpecular The specular color of the material
 * \param pDiffuse The diffuse color of the material
 * \param pAmbient The ambient color of the material
 * \param _shininess The shininess of the material
 */
void GLMaterial::Initialize(StandardColor * pEmission,
						StandardColor * pSpecular,
						StandardColor * pDiffuse,
						StandardColor * pAmbient,
						float _shininess)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLMaterial::Initialize()");

	emission = *pEmission;
	specular = *pSpecular;
	diffuse = *pDiffuse;
	ambient = *pAmbient;
	shininess = _shininess;

}

/**
 * \fn void GLMaterial::Uninitialize()
 * \brief Uninitialize the material
 */
void GLMaterial::Uninitialize()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLMaterial::Uninitialize()");

}

/**
 * \fn void GLMaterial::Apply()
 * \brief Sets the material properties in the object to be current on the rendering device.
 */
void GLMaterial::Apply()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLMaterial::Apply()");

	GLMaterial *currMat = (GLMaterial*)GetCurrentMaterial();

	if( currMat->ambient != ambient )
	{
		glMaterialfv( GL_FRONT, GL_AMBIENT, (float*)&ambient );
		currMat->ambient = ambient;
	}
	if( currMat->diffuse != diffuse)
	{
		glMaterialfv( GL_FRONT, GL_DIFFUSE, (float*)&diffuse );
		currMat->diffuse = diffuse;
	}
	if( currMat->emission != emission )
	{
		glMaterialfv( GL_FRONT, GL_EMISSION, (float*)&emission );
		currMat->emission = emission;
	}
	if( currMat->specular != specular )
	{
		glMaterialfv( GL_FRONT, GL_SPECULAR, (float*)&specular );
		currMat->specular = specular;
	}
	if( currMat->shininess != shininess )
	{
		glMaterialfv( GL_FRONT, GL_SHININESS, &shininess );
		currMat->shininess = shininess;
	}

}

/**
 * \fn StandardMaterial * GLMaterial::GetCurrentMaterial()
 * \brief Returns the current material which is a global variable.
 */
StandardMaterial * GLMaterial::GetCurrentMaterial()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLMaterial::GetCurrentMaterial()");

    return m_currMat;
}

#endif
