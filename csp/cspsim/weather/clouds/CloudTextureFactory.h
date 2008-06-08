#ifndef __CSPSIM_CLOUDS_CLOUDTEXTUREFACTORY_H__
#define __CSPSIM_CLOUDS_CLOUDTEXTUREFACTORY_H__

#include <osg/ref_ptr>
#include <osg/Texture2D>

namespace csp {
namespace weather {
namespace clouds {

/* This class holds cloud textures with the filename of the texture
as a key. This singleton will keep all textures in memory until the
Reset method is called. */
class CloudTextureFactory
{
public:
	static CloudTextureFactory* Instance();

	// Clears this singleton from all cached textures.
	void Reset();

	/* Returns the texture for the specified image file name. Calling
	this method several times will result in the same texture returned. */
	osg::ref_ptr<osg::Texture2D> getTexture(const char* fileName);
private:
	typedef std::map<std::string, osg::ref_ptr<osg::Texture2D> > TextureMap;
	TextureMap m_Textures;

	CloudTextureFactory(void) {}
	static CloudTextureFactory* m_Instance;
};

}	// end namespace clouds
} 	// end namespace weather
}	// end namespace csp

#endif
