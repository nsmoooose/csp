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

%{
#include <csp/cspwf/Button.h>
#include <csp/cspwf/CheckBox.h>
#include <csp/cspwf/ControlMoveEventHandler.h>
#include <csp/cspwf/Label.h>
#include <csp/cspwf/ListBox.h>
#include <csp/cspwf/ListBoxItem.h>
#include <csp/cspwf/Model.h>
#include <csp/cspwf/ResourceLocator.h>
#include <csp/cspwf/Serialization.h>
#include <csp/cspwf/Signal.h>
#include <csp/cspwf/SignalData.h>
#include <csp/cspwf/Slot.h>
#include <csp/cspwf/Window.h>
#include <csp/cspwf/WindowManagerViewer.h>
%}

namespace csp
{

namespace wf
{

%feature("director") Slot;

%feature("ref")   WindowManager "$this->_incref();"
%feature("unref") WindowManager "$this->_decref();"

class Button;
class CheckBox;
class Container;
class Control;
class Label;
class ListBoxItem;
class Model;
class Signal;
class SignalData;
class Slot;
class Window;
class WindowManager;

class SignalData : public Referenced {
public:
};

class Slot : public Referenced {
public:
	Slot();
	virtual ~Slot();

	virtual void notify(SignalData* data);
};

class Signal : public Referenced {
public:
	virtual void connect(Slot* slot);
	virtual void disconnect(Slot* slot);
	virtual void emit(SignalData* data);
	
	virtual void dispose();
};

%typemap(out) Control * = SWIGTYPE *DYNAMIC;
%{
  static swig_type_info *
  csp__wf__Control_dynamic_cast(void **ptr) {
     csp::wf::Control **nptr = (csp::wf::Control **) ptr;
     {
	     csp::wf::Label *e = dynamic_cast<csp::wf::Label *>(*nptr);
	     if (e) {
	        *ptr = (void *) e;
	        return SWIGTYPE_p_csp__wf__Label;
	     }
	 }
     {
	     csp::wf::Button *e = dynamic_cast<csp::wf::Button *>(*nptr);
	     if (e) {
	        *ptr = (void *) e;
	        return SWIGTYPE_p_csp__wf__Button;
	     }
	 }
     {
	     csp::wf::CheckBox *e = dynamic_cast<csp::wf::CheckBox *>(*nptr);
	     if (e) {
	        *ptr = (void *) e;
	        return SWIGTYPE_p_csp__wf__CheckBox;
	     }
	 }
     {
	     csp::wf::ListBox *e = dynamic_cast<csp::wf::ListBox *>(*nptr);
	     if (e) {
	        *ptr = (void *) e;
	        return SWIGTYPE_p_csp__wf__ListBox;
	     }
	 }
     {
	     csp::wf::Window *e = dynamic_cast<csp::wf::Window *>(*nptr);
	     if (e) {
	        *ptr = (void *) e;
	        return SWIGTYPE_p_csp__wf__Window;
	     }
	 }
     {
	     csp::wf::Model *e = dynamic_cast<csp::wf::Model *>(*nptr);
	     if (e) {
	        *ptr = (void *) e;
	        return SWIGTYPE_p_csp__wf__Model;
	     }
	 }
     csp::wf::Control* f = dynamic_cast<csp::wf::Control*>(*nptr);
     if(f) {
     	*ptr = (void *) f;
     	return SWIGTYPE_p_csp__wf__Control;
     }
     return 0;
  }
%}

// ***************** CONTROL *****************************

// These methods returns reference counted objects.
%newobject Control::getParent();
%newobject Control::getWindowManager();
%newobject Control::getClickSignal();

class Control : public Referenced {
public:
	virtual ~Control();

	virtual std::string getId();
	virtual void setId(const std::string& id);
		
	virtual bool getEnabled() const;
	virtual void setEnabled(bool enabled);

	virtual std::string getName();

	virtual Container* getParent();
	virtual void setParent(Container* parent);
	virtual WindowManager* getWindowManager();

	virtual void performLayout();

	virtual Signal* getClickSignal();

	virtual void dispose();
	
private:
	Control();
};

// ***************** CHECKBOX *****************************

// These methods returns reference counted objects.
%newobject CheckBox::getCheckedChangedSignal();

class CheckBox : public Control {
public:
	CheckBox();
	virtual ~CheckBox();

	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
	
	virtual bool getChecked() const;
	virtual void setChecked(bool checked);
	virtual Signal* getCheckedChangedSignal();
};

// ***************** LABEL *****************************

class Label : public Control {
public:
	Label();
	virtual ~Label();

	virtual const std::string& getText() const;
	virtual void setText(const std::string& text);
};

// ***************** CONTAINER *****************************

%newobject Container::getById();

class Container : public Control {
private:
	Container();
public:
	Control* getById(const std::string& id);
};

// ***************** LISTBOX *****************************

%newobject ListBox::getSelectedItem();
%newobject ListBox::getSelectedItemChangedSignal();

class ListBox : public Container {
public:
	ListBox();
	virtual ~ListBox();

	virtual void addItem(ListBoxItem* item);
//	virtual ListBoxItemVector getItems();
	
	virtual ListBoxItem* getSelectedItem();
	virtual void setSelectedItem(ListBoxItem* item);
	virtual bool setSelectedItemByText(const std::string& text);

	virtual Signal* getSelectedItemChangedSignal();
};

// ***************** MODEL *****************************

class Model : public Control {
public:
	virtual std::string getModelFilePath() const;
	virtual void setModelFilePath(std::string& filePath);

	virtual double getScale() const;
	virtual void setScale(double scale);
};

// ***************** SINGLECONTROLCONTAINER *****************************

class SingleControlContainer : public Container {
public:
	SingleControlContainer();
};

// ***************** BUTTON *****************************

class Button : public SingleControlContainer {
public:
	Button();
	virtual ~Button();

	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
};

// ***************** LISTBOXITEM *****************************

class ListBoxItem : public SingleControlContainer {
public:
	ListBoxItem();
	ListBoxItem(const std::string& text);
	virtual ~ListBoxItem();

	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
};

// ***************** EVENTHANDLERS *****************************
class ControlMoveEventHandler : public Referenced {
public:
	ControlMoveEventHandler(Control* control);
	virtual ~ControlMoveEventHandler();
};

// ***************** SERIALIZATION *****************************

class ResourceLocator : public Referenced {
public:
	ResourceLocator();
	virtual void addFolder(const std::string& folder);
	virtual std::string getPathVariable() const;
};

void setDefaultResourceLocator(ResourceLocator* locator);

class SerializationException : public std::exception {
public:
	SerializationException(const char* message);
	virtual ~SerializationException();

	const char* what();
};

class Serialization : public Referenced {
public:
	Serialization();
	virtual void load(Window* window, const std::string& file) throw (SerializationException);
};

%newobject WindowManager::getById();

class WindowManager : public Referenced {
public:
	virtual Window* getById(const std::string& id);
	virtual void show(Window* window);
	virtual void close(Window* window);	
	virtual void closeAll();
private:
	WindowManager();
};

class WindowManagerViewer : public WindowManager {
public:
	WindowManagerViewer(int width=1024, int height=768);
	virtual ~WindowManagerViewer();
};

%extend WindowManagerViewer {
	osg::Node* getRootNode() {
		return self->getRootNode().get();
	}
};

class Window : public SingleControlContainer {
public:
	Window();
	
	virtual void setWindowManager(WindowManager* manager);
	virtual void setTheme(const std::string& theme);
	virtual std::string getTheme() const;
	virtual void close();	
	virtual void centerWindow();
	virtual void maximizeWindow();
};

}
}

DYNAMIC_CAST(SWIGTYPE_p_csp__wf__Control, csp__wf__Control_dynamic_cast);
