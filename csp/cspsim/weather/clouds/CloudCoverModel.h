#pragma once

#include <vector>
#include <csp/csplib/data/Link.h>
#include <csp/cspsim/theater/IsoContour.h>
#include <csp/cspsim/theater/FeatureGroupModel.h>
#include <csp/cspsim/weather/clouds/CloudModel.h>

namespace csp {
namespace weather {
namespace clouds {

	class CloudCoverModel : public FeatureGroupModel {
	public:
		CSP_DECLARE_STATIC_OBJECT(CloudCoverModel)

		void postCreate();

		void addSceneModel(FeatureSceneGroup *group, LayoutTransform const &transform, ElevationCorrection const &correction);

	private:
		typedef std::vector<float> DensityVector;
		typedef std::pair<Vector3, Ref<weather::clouds::CloudModel> > CloudPos;
		typedef std::vector<CloudPos> LayoutVector;

		Link<weather::clouds::CloudModel>::vector m_Models;
		DensityVector m_Density;
		float m_Zmin;
		float m_Zmax;
		int m_Seed;
		Link<IsoContour> m_IsoContour;

		LayoutVector m_Layout;
	};

}
}
}
