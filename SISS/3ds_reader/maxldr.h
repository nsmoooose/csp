/*
 *
 *	3DS File manipulator
 *
 *	03/27/2001
 *
 */

#ifndef	__MAXLDR_H__
#define	__MAXLDR_H__

#include "vector.h"
#include "list.h"
#include "I:/csp/framework/include/framework.h"

class istream;

class Color3DS {
	friend	istream& operator>>( istream&, Color3DS& );
public:
	Color3DS();
	Color3DS( float, float, float );

	float	Red();
	void	Red( float );
	float	Green();
	void	Green( float );
	float	Blue();
	void	Blue( float );

	void	floatFlag( bool );

private:
	float	m_r;
	float	m_g;
	float	m_b;
	bool	fl;
};


class Material3DS {
	friend istream &operator>>( istream &, Material3DS & );
public:
	Material3DS();
	~Material3DS();
	void setMatName( char * );
	void setAmb( Color3DS * );
	void setDif( Color3DS * );
	void setSpc( Color3DS * );
	void setTexName( char * );
	void setTexPct( float );
	void setTexTile( unsigned short );
	void setTexBlur( float );

	char *			getMatName();
	Color3DS *		getAmb();
	Color3DS *		getDif();
	Color3DS *		getSpc();
	char *			getTexName();
	float			getTexPct();
	unsigned short	getTexTile();
	float			getTexBlur();

private:
	char			*mp_matname;
	Color3DS		m_ambient;
	Color3DS		m_diffuse;
	Color3DS		m_specular;
	char			*mp_texname;
	float			m_pct;
	unsigned short	m_tiling;
	float			m_texblur;
};

struct Face3DS {
	int	vertex1;
	int vertex2;
	int vertex3;
	int	flags;
};

struct Mat3DS {
	char	*name;
	short	numFaces;
	short	*faces;
};

struct TexCoords3DS {
	float	u;
	float	v;
};

class Object3DS {
	friend istream &operator>>( istream &, Object3DS & );
public:
	Object3DS();
	~Object3DS();
	void			setName( char * );
	void			setVArray( StandardVector4 * );
	void			setFArray( Face3DS * );
	void			setTCoords( TexCoords3DS * );

	char *			getName();
	StandardVector4 *getVArray();
	int				getNumVertex();
	Face3DS *		getFArray();
	int				getNumFaces();
	Mat3DS *		getMGroup(int);
	int				getNumMats();
	TexCoords3DS *	getTCoords();
	StandardMatrix4 * getMatrix();

private:
	char			*mp_objname;
	StandardVector4	*mp_vertexArray;
	int				numVertex;
	Face3DS			*mp_faceArray;
	int				numFaces;
	List<Mat3DS *>	mp_matgroup;
	TexCoords3DS	*mp_texcoords;
	StandardMatrix4	m_transMatrix;
};

class Animation3DS {
	friend istream &operator>>( istream &, Animation3DS & );
public:
	Animation3DS();
	~Animation3DS();
	char *	getObjectName();
	float *	getPivot();
	float *	getGlobalPos();
	float *	getRotationAngle();
	float *	getRotX();
	float *	getRotY();
	float *	getRotZ();
	float *	getScaleX();
	float *	getScaleY();
	float *	getScaleZ();

private:
	char		*objName;
	float		pivot[3];
	float		globalPos[3];
	float		rotAngle;
	float		rotX;
	float		rotY;
	float		rotZ;
	float		scaleX;
	float		scaleY;
	float		scaleZ;
};

class Reader3DS {
public:
	//~Reader3DS();
	Object3DS *		getObject( int );
	int				getNumObjects();
	Material3DS *	getMaterial( int );
	int				getNumMaterials();
	Animation3DS *	getAnimation( int );
	int				getNumAnimations();
	bool			read3DS( const char * );
	void			scaleVertex( float );

private:
	List<Object3DS *>		objects;
	List<Material3DS *>		materials;
	List<Animation3DS *>	animations;
};


#endif