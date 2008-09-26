#include <csp/csplib/data/Date.h>
#include <csp/cspsim/ObjectModel.h>
#include <csp/cspsim/sky/OrbitalBodyImposter.h>
#include <csp/cspsim/sky/OrbitalBodyModel.h>
#include <csp/cspsim/sky/Sky.h>
#include <csp/cspsim/sky/SkyDome.h>
#include <csp/cspsim/sky/SolarSystem.h>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

using namespace csp;

/** This is the date and time for this application. */
double global_date = 0.0;

/** When we have significantly changed date and time we can set this variable to force
 * 	a complete update of the sky dome. */
bool force_update_of_sky_dome = false;

/** This class is responsible for setting up the sky so we are able to look at it. 
 *  It is a group with all stars, sky dome, sun and the moon. */
class SkyGroup : public osg::Group {
public:
	SkyGroup() {
		osg::ref_ptr<Sky> sky = new Sky(1e+6);

		Ref<SolarSystem> ss = sky->getSolarSystem();
		osg::ref_ptr<OrbitalBodyImposter> moon_imposter = new OrbitalBodyImposter(osgDB::readImageFile("moon.png"), 1.0);
		Ref<OrbitalBodyModel> moon = new OrbitalBodyModel(moon_imposter.get(), 0.0, ss->moon(), ss->earth(), 0.01);
		sky->addModel(moon.get());
		sky->addSunlight(0);
		sky->addMoonlight(1);

		addChild(sky->getSunlight());
		addChild(sky->getMoonlight());
		addChild(sky->group());
	
		m_sky = sky;
	}

	Sky* sky() {
		return m_sky.get();
	}

private:
	osg::ref_ptr<Sky> m_sky;
};

/** This keyboard event handler makes is possible for you to advance date and time and
	look at the result. */
class DateTimeEventHandler : public osgGA::GUIEventHandler
{
public:
	/** Handle keyboard event. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&) {
    	switch(ea.getEventType())
	    {
        	case(osgGA::GUIEventAdapter::KEYDOWN):
    	    {
	            if (ea.getKey()==osgGA::GUIEventAdapter::KEY_Right ||
                	ea.getKey()==osgGA::GUIEventAdapter::KEY_KP_Right)
            	{
					global_date += (1.0 / 24) / 4;
					force_update_of_sky_dome = true;
            	    return true;
        	    }
	            else if (ea.getKey()==osgGA::GUIEventAdapter::KEY_Left ||
    	                 ea.getKey()==osgGA::GUIEventAdapter::KEY_KP_Left)
            	{
					global_date -= (1.0 / 24) / 4;
					force_update_of_sky_dome = true;
                	return true;
            	}
            	return false;
        	}

        	default:
    	        return false;
	    }
	}

	/** We also provide some help on this application. */
    virtual void getUsage(osg::ApplicationUsage& usage) const {
	    usage.addKeyboardMouseBinding("Left Arrow", "+ 15 minutes");
    	usage.addKeyboardMouseBinding("Right Array", "- 15 minutes");
	}
};

/** This class is responsible for updating stars, sun, moon and the sky dome
 *	for each frame. */
class SkyUpdateCallback : public osg::NodeCallback
{
	virtual void operator()(osg::Node* node, osg::NodeVisitor*)
	{
		SkyGroup* skyGroup = dynamic_cast<SkyGroup*>(node);
		if(skyGroup) {
			skyGroup->sky()->update(global_date, force_update_of_sky_dome);
			force_update_of_sky_dome = false;
		}
	}
};

int main(int, char**) {
	std::string search_path = "./data/images/";
	csp::ObjectModel::setDataFilePathList(search_path);

	// Set the default date and time for this application.
	global_date = SimDate(2008, 12, 31, 23, 59, 0).getJulianDate();

	// The sky group contains stars, sky dome, sun and moon.
	osg::ref_ptr<SkyGroup> skyGroup = new SkyGroup();
	skyGroup->setUpdateCallback(new SkyUpdateCallback());

//	I guess that we should set where on earth we are.
//	skyGroup->sky()->setPosition(48 * (PI / 180), 2 * (PI / 180));

    // construct the viewer
    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::HelpHandler());
	viewer.addEventHandler(new DateTimeEventHandler());
    
	// Set scene data
    viewer.setSceneData(skyGroup.get());

    // run the viewers frame loop
    return viewer.run();
}
