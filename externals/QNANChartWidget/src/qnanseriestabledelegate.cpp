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

#include "qnanseriestabledelegate.h"

#include <QColorDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDebug>

#include "qnandefaultchartseries.h"

QNANSeriesTableDelegate::QNANSeriesTableDelegate(QObject *parent)
	: QItemDelegate(parent)
{
}

QWidget * QNANSeriesTableDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &option,	const QModelIndex &index) const
{
	// for each of the columns we have a different editor
	switch (index.column()) {
		case 0 : break; // no editor for ID
		case 1 : break; // default editor for name
		case 2 : { // color picker dialog
			QColor col(index.data(Qt::UserRole).value<QColor>());
			col = QColorDialog::getColor(col, parent);
			if (col.isValid()) {
				// store color
				QAbstractItemModel * model = const_cast<QAbstractItemModel *>(index.model());
				model->setData(index, col, Qt::UserRole);
				model->setData(index, QBrush(col), Qt::BackgroundRole);

				//index.setData(Qt::BackgroundRole, QBrush(col));
				//index.setData(Qt::UserRole, col);
			}
		}
		return 0;
/*			QComboBox * c  = new QComboBox(parent);
			for (int i=0; i<=QNANDefaultChartSeries::NUM_DEFAULT_COLORS; ++i) {
				QPixmap p(60, 60);
				p.fill(QColor(QNANDefaultChartSeries::seriesDefaultColors[i]));
				c->addItem(QIcon(p),"", QNANDefaultChartSeries::seriesDefaultColors[i]);
			}
			c->installEventFilter(const_cast<QNANSeriesTableDelegate*>(this));
			return c;
*/

		case 3 : // SeriesType
		{
			QComboBox * c  = new QComboBox(parent);
			for (int i=0; i<=QNANDefaultChartSeries::LineAndMarker; ++i) {
				c->addItem(QNANDefaultChartSeries::stringForSeriesType(
						static_cast<QNANDefaultChartSeries::SeriesType>(i)));
			}
			c->installEventFilter(const_cast<QNANSeriesTableDelegate*>(this));
			return c;
		}

		case 4 : // LineStyle
		{
			QComboBox * c  = new QComboBox(parent);
			for (int i=0; i<=QNANDefaultChartSeries::DashDot; ++i) {
				c->addItem(QNANDefaultChartSeries::stringForLineStyle(
						static_cast<QNANDefaultChartSeries::LineStyle>(i)));
			}
			c->installEventFilter(const_cast<QNANSeriesTableDelegate*>(this));
			return c;
		}

		case 5 : // MarkerStyle
		{
			QComboBox * c  = new QComboBox(parent);
			for (int i=0; i<=QNANDefaultChartSeries::Triangle; ++i) {
				c->addItem(QNANDefaultChartSeries::stringForMarkerStyle(
					static_cast<QNANDefaultChartSeries::MarkerStyle>(i)));
			}
			c->installEventFilter(const_cast<QNANSeriesTableDelegate*>(this));
			return c;
		}

		case 6 : // MarkerSize
		{
			QSpinBox *editor = new QSpinBox(parent);
			editor->setMinimum(1);
			editor->setMaximum(12);
			editor->installEventFilter(const_cast<QNANSeriesTableDelegate*>(this));
			return editor;
		}

		case 7 : // X Axis
		{
			QComboBox * c  = new QComboBox(parent);
			c->addItem(tr("X1 (bottom)"));
			c->addItem(tr("X2 (top)"));
			c->installEventFilter(const_cast<QNANSeriesTableDelegate*>(this));
			return c;
		}

		case 8 : // Y Axis
		{
			QComboBox * c  = new QComboBox(parent);
			c->addItem(tr("Y1 (left)"));
			c->addItem(tr("Y2 (right)"));
			c->installEventFilter(const_cast<QNANSeriesTableDelegate*>(this));
			return c;
		}

	 }

	 return QItemDelegate::createEditor(parent, option, index);
}

void QNANSeriesTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	// for each of the columns we have a different editor
	switch (index.column()) {
		case 0 : break; // no editor for ID
		case 1 : break; // default editor for name
		case 2 : // create color picker widget
			break;

		case 3 : ; // SeriesType
		case 4 : ; // LineStyle
		case 5 : ; // MarkerStyle
		case 7 : ; // X axis
		case 8 : ; // Y axis
		{
			int value = index.model()->data(index, Qt::UserRole).toInt();
			QComboBox * c = static_cast<QComboBox*>(editor);
			c->setCurrentIndex(value);
			return;
		}

		case 6 : // MarkerSize
		{
			int value = index.model()->data(index, Qt::DisplayRole).toInt();
			QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
			spinBox->setValue(value);
			return;
		}

	}
	QItemDelegate::setEditorData(editor, index);
}


void QNANSeriesTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	// for each of the columns we have a different editor
	switch (index.column()) {
		case 0 : break; // no editor for ID
		case 1 : break; // default editor for name
		case 2 : // create color picker widget
			qDebug() << "Need to set the data" << endl;
			break;

		case 3 : // SeriesType
		{
			QComboBox * c = static_cast<QComboBox*>(editor);
			int value = c->currentIndex();
			model->setData(index, value, Qt::UserRole);
			QNANDefaultChartSeries::SeriesType t = static_cast<QNANDefaultChartSeries::SeriesType>(value);
			model->setData(index, QNANDefaultChartSeries::stringForSeriesType(t), Qt::DisplayRole);
			return;
		}

		case 4 : // LineStyle
		{
			QComboBox * c = static_cast<QComboBox*>(editor);
			int value = c->currentIndex();
			model->setData(index, value, Qt::UserRole);
			QNANDefaultChartSeries::LineStyle t = static_cast<QNANDefaultChartSeries::LineStyle>(value);
			model->setData(index, QNANDefaultChartSeries::stringForLineStyle(t), Qt::DisplayRole);
			return;
		}

		case 5 : // MarkerStyle
		{
			QComboBox * c = static_cast<QComboBox*>(editor);
			int value = c->currentIndex();
			model->setData(index, value, Qt::UserRole);
			QNANDefaultChartSeries::MarkerStyle t = static_cast<QNANDefaultChartSeries::MarkerStyle>(value);
			model->setData(index, QNANDefaultChartSeries::stringForMarkerStyle(t), Qt::DisplayRole);
			return;
		}

		case 7 : // X axis
		{
			QComboBox * c = static_cast<QComboBox*>(editor);
			int value = c->currentIndex();
			model->setData(index, value, Qt::UserRole);
			QString axisString;
			switch (value) {
				case 0 : axisString = tr("X1 (bottom)"); break;
				case 1 : axisString = tr("X2 (top)"); break;
			}
			model->setData(index, axisString, Qt::DisplayRole);
			return;
		}

		case 8 : // Y axis
		{
			QComboBox * c = static_cast<QComboBox*>(editor);
			int value = c->currentIndex();
			model->setData(index, value, Qt::UserRole);
			QString axisString;
			switch (value) {
				case 0 : axisString = tr("Y1 (left)"); break;
				case 1 : axisString = tr("Y2 (right)"); break;
			}
			model->setData(index, axisString, Qt::DisplayRole);
			return;
		}
	}
	QItemDelegate::setModelData(editor, model, index);
}

void QNANSeriesTableDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &  /*index */) const
{
	editor->setGeometry(option.rect);
}

