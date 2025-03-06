#include <osgDB/ReadFile>
#include <csp/csplib/util/Log.h>
#include <csp/cspsim/weather/clouds/CloudTextureFactory.h>

namespace csp {
namespace weather {
namespace clouds {

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
		if(image.valid()) {
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
			texture->setImage(image.get());
			m_Textures.insert(TextureMap::value_type(fileName, texture));
			CSPLOG(Prio_DEBUG, Cat_APP) << "Caching texture file: " << fileName;
			return texture;
		}
		else {
			CSPLOG(Prio_DEBUG, Cat_APP) << "Texture file not found: " << fileName;
		}
	}
	return foundTexture->second;
}

} // end namespace clouds
} // end namespace weather
} // end namespace csp
