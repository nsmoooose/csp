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

class XmlNodeString : public XmlNodeElement
{
	public:
};

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


// csplib archive simple type data Nodes:

class XmlNodeReal : public XmlNodeElement
{
	public:
};

class XmlNodeECEF : public XmlNodeElement
{
	public:
};

class XmlNodeLLA : public XmlNodeElement
{
	public:
};

class XmlNodeUTM : public XmlNodeElement
{
	public:
};

class XmlNodeVector : public XmlNodeElement
{
	public:
};

class XmlNodeMatrix : public XmlNodeElement
{
	public:
};

class XmlNodeQuat : public XmlNodeElement
{
	public:
};

class XmlNodeDate : public XmlNodeElement
{
	public:
};

class XmlNodeEnum : public XmlNodeElement
{
	public:
};

class XmlNodeExternal : public XmlNodeElement
{
	public:
};

class XmlNodeKey : public XmlNodeElement
{
	public:
};

class XmlNodePath : public XmlNodeElement
{
	public:
};

class XmlNodeList : public XmlNodeElement
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
