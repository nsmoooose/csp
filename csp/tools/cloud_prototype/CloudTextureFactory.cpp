#include <osgDB/ReadFile>
#include "CloudTextureFactory.h"

CloudTextureFactory* CloudTextureFactory::m_Instance = NULL;

CloudTextureFactory* CloudTextureFactory::Instance() {
	if(m_Instance == NULL) {
		m_Instance = new CloudTextureFactory();
	}
	return m_Instance;
}

void CloudTextureFactory::Reset() {
	if(m_Instance != NULL) {
		delete m_Instance;
		m_Instance = NULL;
	}
}

osg::ref_ptr<osg::Texture2D> CloudTextureFactory::getTexture(const char* fileName) {
	TextureMap::iterator foundTexture = m_Textures.find(fileName);
	if(foundTexture == m_Textures.end()) {
		osg::ref_ptr<osg::Image> image = osgDB::readImageFile(fileName);
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
		texture->setImage(image.get());
		m_Textures.insert(TextureMap::value_type(fileName, texture));
		return texture;
	}
	return foundTexture->second;
}
