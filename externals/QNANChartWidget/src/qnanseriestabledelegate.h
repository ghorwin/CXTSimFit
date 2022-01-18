/*******************************************************************************

QNANWidgets - Easy-to-use widgets for scientific and engineering applications
Copyright (C) 2007 Andreas Nicolai

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA 

*******************************************************************************/

#ifndef qnanseriestabledelegate_h
#define qnanseriestabledelegate_h

#include <QItemDelegate>

/// @brief Implements the various editors of the series edit table.
/// @author Andreas Nicolai
///
class QNANSeriesTableDelegate : public QItemDelegate {
	Q_OBJECT
public:
	QNANSeriesTableDelegate(QObject *parent = 0);
	
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
	
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;
	
	virtual void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // qnanseriestabledelegate_h
