#ifndef __CSPSIM_CLOUDS_CLOUD_H__
#define __CSPSIM_CLOUDS_CLOUD_H__

#include <osg/Node>

class Cloud
{
public:
	Cloud(void);
	virtual ~Cloud(void);

	void Init();

	osg::Node* getModel;

private:
	osg::ref_ptr<osg::Node> m_Model;
};

#endif
