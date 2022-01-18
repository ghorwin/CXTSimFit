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

#ifndef qnancharteditdialog_h
#define qnancharteditdialog_h

#include <QDialog>

namespace Ui {
	class QNANChartEditDialog;
}

class QTreeWidgetItem;
class QNANChartWidget;

/// @brief The chart appearance edit dialog.
/// @author Andreas Nicolai
///
class QNANChartEditDialog : public QDialog {
	Q_OBJECT
public:
	/// @brief Constructor.
	QNANChartEditDialog(QNANChartWidget * chart, QWidget * parent = 0);

	/// @brief Sets up a page of the edit dialog.
	///
	/// This function sets up the page indicated by section with data from m_chart.
	void setData(QTreeWidgetItem * section);

	/// @brief Stores the dialog data into the chart object.
	///
	/// This function reads the data from the chart and stores it into the chart
	void storeData(QTreeWidgetItem * section);

public slots:

	void optionSelected();
	void updateNumberFormatExamples();
	void checkBoxClicked();

	void dialogAccepted();
	void dialogRejected();

private:

	static int numberFormatToIndex(char numberFormat);
	static char numberFormatFromIndex(int index);


	Ui::QNANChartEditDialog		*ui;

	QTreeWidgetItem				*m_currentItem;
	QTreeWidgetItem				*m_leftAxisItem;
	QTreeWidgetItem				*m_rightAxisItem;
	QTreeWidgetItem				*m_topAxisItem;
	QTreeWidgetItem				*m_bottomAxisItem;

	QNANChartWidget				*m_chart;
};

#endif // charteditdialog_h
