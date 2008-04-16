#pragma once

#include <osg/ref_ptr>
#include <osg/Texture2D>

class CloudTextureFactory
{
public:
	static CloudTextureFactory* Instance();

	void Reset();

	osg::ref_ptr<osg::Texture2D> getTexture(const char* fileName);
private:
	typedef std::map<std::string, osg::ref_ptr<osg::Texture2D> > TextureMap;
	TextureMap m_Textures;

	CloudTextureFactory(void) {}
	static CloudTextureFactory* m_Instance;
};
