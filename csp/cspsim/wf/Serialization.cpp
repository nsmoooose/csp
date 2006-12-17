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
#include <csp/csplib/util/FileUtility.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/xml/XmlParser.h>
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/MultiControlContainer.h>
#include <csp/cspsim/wf/Serialization.h>
#include <csp/cspsim/wf/Tab.h>
#include <csp/cspsim/wf/TableControlContainer.h>
#include <csp/cspsim/wf/Theme.h>
#include <csp/cspsim/wf/Window.h>

CSP_NAMESPACE

namespace wf {

void ToValue(Theme* theme, XMLNode& node, const std::string& src, bool* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, float* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, double* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, std::string* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, Ref<wf::Control>* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, ControlVector* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, ListBoxItemVector* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, TabPageVector* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, TableControlContainer::ColumnVector* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, TableControlContainer::RowVector* dst);
void ToValue(Theme* theme, XMLNode& node, const std::string& src, TableControlContainer::XYVector* dst);

/** This class is an interface for all xml string conversion to
 * the internal data representation in our objects.
 */
class StringConverter : public Referenced {
public:
	/** This method must be implemented in order to make a data 
	 * conversion to the internal type of an object. 
	 */
	virtual void Convert(Theme* theme, XMLNode& node, const std::string& value)=0;
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
	
	virtual void Convert(Theme* theme, XMLNode& node, const std::string& value) {
		// Call a method to convert the string. 
		ToValue(theme, node, value, m_Pointer);
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
	void load(Theme* theme, T* object, XMLNode& node) {
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
				converter->second->Convert(theme, childNode, text);
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
				converter->second->Convert(theme, node, text);
			}						
		}
	}

	template<class T>
	void loadControl(Theme* theme, T* control, XMLNode& node) {
		load(theme, control, node);
	}
	
	void loadDocument(Window* window, XMLNode& document) {
		// Process resource includes...
	
		XMLNode node = document.selectSingleNode("WindowDocument/Window");
		loadControl(window->getTheme(), window, node);
	}

private:
	typedef std::map<std::string, Ref<StringConverter> > NodeConverterMap;
	NodeConverterMap m_NodeConverter;
};

Serialization::Serialization(const std::string& userInterfaceDirectory) :
	m_UserInterfaceDirectory(userInterfaceDirectory) {
}

Serialization::~Serialization() {
}

void Serialization::load(Window* window, const std::string& theme, const std::string& file) {
	// Build the path to the actual document we wish to load.
	std::string themesPath = ospath::join(m_UserInterfaceDirectory, "themes");
	std::string themePath = ospath::join(themesPath, theme);
	std::string filePath = ospath::join(themePath, file);
	
	// Load the document.
	XMLNode document = XMLNode::parseFile(filePath.c_str());

	// Parse the content of the document by using our internal archive class.
	ReadingArchive archive;
	archive.loadDocument(window, document);
}

Ref<Control> createControl(Theme* theme, XMLNode& node) {
	ReadingArchive archive;
	std::string name = node.getName();
	if(name == "Button") {
		Ref<Button> control = new Button(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else if(name == "CheckBox") {
		Ref<CheckBox> control = new CheckBox(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else if(name == "Label") {
		Ref<Label> control = new Label(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else if(name == "ListBox") {
		Ref<ListBox> control = new ListBox(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else if(name == "MultiControlContainer") {
		Ref<MultiControlContainer> control = new MultiControlContainer(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else if(name == "SingleControlContainer") {
		Ref<SingleControlContainer> control = new SingleControlContainer(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else if(name == "Tab") {
		Ref<Tab> control = new Tab(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else if(name == "TableControlContainer") {
		Ref<TableControlContainer> control = new TableControlContainer(theme);
		archive.loadControl(theme, control.get(), node);
		return control;
	}
	else {
		return NULL;
	}	
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, bool* dst) {
	if(src == "1" || src == "true") {
		*dst = true;
	} 
	else {
		*dst = false;
	}
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, float* dst) {
	*dst = (float)atof(src.c_str());
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, double* dst) {
	*dst = atof(src.c_str());
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, std::string* dst) {
	*dst = src;
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, Ref<wf::Control>* dst) {
	int childNodeCount = node.nChildNode();
	if(childNodeCount > 0) {
		XMLNode childNode = node.getChildNode(0);
		*dst = createControl(theme, childNode);
	}
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, ControlVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		Ref<Control> control = createControl(theme, childNode);
		if(control.valid()) {
			dst->push_back(control);
		}
	}
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, ListBoxItemVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		Ref<Control> control = createControl(theme, childNode);
		if(control.valid()) {
			dst->push_back(control);
		}
	}
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, TabPageVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);
		Ref<Control> control = createControl(theme, childNode);
		if(control.valid()) {
			dst->push_back(control);
		}
	}
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, TableControlContainer::ColumnVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);

		TableControlContainerColumn column;

		ReadingArchive archive;
		archive.load(theme, &column, childNode);
		dst->push_back(column);
	}
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, TableControlContainer::RowVector* dst) {
	int childNodeCount = node.nChildNode();
	for(int index = 0;index < childNodeCount;++index) {
		XMLNode childNode = node.getChildNode(index);

		TableControlContainerRow row;

		ReadingArchive archive;
		archive.load(theme, &row, childNode);
		dst->push_back(row);
	}
}

void ToValue(Theme* theme, XMLNode& node, const std::string& src, TableControlContainer::XYVector* dst) {
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
				Ref<Control> cellControl = createControl(theme, cellNode.getChildNode(0));
				
				(*dst)[cellIndex][rowIndex] = cellControl;
			}
		}
	}
}

} // namespace wf

CSP_NAMESPACE_END
