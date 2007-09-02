// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file Serialization.cpp
 *
 **/

#include <map>
#include <sstream>
#include <iomanip>
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/xml/XmlParser.h>
#include <csp/cspsim/Config.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/Check.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/Image.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/Model.h>
#include <csp/cspsim/wf/MultiControlContainer.h>
#include <csp/cspsim/wf/ResourceLocator.h>
#include <csp/cspsim/wf/Serialization.h>
#include <csp/cspsim/wf/StringResourceManager.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/wf/Window.h>

CSP_NAMESPACE

namespace wf {

void ToValue(XMLNode& node, const std::string& src, osg::Vec4* dst);
void ToValue(XMLNode& node, const std::string& src, bool* dst);
void ToValue(XMLNode& node, const std::string& src, float* dst);
void ToValue(XMLNode& node, const std::string& src, double* dst);
void ToValue(XMLNode& node, const std::string& src, std::string* dst);
void ToValue(XMLNode& node, const std::string& src, Ref<wf::Control>* dst);
void ToValue(XMLNode& node, const std::string& src, ControlVector* dst);
void ToValue(XMLNode& node, const std::string& src, ListBoxItemVector* dst);
void ToValue(XMLNode& node, const std::string& src, Ref<Style>* dst);
void ToValue(XMLNode& node, const std::string& src, Style::UnitValue* dst);
void ToValue(XMLNode& node, const std::string& src, NamedStyleMap* dst);
void ToValue(XMLNode& node, const std::string& src, TabPageVector* dst);
void ToValue(XMLNode& node, const std::string& src, TableControlContainer::ColumnVector* dst);
void ToValue(XMLNode& node, const std::string& src, TableControlContainer::RowVector* dst);
void ToValue(XMLNode& node, const std::string& src, TableControlContainer::XYVector* dst);
void ToValue(XMLNode& node, const std::string& src, StringMap* dst);
template<class T> void ToValue(XMLNode& node, const std::string& src, optional<T>* dst);

/** This class is an interface for all xml string conversion to
 * the internal data representation in our objects.
 */
class StringConverter : public Referenced {
public:
	/** This method must be implemented in order to make a data
	 * conversion to the internal type of an object.
	 */
	virtual void Convert(XMLNode& node, const std::string& value)=0;
};

/** A template based class that implements the converter interface.
 * This class is responsible for calls to the correct data conversion
 * routine.
 */
template<class T>
class TypedStringConverter : public StringConverter {
public:
	/** Take a pointer to the destination object. When the convert
	 * method is called we will overwrite the current object at the
	 * pointer.
	 */
	TypedStringConverter(T* pointer) : m_Pointer(pointer) {}

	virtual void Convert(XMLNode& node, const std::string& value) {
		// Call a method to convert the string.
		ToValue(node, value, m_Pointer);
	}

private:
	T* m_Pointer;
};


/** This class is responsible for all reading of values from the xml file requested.
 * All serialization is done by using template methods.
 */
class ReadingArchive {
public:
	template<class T>
	ReadingArchive* operator &(std::pair<std::string, T*> nvp) {
		m_NodeConverter[nvp.first] = new TypedStringConverter<T>(nvp.second);
		return this;
	}

	template<class T>
	void load(T* object, XMLNode& node) {
		object->serialize(*this);

		// First we iterate all nodes and converts values into the objects
		// data representation.
		int children = node.nChildNode();
		for(int index=0;index<children;++index) {
			XMLNode childNode = node.getChildNode(index);
			std::string name = childNode.getName();
			NodeConverterMap::iterator converter = m_NodeConverter.find(name);
			if(converter != m_NodeConverter.end()) {
				std::string text;
				if(childNode.nText() > 0) {
					text = childNode.getText();
				}
				converter->second->Convert(childNode, text);
			}
		}
		// Then we take all attributes and converts them into the objects
		// data representation.
		int attributes = node.nAttribute();
		for(int index=0;index<attributes;++index) {
			std::string name = node.getAttributeName(index);
			name.insert(0, "@");
			NodeConverterMap::iterator converter = m_NodeConverter.find(name);
			if(converter != m_NodeConverter.end()) {
				std::string text = node.getAttributeValue(index);
				converter->second->Convert(node, text);
			}
		}
	}

	template<class T>
	void loadControl(T* control, XMLNode& node) {
		load(control, node);
	}

	void loadDocument(Window* window, XMLNode& document) {
		XMLNode windowNode = document.selectSingleNode("WindowDocument/Window");
		loadControl(window, windowNode);

		// Fix all parent references that is missing due to serialization.
		setParent(window);
	}

	void loadDocument(StringResourceManager* resourceManager, XMLNode& document) {
		XMLNode stringsNode = document.selectSingleNode("StringTableDocument");
		load(resourceManager, stringsNode);
	}

	void setParent(Container* parent) {
		ControlVector childControls = parent->getChildControls();
		ControlVector::iterator childControl = childControls.begin();
		for(;childControl != childControls.end(); ++childControl) {
			(*childControl)->setParent(parent);
			Container* childContainer = dynamic_cast<Container*>(childControl->get());
			if(childContainer != NULL) {
				setParent(childContainer);
			}
		}
	}

private:
	typedef std::map<std::string, Ref<StringConverter> > NodeConverterMap;
	NodeConverterMap m_NodeConverter;
};

Serialization::Serialization() {
	m_UserInterfaceDirectory = getUIPath();
}

Serialization::Serialization(const std::string& userInterfaceDirectory) :
	m_UserInterfaceDirectory(userInterfaceDirectory) {
}

Serialization::~Serialization() {
}

void Serialization::load(Window* window, const std::string& file) {
	std::string absoluteFilePath = file;
	Ref<ResourceLocator> resourceLocator = createDefaultResourceLocator();
	if(!resourceLocator->locateResource(absoluteFilePath)) {
		CSPLOG(ERROR, APP) << "UI Window document not found.";
		return;
	}

	// Load the document.
	XMLNode document = XMLNode::parseFile(absoluteFilePath.c_str());

	// Parse the content of the document by using our internal archive class.
	ReadingArchive archive;
	archive.loadDocument(window, document);

	// Process resource includes...
	XMLNode includesNode = document.selectSingleNode("WindowDocument/Includes");
	if(!includesNode.isEmpty()) {
		int nodeCount = includesNode.nChildNode();
		for(int index = 0;index < nodeCount;++index) {
			XMLNode includeNode = includesNode.getChildNode(index);
			if(includeNode.nText() == 0) {
				continue;
			}

			std::string includeFile = includeNode.getText(0);

			std::string includeNodeName = includeNode.getName();
			if(includeNodeName == "StringTableInclude") {
				Ref<StringResourceManager> loadedResources = new StringResourceManager;
				if(resourceLocator->locateResource(includeFile)) {
					// Load the resources from the file and merge it to the existing
					// resources in this window manager.
					load(loadedResources.get(), includeFile);
					window->getStringResourceManager()->merge(loadedResources.get());
				}
			}
			else if(includeNodeName == "StyleInclude") {
				if(resourceLocator->locateResource(includeFile)) {
					XMLNode includeDocument = XMLNode::parseFile(includeFile.c_str());
					XMLNode namedStylesNode = includeDocument.selectSingleNode("StyleDocument/NamedStyles");
					NamedStyleMap styles;
					ToValue(namedStylesNode, "", &styles);
	
					NamedStyleMap::iterator style = styles.begin();
					for(;style != styles.end();++style) {
						window->addNamedStyle(style->first, style->second.get());
					}
				}
			}
		}
	}
}

void Serialization::load(StringResourceManager* resourceManager, const std::string& filePath) {
	// Test to see if the file exists.
	if(!ospath::exists(filePath)) {
		CSPLOG(ERROR, APP) << "UI string table document not found.";
		return;
	}

	// Load the document.
	XMLNode document = XMLNode::parseFile(filePath.c_str());

	// Parse the content of the document by using our internal archive class.
	ReadingArchive archive;
	archive.loadDocument(resourceManager, document);
}

Ref<Control> createControl(XMLNode& node) {
	ReadingArchive archive;
	std::string name = node.getName();
	if(name == "Button") {
		Ref<Button> control = new Button();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "Check") {
		Ref<Check> control = new Check();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "CheckBox") {
		Ref<CheckBox> control = new CheckBox();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "Item") {
		Ref<ListBoxItem> control = new ListBoxItem();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "Image") {
		Ref<Image> control = new Image();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "Label") {
		Ref<Label> control = new Label();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "ListBox") {
		Ref<ListBox> control = new ListBox();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "Model") {
		Ref<Model> control = new Model();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "MultiControlContainer") {
		Ref<MultiControlContainer> control = new MultiControlContainer();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "SingleControlContainer") {
		Ref<SingleControlContainer> control = new SingleControlContainer();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "Tab") {
		Ref<Tab> control = new Tab();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "Page") {
		Ref<TabPage> control = new TabPage();
		archive.loadControl(control.get(), node);
		return control;
	}
	else if(name == "TableControlContainer") {
		Ref<TableControlContainer> control = new TableControlContainer();
		archive.loadControl(control.get(), node);
		return control;
	}
	else {
		return NULL;
	}
}

void ToValue(XMLNode& /* node */, const std::string& src, osg::Vec4* dst) {
	std::istringstream data(src);

	unsigned int rgba = 0;
	data >> std::hex >> rgba;

	unsigned char alpha = rgba & 0xff;
	unsigned char blue = (rgba >> 8) & 0xff;
	unsigned char green = (rgba >> 16) & 0xff;
	unsigned char red = (rgba >> 24) & 0xff;

	double step = 1.0f / 255;

	dst->_v[0] = red * step;
	dst->_v[1] = green * step;
	dst->_v[2] = blue * step;
	dst->_v[3] = alpha * step;
}

void ToValue(XMLNode& /* node */, const std::string& src, bool* dst) {
	if(src == "1" || src == "true") {
		*dst = true;
	}
	else {
		*dst = false;
	}
}

void ToValue(XMLNode& /* node */, const std::string& src, float* dst) {
	*dst = (float)atof(src.c_str());
}

void ToValue(XMLNode& /* node */, const std::string& src, double* dst) {
	*dst = atof(src.c_str());
}

void ToValue(XMLNode& /* node */, const std::string& src, std::string* dst) {
	*dst = src;
}

void ToValue(XMLNode& node, const std::string& /* src */, Ref<wf::Control>* dst) {
	int childNodeCount = node.nChildNode();
	if(childNodeCount > 0) {
		XMLNode childNode = node.getChildNode(0);
		*dst = createControl(childNode);
	}
}

void ToValue(XMLNode& node, const std::string& /* src */, ControlVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		Ref<Control> control = createControl(childNode);
		if(control.valid()) {
			dst->push_back(control);
		}
	}
}

void ToValue(XMLNode& node, const std::string& /* src */, ListBoxItemVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		Ref<Control> control = createControl(childNode);
		if(control.valid()) {
			dst->push_back(control);
		}
	}
}

void ToValue(XMLNode& node, const std::string& /* src */, Ref<Style>* dst) {
	ReadingArchive archive;
	archive.load(dst->get(), node);
}

void ToValue(XMLNode& /*node*/, const std::string& src, Style::UnitValue* dst) {
	/* The following formats is supported for the string.
	 * 100.3   ==    out_unit = ePixels out_value = 100.3
	 * 22%     ==    out_unit = ePercentage out_value = 22
	 */
	
	// Default unit is pixels.
	dst->unit = Style::Pixels;
	
	// Check if this is a percentage.
	std::string::size_type pos = src.find("%");
	if(pos != std::string::npos) {
		dst->unit = Style::Percentage;
	}
	
	// Do the conversion.
	dst->value = (float)atof(src.substr(0, pos).c_str());
}

void ToValue(XMLNode& node, const std::string& /* src */, NamedStyleMap* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		Ref<Style> style = new Style;
		ToValue(childNode, "", &style);

		CSP_XMLCSTR name = childNode.getAttribute("Name");
		if(name != NULL) {
			(*dst)[name] = style;
		}
	}
}

void ToValue(XMLNode& node, const std::string& /* src */, TabPageVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		Ref<Control> control = createControl(childNode);
		if(control.valid()) {
			dst->push_back(control);
		}
	}
}

void ToValue(XMLNode& node, const std::string& /* src */, TableControlContainer::ColumnVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);

		TableControlContainerColumn column;

		ReadingArchive archive;
		archive.load(&column, childNode);
		dst->push_back(column);
	}
}

void ToValue(XMLNode& node, const std::string& /* src */, TableControlContainer::RowVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);

		TableControlContainerRow row;

		ReadingArchive archive;
		archive.load(&row, childNode);
		dst->push_back(row);
	}
}

void ToValue(XMLNode& node, const std::string& /* src */, TableControlContainer::XYVector* dst) {
	bool resizeVector = true;

	// Iterate all rows that is found.
	int rowNodeCount = node.nChildNode();
	for(int rowIndex = 0;rowIndex < rowNodeCount;++rowIndex) {
		XMLNode rowNode = node.getChildNode(rowIndex);

		// Iterate all cells found for each row.
		int cellNodeCount = rowNode.nChildNode();

		// We need to resize the vector in order to manage insertion
		// in the vector.
		if(resizeVector) {
			dst->resize(cellNodeCount);
			for(int i=0;i<cellNodeCount;++i) {
				dst->at(i).resize(rowNodeCount);
			}
			resizeVector = false;
		}

		for(int cellIndex=0;cellIndex < cellNodeCount;++cellIndex) {
			XMLNode cellNode = rowNode.getChildNode(cellIndex);
			if(cellNode.nChildNode() > 0) {
				XMLNode controlNode = cellNode.getChildNode(0);
				Ref<Control> cellControl = createControl(controlNode);

				(*dst)[cellIndex][rowIndex] = cellControl;
			}
		}
	}
}

template<class T>
void ToValue(XMLNode& node, const std::string& src, optional<T>* dst) {
	// Make sure that there is constructed an object.
	dst->assign(T());
	// Convert the optional datatype to the destination object.
	ToValue(node, src, dst->get_ptr());
}

void ToValue(XMLNode& node, const std::string& /*src*/, StringMap* dst) {
	StringMap& stringTable = *dst;
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		CSP_XMLCSTR key = childNode.getAttribute("key");
		if(childNode.nText() > 0) {
			stringTable[key] = childNode.getText();
		}
	}
}

} // namespace wf

CSP_NAMESPACE_END
