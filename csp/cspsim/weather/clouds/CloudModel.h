#ifndef __CSPSIM_CLOUDS_CLOUDMODEL_H__
#define __CSPSIM_CLOUDS_CLOUDMODEL_H__

#include <csp/csplib/data/Link.h>
#include <csp/csplib/data/Object.h>
#include <csp/csplib/data/Vector2.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/cspsim/Export.h>

namespace osg {
	class Group;
	class LOD;
}

namespace csp {
namespace weather {
namespace clouds {
	class CSPSIM_EXPORT CloudLOD : public Object {
	public:
		CSP_DECLARE_OBJECT(CloudLOD)

		float m_Distance1;
		float m_Distance2;
		float m_Distance3;
	};

	class CSPSIM_EXPORT CloudColorLevels : public Object {
	public:
		CSP_DECLARE_OBJECT(CloudColorLevels)
		typedef std::vector<Vector3> ColorList;
		typedef std::vector<float> LevelList;

		ColorList m_Colors;
		LevelList m_Levels;

		void addLevel(float level, Vector3 color);
	};

	class CSPSIM_EXPORT CloudOpacityLevels : public Object {
	public:
		CSP_DECLARE_OBJECT(CloudOpacityLevels)
		typedef std::vector<float> OpacityList;
		typedef std::vector<float> LevelList;

		LevelList m_Levels;
		OpacityList m_Opacity;

		void addLevel(float level, float opacity);
	};

	class CSPSIM_EXPORT CloudSprite : public Object {
	public:
		CSP_DECLARE_OBJECT(CloudSprite)

		Vector3 m_Position;
		Vector2 m_Size;
		float m_Rotation;
	};

	class CSPSIM_EXPORT CloudBox : public Object {
	public:
		CSP_DECLARE_OBJECT(CloudBox)

		CloudBox();
		virtual ~CloudBox();

		Vector3 m_Size;
		Link<CloudSprite>::vector m_Sprites;
		Link<CloudColorLevels> m_ColorLevels;
		Link<CloudOpacityLevels> m_OpacityLevels;
		Link<CloudLOD> m_LOD;

		virtual void postCreate();

		osg::LOD* getNode();

	private:
		osg::ref_ptr<osg::LOD> m_Node;
	};

	class CSPSIM_EXPORT CloudModel : public Object {
	public:
		CSP_DECLARE_OBJECT(CloudModel)

		CloudModel();
		virtual ~CloudModel();

		Link<CloudBox>::vector m_CloudBoxes;
		std::vector<Vector3> m_CloudBoxPositions;

		virtual void postCreate();

		osg::Group* getNode();

	private:
		osg::ref_ptr<osg::Group> m_Node;
};

}	// end namespace clouds
} 	// end namespace weather
}	// end namespace csp

#endif
