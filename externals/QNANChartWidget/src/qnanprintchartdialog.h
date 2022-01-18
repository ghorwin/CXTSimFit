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

#ifndef qnanprintchartdialog_h
#define qnanprintchartdialog_h

#include <QDialog>

namespace Ui {
	class QNANPrintChartDialog;
}

class QNANChartWidget;
class QPrinter;

/// @brief The chart printing dialog.
/// @author Andreas Nicolai
///
class QNANPrintChartDialog : public QDialog {
	Q_OBJECT
public:
	/// @brief Constructor.
	QNANPrintChartDialog(QNANChartWidget * chart, QWidget * parent = 0);

	/// @brief Destructur, destroys the printer object.
	~QNANPrintChartDialog();

	/// @brief Sets up the printer dialog.
	///
	/// This function first sets up the dialog with the data from m_chart.
	void setupDialog();

private slots:
	void on_printButton_clicked();
	void on_screenResolutionSpin_valueChanged(int);

private:
	Ui::QNANPrintChartDialog	*ui;

	QPrinter					*m_printer;
	QNANChartWidget				*m_chart;
};

#endif // qnanprintchartdialog_h
