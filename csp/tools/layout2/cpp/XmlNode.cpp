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

std::string XmlNode::trace() const
{
	return "XmlNode";
}


XmlNodeDocument::XmlNodeDocument()
{
	std::cout << "XmlNodeDocument::XmlNodeDocument()" << std::endl;
}

XmlNodeDocument::~XmlNodeDocument()
{
	std::cout << "XmlNodeDocument::~XmlNodeDocument()" << std::endl;
}

std::string XmlNodeDocument::trace() const
{
	return "XmlNodeDocument";
}


XmlNodeElement::XmlNodeElement()
{
	std::cout << "XmlNodeElement::XmlNodeElement()" << std::endl;
}

XmlNodeElement::~XmlNodeElement()
{
	std::cout << "XmlNodeElement::~XmlNodeElement()" << std::endl;
}

std::string XmlNodeElement::trace() const
{
	return "XmlNodeElement";
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

std::string XmlNodeObject::trace() const
{
	return "XmlNodeObject";
}

} // namespace layout
} // namespace csp
