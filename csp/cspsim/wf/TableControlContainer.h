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
 * @file TableControlContainer.h
 *
 **/

#ifndef __CSPSIM_WF_TABLECONTROLCONTAINER_H__
#define __CSPSIM_WF_TABLECONTROLCONTAINER_H__

#include <csp/cspsim/wf/Container.h>
#include <csp/cspsim/wf/Padding.h>

CSP_NAMESPACE

namespace wf {

enum TableControlContainerSizeType {
	Percent,
	Absolute
};

class TableControlContainerColumn {
public:
	TableControlContainerColumn() : m_SizeType(Percent), m_Width(0.0f) {}

	double getWidth() const { return m_Width; }
	void setWidth(double width) { m_Width = width; }
	
	TableControlContainerSizeType getSizeType() const { return m_SizeType; }
	void setSizeType(TableControlContainerSizeType sizeType) { m_SizeType = sizeType; }

private:
	TableControlContainerSizeType m_SizeType;
	double m_Width;
};

class TableControlContainerRow {
public:
	TableControlContainerRow() : m_SizeType(Percent), m_Height(0.0f) {}
	
	double getHeight() const { return m_Height; }
	void setHeight(double height) { m_Height = height; }
	
	TableControlContainerSizeType getSizeType() const { return m_SizeType; }
	void setSizeType(TableControlContainerSizeType sizeType) { m_SizeType = sizeType; }
	
private:
	TableControlContainerSizeType m_SizeType;
	double m_Height;
};

/** 
 *
 */
class TableControlContainer : public Container {
public:
	typedef std::vector<TableControlContainerColumn> ColumnVector;
	typedef std::vector<TableControlContainerRow> RowVector;

	TableControlContainer(Theme* theme);
	TableControlContainer(Theme* theme, unsigned int columns, unsigned int rows);
	virtual ~TableControlContainer();

	virtual void buildGeometry();
	
	//! Resizes the child control to fit the entire surface of this control.
	virtual void layoutChildControls();
	
	virtual ColumnVector::size_type getColumnCount() const;
	virtual void setColumnCount(ColumnVector::size_type columns);
	
	virtual RowVector::size_type getRowCount() const;
	virtual void setRowCount(RowVector::size_type rows);
	
	virtual ColumnVector& getColumns();
	virtual RowVector& getRows();
	
	virtual Control* getControl(int x, int y);
	virtual void setControl(int x, int y, Control* control);
	
	virtual Padding& getCellPadding();
	virtual void setCellPadding(Padding& padding);
	
protected:

private:
	ColumnVector m_Columns;
	RowVector m_Rows;
	
	typedef std::vector<Ref<Control> > ControlVector;
	typedef std::vector<ControlVector> XYVector;

	Padding m_CellPadding;
	XYVector m_Controls;
	
	void resizeControlsVector();
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_TABLECONTROLCONTAINER_H__
