#ifndef __CSP_LAYOUT_OSGTERRAINOVERVIEWMANIPULATOR_H__
#define __CSP_LAYOUT_OSGTERRAINOVERVIEWMANIPULATOR_H__

#include <osgGA/MatrixManipulator>

namespace csp
{
namespace layout
{

class OsgTerrainOverviewManipulator : public osgGA::MatrixManipulator
{
	public:
		static const double MinLevel;
		static const double MaxLevel;
		static const int TileSize;
		static const float LodDistance;

		OsgTerrainOverviewManipulator();

		virtual const char* className() const { return "OsgTerrainOverviewManipulator"; }

		virtual void setByMatrix(const osg::Matrixd& matrix);
		virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix( osg::Matrixd::inverse( matrix ) ); }

		virtual osg::Matrixd getMatrix() const { return osg::Matrixd::inverse( m_inverseMatrix ); }
		virtual osg::Matrixd getInverseMatrix() const { return m_inverseMatrix; }

		virtual void setNode(osg::Node* node) { m_node = node; }
		virtual const osg::Node* getNode() const { return m_node.get(); }

		virtual void computeHomePosition();

		virtual void home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		virtual void init(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	private:
		~OsgTerrainOverviewManipulator();

		void resetMove();
		void onStartDrag(const osgGA::GUIEventAdapter& ea);
		void onStartZoom(const osgGA::GUIEventAdapter& ea);
		bool onMouseDrag(const osgGA::GUIEventAdapter& ea);
		bool onMouseZoom(const osgGA::GUIEventAdapter& ea);

		void zoomIn(const osgGA::GUIEventAdapter& ea);
		void zoomOut(const osgGA::GUIEventAdapter& ea);

		void computeCenterOnZoom(const osgGA::GUIEventAdapter& startZoomEvent, osg::Vec3d startZoomCenter, double startZoomLevel);
		void clampLevel();
		void computeMatrix();

		osg::ref_ptr<osg::Node> m_node;

		osg::Vec3d m_center;
		double m_level;

		osg::Matrixd m_inverseMatrix;

		osg::ref_ptr<const osgGA::GUIEventAdapter> m_startMoveEvent;
		osg::Vec3d m_startZoomCenter;
		double m_startZoomLevel;
};

} // namespace layout
} // namespace csp

#endif // __CSP_LAYOUT_OSGTERRAINOVERVIEWMANIPULATOR_H__
