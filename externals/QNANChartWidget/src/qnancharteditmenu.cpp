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

#include "qnancharteditmenu.h"

#include <QtGui>

#include "qnancharteditdialog.h"
#include "qnanserieseditdialog.h"

QNANChartEditMenu::QNANChartEditMenu(QNANChartWidget * chart, QObject * parent)
	: QObject(parent)
{
	m_chart = chart;
	
	m_editChartAction = new QAction(tr("Edit chart appearance..."), this);
	connect(m_editChartAction, SIGNAL(triggered()), this, SLOT(editChart()));
	m_editCurvesAction = new QAction(tr("Edit curves..."), this);
	connect(m_editCurvesAction, SIGNAL(triggered()), this, SLOT(editCurves()));
}

QAction * QNANChartEditMenu::preferredEditAction() const {
	return m_editChartAction;
}

QList<QAction *> QNANChartEditMenu::taskActions() const {
	QList<QAction *> list;
	list.append(m_editChartAction);
	list.append(m_editCurvesAction);
	return list;
}

void QNANChartEditMenu::editChart() {
	QNANChartEditDialog dlg(m_chart);
    dlg.exec();
}

void QNANChartEditMenu::editCurves() {
	QNANSeriesEditDialog dlg(m_chart);
    dlg.setupDialog();
    if (dlg.exec() == QDialog::Accepted) {
    	dlg.storeData();
    }
}
