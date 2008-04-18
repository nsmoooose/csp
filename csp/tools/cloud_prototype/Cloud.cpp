#include "Cloud.h"
#include "CloudBox.h"
#include "CloudSprite.h"
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
