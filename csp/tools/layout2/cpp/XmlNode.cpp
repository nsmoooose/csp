#include <csp/tools/layout2/cpp/XmlNode.h>
#include <iostream>

namespace csp
{
namespace layout
{

// Pure XML Nodes:

XmlNode::XmlNode()
{
}

XmlNode::~XmlNode()
{
}


XmlNodeDocument::XmlNodeDocument()
{
	std::cout << "XmlNodeDocument::XmlNodeDocument()" << std::endl;
}

XmlNodeDocument::~XmlNodeDocument()
{
	std::cout << "XmlNodeDocument::~XmlNodeDocument()" << std::endl;
}


XmlNodeElement::XmlNodeElement()
{
	std::cout << "XmlNodeElement::XmlNodeElement()" << std::endl;
}

XmlNodeElement::~XmlNodeElement()
{
	std::cout << "XmlNodeElement::~XmlNodeElement()" << std::endl;
}


// csplib builtin data archive Nodes:


// csplib simple type data archive Nodes:


// csplib Object type data archive Node:

XmlNodeObject::XmlNodeObject()
{
	std::cout << "XmlNodeObject::XmlNodeObject()" << std::endl;
}

XmlNodeObject::~XmlNodeObject()
{
	std::cout << "XmlNodeObject::~XmlNodeObject()" << std::endl;
}

} // namespace layout
} // namespace csp
