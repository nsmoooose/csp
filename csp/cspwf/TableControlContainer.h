#pragma once
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

#include <csp/cspwf/Container.h>
#include <csp/cspwf/Padding.h>

namespace csp {

namespace wf {

enum TableControlContainerSizeType {
	Percent,
	Absolute
};

/** A column in a table. This class provides information
 * about the column with width and how the width shall
 * be calculated.
 */
class CSPWF_EXPORT TableControlContainerColumn {
public:
	TableControlContainerColumn() : m_SizeType(Percent), m_Width(0.0f) {}

	double getWidth() const { return m_Width; }
	void setWidth(double width) { m_Width = width; }

	TableControlContainerSizeType getSizeType() const { return m_SizeType; }
	void setSizeType(TableControlContainerSizeType sizeType) { m_SizeType = sizeType; }

	template<class Archive>
	void serialize(Archive & ar) {
		ar & make_nvp("@Width", m_Width);
	}

private:
	TableControlContainerSizeType m_SizeType;
	double m_Width;
};

/** A row in a table container. This class provides information
 * about the height of the row and how the height shall
 * be calculated.
 */
class CSPWF_EXPORT TableControlContainerRow {
public:
	TableControlContainerRow() : m_SizeType(Percent), m_Height(0.0f) {}

	double getHeight() const { return m_Height; }
	void setHeight(double height) { m_Height = height; }

	TableControlContainerSizeType getSizeType() const { return m_SizeType; }
	void setSizeType(TableControlContainerSizeType sizeType) { m_SizeType = sizeType; }

	template<class Archive>
	void serialize(Archive & ar) {
		ar & make_nvp("@Height", m_Height);
	}

private:
	TableControlContainerSizeType m_SizeType;
	double m_Height;
};

/** A table container where you add controls in cells. The table
 * is specified as a number of rows and columns. */
class CSPWF_EXPORT TableControlContainer : public Container {
public:
	typedef std::vector<TableControlContainerColumn> ColumnVector;
	typedef std::vector<TableControlContainerRow> RowVector;
	typedef std::vector<ControlVector> XYVector;

	TableControlContainer();
	virtual ~TableControlContainer();

	virtual ControlVector getChildControls();

	virtual void performLayout();

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

	template<class Archive>
	void serialize(Archive & ar) {
		Container::serialize(ar);
		ar & make_nvp("ColumnDefinitions", m_Columns);
		ar & make_nvp("RowDefinitions", m_Rows);
		ar & make_nvp("Table", m_Controls);
		ar & make_nvp("@CellPaddingLeft", m_CellPadding.left);
		ar & make_nvp("@CellPaddingTop", m_CellPadding.top);
		ar & make_nvp("@CellPaddingRight", m_CellPadding.right);
		ar & make_nvp("@CellPaddingBottom", m_CellPadding.bottom);
	}

protected:

private:
	ColumnVector m_Columns;
	RowVector m_Rows;

	Padding m_CellPadding;
	XYVector m_Controls;

	void resizeControlsVector();
};

} // namespace wf
} // namespace csp
