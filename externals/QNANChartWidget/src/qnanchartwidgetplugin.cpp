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

#include <QtCore/QtPlugin>

#include <QDesignerFormEditorInterface>
#include <QExtensionManager>
#include <QDesignerTaskMenuExtension>

#include "qnanchartwidget.h"
#include "qnanchartwidgetplugin.h"
#include "qnancharteditmenufactory.h"

QNANChartWidgetPlugin::QNANChartWidgetPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void QNANChartWidgetPlugin::initialize(QDesignerFormEditorInterface * formEditor)
{
	// prevent multiple initializations
	if (initialized)	return;

	// register our own extension 
	QExtensionManager * manager = formEditor->extensionManager();
    Q_ASSERT(manager != 0);
    // add our own context menu extension
	manager->registerExtensions(new QNANChartEditMenuFactory(manager),
                                Q_TYPEID(QDesignerTaskMenuExtension));

	initialized = true;
}

bool QNANChartWidgetPlugin::isInitialized() const
{
	return initialized;
}

QWidget *QNANChartWidgetPlugin::createWidget(QWidget *parent)
{
	QNANChartWidget * w = new QNANChartWidget(parent);
	w->m_inDesigner = true;
	return w;
}

QString QNANChartWidgetPlugin::name() const
{
	return "QNANChartWidget";
}

QString QNANChartWidgetPlugin::group() const
{
	return "QNANWidgets";
}

QIcon QNANChartWidgetPlugin::icon() const
{
	return QIcon(":/QNANChartWidget/icon.png");
}

QString QNANChartWidgetPlugin::toolTip() const
{
	return "Use the context menu options to edit the chart and data series.";
}

QString QNANChartWidgetPlugin::whatsThis() const
{
	return "";
}

bool QNANChartWidgetPlugin::isContainer() const
{
	return false;
}

QString QNANChartWidgetPlugin::domXml() const
{
	return "<widget class=\"QNANChartWidget\" name=\"chart\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>350</width>\n"
		"   <height>250</height>\n"
		"  </rect>\n"
		" </property>\n"
		" <property name=\"chartSettings\">\n"
		"  <string>&lt;QNANChart>\n"
		"   &lt;/QNANChart>\n"
		"  </string>\n"
		" </property>\n"
		"</widget>\n";
}

QString QNANChartWidgetPlugin::includeFile() const
{
	return "qnanchartwidget.h";
}

Q_EXPORT_PLUGIN2(qnanchartwidgetplugin, QNANChartWidgetPlugin)
