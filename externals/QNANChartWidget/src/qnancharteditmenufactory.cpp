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

#include "qnancharteditmenufactory.h"

#include "qnanchartwidget.h"
#include "qnancharteditmenu.h"

QNANChartEditMenuFactory::QNANChartEditMenuFactory(QExtensionManager *parent)
	: QExtensionFactory(parent)
{
}

QObject *QNANChartEditMenuFactory::createExtension(QObject *object,
	const QString &iid,	QObject *parent) const
{
	if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
		return 0;

	if (QNANChartWidget *chart = qobject_cast<QNANChartWidget*>(object))
		return new QNANChartEditMenu(chart, parent);

    return 0;
}
