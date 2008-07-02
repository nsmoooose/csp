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
 * @file TableControlContainer.cpp
 *
 **/

#include <csp/cspwf/TableControlContainer.h>
#include <osg/Group>

namespace csp {

namespace wf {

TableControlContainer::TableControlContainer() : 
	Container("TableControlContainer") {
	m_CellPadding.setAll(2.0f);
}

TableControlContainer::~TableControlContainer() {
}

ControlVector TableControlContainer::getChildControls() {
	ControlVector childControls;
	ColumnVector::size_type columnCount = m_Columns.size();
	RowVector::size_type rowCount = m_Rows.size();
	for(ColumnVector::size_type x=0;x<columnCount;++x) {
		for(ColumnVector::size_type y=0;y<rowCount;++y) {
			Ref<Control> control = m_Controls[x][y];
			if(!control.valid())
				continue;
			
			childControls.push_back(control);
		}
	}
	return childControls;
}

void TableControlContainer::buildGeometry() {
	Container::buildGeometry();
	
	osg::Group* group = getNode();
	ControlVector childControls = getChildControls();
	ControlVector::iterator childControl = childControls.begin();
	for(;childControl != childControls.end();++childControl) {
		(*childControl)->buildGeometry();
		group->addChild((*childControl)->getNode());
	}
}

void TableControlContainer::layoutChildControls() {
	ColumnVector::size_type columnCount = m_Columns.size();
	RowVector::size_type rowCount = m_Rows.size();
	
	Size tableSize = getSize();
	
	Point point(0, 0);
	for(ColumnVector::size_type x=0;x<columnCount;++x) {
		const TableControlContainerColumn& column = m_Columns[x];
		
		point.y = 0;
		for(ColumnVector::size_type y=0;y<rowCount;++y) {
			const TableControlContainerRow& row = m_Rows[y];
					
			Ref<Control> control = m_Controls[x][y];
			if(control.valid()) {		
				double width = tableSize.width * column.getWidth() - m_CellPadding.left - m_CellPadding.right;
				double height = tableSize.height * row.getHeight() - m_CellPadding.top - m_CellPadding.bottom;

				Ref<Style> style = control->getStyle();
				style->setWidth(Style::UnitValue(Style::Pixels, width));
				style->setHeight(Style::UnitValue(Style::Pixels, height));
				style->setLeft(Style::UnitValue(Style::Pixels, point.x + m_CellPadding.left));
				style->setTop(Style::UnitValue(Style::Pixels, point.y + m_CellPadding.top));  
				
				Container* controlContainer = dynamic_cast<Container*>(control.get());
				if(controlContainer != NULL) {
					controlContainer->layoutChildControls();
				}
			}
			
			point.y += (tableSize.height * row.getHeight());
		}
		point.x += (tableSize.width * column.getWidth());
	}
}

TableControlContainer::ColumnVector::size_type TableControlContainer::getColumnCount() const {
	return m_Columns.size();
}

void TableControlContainer::setColumnCount(ColumnVector::size_type columns) {
	m_Columns.resize(columns);
	resizeControlsVector();
}

TableControlContainer::RowVector::size_type TableControlContainer::getRowCount() const {
	return m_Rows.size();
}

void TableControlContainer::setRowCount(RowVector::size_type rows) {
	m_Rows.resize(rows);
	resizeControlsVector();
}

TableControlContainer::ColumnVector& TableControlContainer::getColumns() {
	return m_Columns;
}

TableControlContainer::RowVector& TableControlContainer::getRows() {
	return m_Rows;
}

Padding& TableControlContainer::getCellPadding() {
	return m_CellPadding;
}

void TableControlContainer::setCellPadding(Padding& padding) {
	m_CellPadding = padding;
}

Control* TableControlContainer::getControl(int x, int y) {
	return m_Controls[x][y].get();
}

void TableControlContainer::setControl(int x, int y, Control* control) {
	m_Controls[x][y] = control;
}

void TableControlContainer::resizeControlsVector() {
	m_Controls.resize(getColumnCount());
	for(ColumnVector::size_type i=0;i<getColumnCount();++i) {
		m_Controls[i].resize(getRowCount());
	}
}

} // namespace wf

} // namespace csp
