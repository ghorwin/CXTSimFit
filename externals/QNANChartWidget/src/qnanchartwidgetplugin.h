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

#ifndef qnanchartwidgetplugin_h
#define qnanchartwidgetplugin_h

#include <QDesignerCustomWidgetInterface>

/// @brief The plugin class for integration of the QNANChartWidget into Qt designer.
/// @author Andreas Nicolai
///
/// Contains implementation of a few default functions that expose certain properties of the widget
/// to the designer and most importantly create the widget.
class QNANChartWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
	/// @brief Constructor.
	QNANChartWidgetPlugin(QObject *parent = 0);

	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	
	/// @brief This function initializes the widget inside the editor.
	void initialize(QDesignerFormEditorInterface * formEditor);

	/// @brief This function creates the chart widget that is shown in the editor.
	/// Upon creating the chart widget for use in Qt designer also the inDesigner
	/// flag is set, so that the chart can do some special stuff inside the designer.
	/// Also a context menu is registered for the chart so that the chart appearance
	/// can be easily edited.
	QWidget *createWidget(QWidget * parent);

private:
	bool initialized;
};

#endif // qnanchartwidgetplugin_h
