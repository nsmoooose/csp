#ifndef	__TEXTUREFORMAT_H__
#define	__TEXTUREFORMAT_H__

#include "GraphicsFormat.h"
#include "Texture.h"

class TextureFormat : public GraphicsFormat
{
public:
	TextureFormat();
	TextureFormat( Texture *, unsigned long, bool );
	~TextureFormat();

	virtual void	Apply();

	void			setTexture( Texture * );
	Texture *		getTexture();

	void			setTexMode( unsigned long );

	void			setWrapS( unsigned long );
	void			setWrapT( unsigned long );
	void			setWrapR( unsigned long );

	void			setMagFilter( unsigned long );
	void			setMinFilter( unsigned long );

	void			setTexRotation( float * );

	void			setMultiTexture( bool );
	void			setTexUnit( int );

private:
	Texture			*tex;
	unsigned long	texMode;
	bool			multiTexture;
	float			*rotation;
	int				texUnit;
};

#endif