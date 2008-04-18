#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include "Cloud.h"
#include "CloudBox.h"
#include "visitors/RemoveRedundantCloudSpritesVisitor.h"
#include "visitors/UpdateCloudModelVisitor.h"

using namespace csp::clouds::visitors;

namespace csp {
namespace clouds {

Cloud::Cloud(void) : m_SpriteRemovalThreshold(2.0) {
}

Cloud::~Cloud(void) {
}

void Cloud::setSpriteRemovalThreshold(float threshold) {
	m_SpriteRemovalThreshold = threshold;
}

float Cloud::getSpriteRemovalThreshold() {
	return m_SpriteRemovalThreshold;
}

void Cloud::UpdateModel() {
	// Hint osg that we must handle the cloud as a transparent object.
	osg::ref_ptr<osg::StateSet> stateset = getOrCreateStateSet();
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// Must also tell osg that we need to blend the alpha channel with
	// the background.
	osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    stateset->setAttributeAndModes(blendFunction.get());

	osg::ref_ptr<osg::AlphaFunc> alphafunc = new osg::AlphaFunc;
	alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
	stateset->setAttributeAndModes(alphafunc.get(), osg::StateAttribute::ON);

	// Update the model by creating all nececarry geometry.
	UpdateCloudModelVisitor updateModelVisitor;
	accept(updateModelVisitor);

	// Remove all reduntant sprites that are very close
	// to each other. 
	RemoveRedundantCloudSpritesVisitor removeReduntantSpritesVisitor(m_SpriteRemovalThreshold);
	accept(removeReduntantSpritesVisitor);
}

} // end namespace clouds
} // end namespace csp
