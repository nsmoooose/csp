#include <csp/tools/layout2/cpp/OsgTerrainOverviewManipulator.h>

namespace csp
{
namespace layout
{

const double OsgTerrainOverviewManipulator::MinLevel = 0.0;
const double OsgTerrainOverviewManipulator::MaxLevel = 20.0;
const int OsgTerrainOverviewManipulator::TileSize = 256;
const float OsgTerrainOverviewManipulator::LodDistance = OsgTerrainOverviewManipulator::TileSize * 100.0f;

OsgTerrainOverviewManipulator::OsgTerrainOverviewManipulator()
	: m_level( MinLevel )
{
}

OsgTerrainOverviewManipulator::~OsgTerrainOverviewManipulator()
{
}

void OsgTerrainOverviewManipulator::setByMatrix(const osg::Matrixd& matrix)
{
}

void OsgTerrainOverviewManipulator::computeHomePosition()
{
}

void OsgTerrainOverviewManipulator::home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
}

void OsgTerrainOverviewManipulator::init(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	computeMatrix();
}

bool OsgTerrainOverviewManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	switch ( ea.getEventType() )
	{
		case osgGA::GUIEventAdapter::SCROLL:
		{
			switch ( ea.getScrollingMotion() )
			{
				case osgGA::GUIEventAdapter::SCROLL_DOWN:
				{
					zoomOut( ea );
					return true;
				}

				case osgGA::GUIEventAdapter::SCROLL_UP:
				{
					zoomIn( ea );
					return true;
				}
			}

			break;
		}

		case osgGA::GUIEventAdapter::KEYDOWN:
		{
			switch ( ea.getKey() )
			{
				case '-':
				{
					zoomOut( ea );
					return true;
				}

				case '+':
				{
					zoomIn( ea );
					return true;
				}
			}

			break;
		}

		case osgGA::GUIEventAdapter::PUSH:
		{
			if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
			{
				onStartDrag( ea );
				return true;
			}
			else if ( ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
				ea.getButtonMask() == (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) )
			{
				onStartZoom( ea );
				return true;
			}
			else
			{
				resetMove();
			}
			break;
		}

		case osgGA::GUIEventAdapter::RELEASE:
		{
			resetMove();
			break;
		}

		case osgGA::GUIEventAdapter::DOUBLECLICK:
		{
			resetMove();
			break;
		}

		case osgGA::GUIEventAdapter::DRAG:
		{
			if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
			{
				if ( onMouseDrag( ea ) ) return true;
			}
			else if ( ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
				ea.getButtonMask() == (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) )
			{
				if ( onMouseZoom( ea ) ) return true;
			}

			break;
		}
	}

	return false;
}

void OsgTerrainOverviewManipulator::zoomIn()
{
	m_level += 1;
	clampLevel();
	computeMatrix();
}

void OsgTerrainOverviewManipulator::zoomOut()
{
	m_level -= 1;
	clampLevel();
	computeMatrix();
}

void OsgTerrainOverviewManipulator::resetMove()
{
	m_startMoveEvent = 0;
}

void OsgTerrainOverviewManipulator::onStartDrag(const osgGA::GUIEventAdapter& ea)
{
	m_startMoveEvent = &ea;
}

void OsgTerrainOverviewManipulator::onStartZoom(const osgGA::GUIEventAdapter& ea)
{
	m_startMoveEvent = &ea;
	m_startZoomCenter = m_center;
	m_startZoomLevel = m_level;
}

bool OsgTerrainOverviewManipulator::onMouseDrag(const osgGA::GUIEventAdapter& ea)
{
	if ( !m_startMoveEvent ) return false;

	double scale = ::pow( 2.0, m_level );
	osg::Vec3d delta( ea.getX() - m_startMoveEvent->getX(), ea.getY() - m_startMoveEvent->getY(), 0.0 );
	m_center -= delta / scale;
	computeMatrix();

	m_startMoveEvent = &ea;

	return true;
}

bool OsgTerrainOverviewManipulator::onMouseZoom(const osgGA::GUIEventAdapter& ea)
{
	if ( !m_startMoveEvent ) return false;

	double dy = ea.getYnormalized() - m_startMoveEvent->getYnormalized();

	m_level = m_startZoomLevel + dy * ( MaxLevel - MinLevel ) / 2;
	clampLevel();
	computeCenterOnZoom(*m_startMoveEvent, m_startZoomCenter, m_startZoomLevel);
	computeMatrix();

	return true;
}

void OsgTerrainOverviewManipulator::zoomIn(const osgGA::GUIEventAdapter& ea)
{
	double startZoomLevel = m_level;
	m_level += 1;
	clampLevel();
	computeCenterOnZoom(ea, m_center, startZoomLevel);
	computeMatrix();
}

void OsgTerrainOverviewManipulator::zoomOut(const osgGA::GUIEventAdapter& ea)
{
	double startZoomLevel = m_level;
	m_level -= 1;
	clampLevel();
	computeCenterOnZoom(ea, m_center, startZoomLevel);
	computeMatrix();
}

void OsgTerrainOverviewManipulator::computeCenterOnZoom(const osgGA::GUIEventAdapter& startZoomEvent, osg::Vec3d startZoomCenter, double startZoomLevel)
{
	osg::Vec3d zoomPivot( startZoomEvent.getX(), startZoomEvent.getY(), 0 );
	osg::Vec3d viewportCenter( (startZoomEvent.getXmin() + startZoomEvent.getXmax()) / 2, (startZoomEvent.getYmin() + startZoomEvent.getYmax()) / 2, 0 );

	// Convert from Window to View coordinate
	zoomPivot -= viewportCenter;

	double scaleRatio = ::pow( 2.0, m_level - startZoomLevel );
	osg::Vec3d newZoomPivot = zoomPivot * scaleRatio;

	double newScale = ::pow( 2.0, m_level );
	m_center = startZoomCenter + (newZoomPivot - zoomPivot) / newScale;
}

void OsgTerrainOverviewManipulator::clampLevel()
{
	if ( m_level < MinLevel ) m_level = MinLevel;
	if ( m_level > MaxLevel ) m_level = MaxLevel;
}

void OsgTerrainOverviewManipulator::computeMatrix()
{
	double scale = ::pow( 2.0, m_level );
	m_inverseMatrix = osg::Matrixd::translate( -m_center )
		* osg::Matrixd::scale( scale, scale, scale )
		* osg::Matrixd::translate( 0, 0, -OsgTerrainOverviewManipulator::LodDistance );
}

} // namespace layout
} // namespace csp
