#include <csp/tools/layout2/cpp/XmlNode.h>
#include <iostream>

namespace csp
{
namespace layout
{

// Pure XML Nodes:

unsigned int XmlNode::nbInstances = 0;

XmlNode::XmlNode()
{
	++nbInstances;
	//std::cout << "XmlNode::nbInstances = " << nbInstances << std::endl;
}

XmlNode::~XmlNode()
{
	--nbInstances;
	//std::cout << "XmlNode::nbInstances = " << nbInstances << std::endl;
}


XmlNodeDocument::XmlNodeDocument()
{
	//std::cout << "XmlNodeDocument::XmlNodeDocument()" << std::endl;
}

XmlNodeDocument::~XmlNodeDocument()
{
	//std::cout << "XmlNodeDocument::~XmlNodeDocument()" << std::endl;
}


XmlNodeElement::XmlNodeElement()
{
}

XmlNodeElement::~XmlNodeElement()
{
}


// csplib archive builtin data Nodes:


// csplib archive simple type data Nodes:


// csplib archive Object type data Node:

XmlNodeObject::XmlNodeObject()
{
}

XmlNodeObject::~XmlNodeObject()
{
}

} // namespace layout
} // namespace csp
