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
#include <csp/cspsim/wf/Button.h>
#include <csp/cspsim/wf/CheckBox.h>
#include <csp/cspsim/wf/Label.h>
#include <csp/cspsim/wf/ListBox.h>
#include <csp/cspsim/wf/ListBoxItem.h>
#include <csp/cspsim/wf/Model.h>
#include <csp/cspsim/wf/Serialization.h>
#include <csp/cspsim/wf/Signal.h>
#include <csp/cspsim/wf/SignalData.h>
#include <csp/cspsim/wf/Slot.h>
#include <csp/cspsim/wf/Window.h>
#include <csp/cspsim/wf/WindowManager.h>
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

// ***************** INPUTINTERFACEMANAGER *****************************

%newobject InputInterfaceManager::getActionSignal();
%newobject InputInterfaceManager::registerActionSignal();

class InputInterfaceManager : public Referenced {
private:
	InputInterfaceManager();
	~InputInterfaceManager();
	
public:
	virtual Signal* getActionSignal(const std::string& id);
	virtual Signal* registerActionSignal(const std::string& id);	
};

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

	virtual void buildGeometry();

	virtual Signal* getClickSignal();

	virtual void dispose();
};

// ***************** CHECKBOX *****************************

// These methods returns reference counted objects.
%newobject CheckBox::getCheckedChangedSignal();

class CheckBox : public Control {
public:
	CheckBox();
	CheckBox(const std::string text);
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
	Label(const std::string text);
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
	Button(const std::string text);
	virtual ~Button();

	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
};

// ***************** LISTBOXITEM *****************************

class ListBoxItem : public SingleControlContainer {
public:
	ListBoxItem();
	ListBoxItem(const std::string text);
	virtual ~ListBoxItem();

	virtual const std::string getText() const;
	virtual void setText(const std::string& text);
};

// ***************** SERIALIZATION *****************************

class Serialization : public Referenced {
public:
	Serialization();
	Serialization(std::string&);
	virtual void load(Window* window, const std::string& file);
};

class WindowManager : public Referenced {
public:
	virtual void show(Window* window);
	virtual void close(Window* window);	
	virtual void closeAll();
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
