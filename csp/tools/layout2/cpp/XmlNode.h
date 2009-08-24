#ifndef __CSP_LAYOUT_XMLNODE_H__
#define __CSP_LAYOUT_XMLNODE_H__

#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/util/Ref.h>

namespace csp
{
namespace layout
{

// Pure XML Nodes:

class XmlNode : public Referenced
{
	public:
		XmlNode();
		virtual ~XmlNode();
};

class XmlNodeDocument : public XmlNode
{
	public:
		XmlNodeDocument();
		~XmlNodeDocument();

		virtual csp::Ref<XmlNode> GetRootElement() const { return 0; };
};

class XmlNodeElement : public XmlNode
{
	public:
		XmlNodeElement();
		~XmlNodeElement();
};


// csplib archive builtin data Nodes:

class XmlNodeBool : public XmlNodeElement
{
	public:
};

class XmlNodeInt : public XmlNodeElement
{
	public:
};

class XmlNodeFloat : public XmlNodeElement
{
	public:
};

class XmlNodeString : public XmlNodeElement
{
	public:
};


// csplib archive simple type data Nodes:

class XmlNodeLLA : public XmlNodeElement
{
	public:
};


// csplib archive Object type data Node:

class XmlNodeObject : public XmlNodeElement
{
	public:
		XmlNodeObject();
		~XmlNodeObject();
};

} // namespace layout
} // namespace csp

#endif // __CSP_LAYOUT_XMLNODE_H__
