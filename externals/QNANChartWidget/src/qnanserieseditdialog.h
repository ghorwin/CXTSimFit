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

#ifndef qnanserieseditdialog_h
#define qnanserieseditdialog_h

#include <QDialog>
#include <QList>

namespace Ui {
	class QNANSeriesEditDialog;
}

class QNANChartWidget;
class QNANDefaultChartSeries;
class QNANAbstractChartSeries;

/// @brief The chart series edit dialog.
/// @author Andreas Nicolai
///
class QNANSeriesEditDialog : public QDialog {
	Q_OBJECT
public:
	/// @brief Constructor.
	QNANSeriesEditDialog(QNANChartWidget * chart, QWidget * parent = 0);

	/// @brief Sets up the dialog.
	///
	/// This function takes all the DefaultChartSeries objects from the
	/// chart and fill the edit table with the data.
	void setupDialog();

	/// @brief Stores the dialog data into the chart object.
	///
	/// This function reads the data from the table and stores it into the chart,
	/// overwriting data series with same IDs that are already in the chart.
	void storeData();

public slots:
	void on_addBtn_clicked();
	void on_removeBtn_clicked();
	void on_aboutBtn_clicked();
	void on_seriesTableWidget_currentCellChanged(int row, int column);

private:
	void fillSeriesData(int row, const QNANAbstractChartSeries * s);
	void storeSeriesData(int row, QNANDefaultChartSeries * s);


	Ui::QNANSeriesEditDialog	*ui;

	QNANChartWidget				*m_chart;
};

#endif // qnanserieseditdialog_h
