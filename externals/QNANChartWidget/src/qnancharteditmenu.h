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

#ifndef qnancharteditmenu_h
#define qnancharteditmenu_h

#include <QDesignerTaskMenuExtension>
#include <QObject>

class QNANChartWidget;

/// @brief The chart edit menu extension in Qt Designer.
/// @author Andreas Nicolai
///
class QNANChartEditMenu : public QObject, public QDesignerTaskMenuExtension
{
	Q_OBJECT
	Q_INTERFACES(QDesignerTaskMenuExtension)

public:
	/// @brief Constructor.
	QNANChartEditMenu(QNANChartWidget * chart, QObject * parent);

	/// @brief Returns the preferred action to be executed when user double-clicks on the chart.
	QAction * preferredEditAction() const;
	
	/// @brief Returns the list of actions to be merged into the widget menu in Qt Designer.
	QList<QAction *> taskActions() const;

private slots:
	/// @brief Opens the edit-chart dialog where the user can fine tune the appearance of the chart.
	void editChart();
	/// @brief Opens the curve-chart dialog where the user can create and customize curves.
	void editCurves();

private:
	QAction 			* m_editChartAction;
	QAction 			* m_editCurvesAction;
	QNANChartWidget 	* m_chart;
};

#endif // qnancharteditmenu_h
