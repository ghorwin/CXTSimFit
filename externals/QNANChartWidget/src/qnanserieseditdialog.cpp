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

#include "qnanserieseditdialog.h"
#include "ui_qnanserieseditdialog.h"

#include <QMessageBox>

#include "qnanchartwidget.h"
#include "qnanseriestabledelegate.h"
#include "qnanabstractchartseries.h"
#include "qnandefaultchartseries.h"

#include <ctime>
#include <cmath>

QNANSeriesEditDialog::QNANSeriesEditDialog(QNANChartWidget * chart, QWidget * parent)
	: QDialog(parent), ui(new Ui::QNANSeriesEditDialog), m_chart(chart)
{
	ui->setupUi(this);

	QNANSeriesTableDelegate * delegate = new QNANSeriesTableDelegate(this);
	ui->seriesTableWidget->setItemDelegate(delegate);

	if (!chart->inDesigner()) {
		ui->removeBtn->setVisible(false);
		ui->addBtn->setVisible(false);
	}
	else {
		ui->removeBtn->setEnabled(false);
	}
	// we need the random number generator for the series
	srand(time(NULL));
}

void QNANSeriesEditDialog::setupDialog() {
	// clear table
	ui->seriesTableWidget->clearContents();
	// get series list from chart
	QMap<int,QNANAbstractChartSeries *>	seriesData = m_chart->seriesData();
	int i=0;
	for (QMap<int,QNANAbstractChartSeries *>::ConstIterator it = seriesData.constBegin();
		it != seriesData.constEnd(); ++it, ++i)
	{
		ui->seriesTableWidget->insertRow(i);
		fillSeriesData(i, it.value());
	}
	ui->seriesTableWidget->resizeColumnToContents(0);
	ui->seriesTableWidget->resizeColumnToContents(1);
	ui->seriesTableWidget->resizeColumnToContents(4);
	ui->seriesTableWidget->resizeColumnToContents(5);
	ui->seriesTableWidget->resizeColumnToContents(6);
}

void QNANSeriesEditDialog::storeData() {
	// get all the IDs of series stored in the table
	QList<int>	seriesIDs;
	for (int i = 0; i<ui->seriesTableWidget->rowCount(); ++i) {
		const QTableWidgetItem * item = ui->seriesTableWidget->item(i,0);
		seriesIDs.append(item->data(Qt::DisplayRole).toInt());
	}
	QMap<int,QNANAbstractChartSeries *>	seriesData = m_chart->seriesData();
	// loop over all data series and remove those that are no longer in the table
	for (QMap<int,QNANAbstractChartSeries *>::ConstIterator it = seriesData.constBegin();
		it != seriesData.constEnd(); ++it)
	{
		// remove all series from the chart that are not in the table
		if (seriesIDs.indexOf(it.key()) == -1)
			m_chart->removeSeries(it.key());
	}
	// now loop over all series in the table and get add them back into the chart
	for (int i = 0; i<ui->seriesTableWidget->rowCount(); ++i) {
		const QTableWidgetItem * item = ui->seriesTableWidget->item(i,0);
		int id = item->data(Qt::DisplayRole).toInt();
		QNANAbstractChartSeries * s = m_chart->takeSeries(id);
		QNANDefaultChartSeries * series = dynamic_cast<QNANDefaultChartSeries *>(s);
		if (s!=NULL && series==NULL) continue;
		if (s==NULL) {
			series = new QNANDefaultChartSeries(m_chart);
		}
		storeSeriesData(i, series);
		// if in designer, set random data to show how the data series will look like
		if (m_chart->inDesigner())
			series->setRandomValues();
		int new_id = m_chart->addSeries(series);
		series->setId(new_id);
	}
	m_chart->updateChart();
}

// *** slot implementations ***

void QNANSeriesEditDialog::on_addBtn_clicked() {
	// add a new row and write data of the new series into the new row
	int rows = ui->seriesTableWidget->rowCount();
	ui->seriesTableWidget->insertRow(rows);
	QNANDefaultChartSeries s;
	fillSeriesData(rows, &s);
}

void QNANSeriesEditDialog::on_removeBtn_clicked() {
	int selectedRow = ui->seriesTableWidget->currentRow();
	if (selectedRow == -1) return;
	QString seriesName = ui->seriesTableWidget->item(selectedRow, 1)->data(Qt::DisplayRole).toString();
	int result = QMessageBox::question(this, tr("Data series editor"),
		tr("Delete data series '%1'?").arg(seriesName), QMessageBox::Yes | QMessageBox::No);
	if (result != QMessageBox::Yes) return;
	if (ui->seriesTableWidget->rowCount() > 0)
		ui->seriesTableWidget->removeRow(qMax(selectedRow, ui->seriesTableWidget->rowCount()-1));
	else
		ui->removeBtn->setEnabled(false);
	ui->seriesTableWidget->setCurrentCell(selectedRow,0);
}

void QNANSeriesEditDialog::on_aboutBtn_clicked() {
	qApp->aboutQt();
}

void QNANSeriesEditDialog::on_seriesTableWidget_currentCellChanged(int /* row */, int /* column */) {
	if (ui->seriesTableWidget->rowCount() > 0)
		ui->removeBtn->setEnabled(true);
}

// *** private member functions ***

void QNANSeriesEditDialog::fillSeriesData(int row, const QNANAbstractChartSeries * s) {
	Q_ASSERT(row < ui->seriesTableWidget->rowCount());

	Qt::ItemFlags DefaultItemState(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	Qt::Alignment DefaultTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	const QNANDefaultChartSeries * series = dynamic_cast<const QNANDefaultChartSeries *>(s);
	if (series==NULL) {
		QTableWidgetItem *i = new QTableWidgetItem(tr("%1").arg(s->id()));
		i->setFlags(0);
		i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
		ui->seriesTableWidget->setItem(row, 0, i);

		i = new QTableWidgetItem(s->name());
		i->setFlags(0);
		i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
		ui->seriesTableWidget->setItem(row, 1, i);

		i = new QTableWidgetItem(tr("<unknown series type>"));
		i->setFlags(0);
		i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
		ui->seriesTableWidget->setItem(row, 3, i);
		return;
	}

	QTableWidgetItem *i = new QTableWidgetItem(tr("%1").arg(series->id()));
	i->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	ui->seriesTableWidget->setItem(row, 0, i);

	i = new QTableWidgetItem(series->name());
	i->setFlags(DefaultItemState);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	ui->seriesTableWidget->setItem(row, 1, i);

	i = new QTableWidgetItem("");
	i->setData(Qt::BackgroundRole, QBrush(series->color()));
	i->setData(Qt::UserRole, series->color());
	i->setFlags(DefaultItemState);
	ui->seriesTableWidget->setItem(row, 2, i);

	i = new QTableWidgetItem(
			QNANDefaultChartSeries::stringForSeriesType(series->seriesType())
		);
	i->setData(Qt::UserRole, series->seriesType());
	i->setFlags(DefaultItemState);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	ui->seriesTableWidget->setItem(row, 3, i);

	i = new QTableWidgetItem(
			QNANDefaultChartSeries::stringForLineStyle(series->lineStyle())
		);
	i->setData(Qt::UserRole, series->lineStyle());
	i->setFlags(DefaultItemState);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	ui->seriesTableWidget->setItem(row, 4, i);

	i = new QTableWidgetItem(
			QNANDefaultChartSeries::stringForMarkerStyle(series->markerStyle())
		);
	i->setData(Qt::UserRole, series->markerStyle());
	i->setFlags(DefaultItemState);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	ui->seriesTableWidget->setItem(row, 5, i);

	i = new QTableWidgetItem(QString("%1").arg(series->markerSize()));
	i->setFlags(DefaultItemState);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	ui->seriesTableWidget->setItem(row, 6, i);

	QString axisString;
	switch (series->xAxis()) {
		case 0 : axisString = tr("X1 (bottom)"); break;
		case 1 : axisString = tr("X2 (top)"); break;
	}
	i = new QTableWidgetItem(axisString);
	i->setFlags(DefaultItemState);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	i->setData(Qt::UserRole, series->xAxis());
	ui->seriesTableWidget->setItem(row, 7, i);

	switch (series->yAxis()) {
		case 0 : axisString = tr("Y1 (left)"); break;
		case 1 : axisString = tr("Y2 (right)"); break;
	}
	i = new QTableWidgetItem(axisString);
	i->setFlags(DefaultItemState);
	i->setData(Qt::TextAlignmentRole, QVariant(DefaultTextAlignment));
	i->setData(Qt::UserRole, series->yAxis());
	ui->seriesTableWidget->setItem(row, 8, i);
}

void QNANSeriesEditDialog::storeSeriesData(int row, QNANDefaultChartSeries * s) {
	Q_ASSERT(row < ui->seriesTableWidget->rowCount());

	const QTableWidgetItem * item = ui->seriesTableWidget->item(row,0);
	int id = item->data(Qt::DisplayRole).toInt();
	s->setId(id);

	item = ui->seriesTableWidget->item(row,1);
	s->setName(item->data(Qt::DisplayRole).toString());

	item = ui->seriesTableWidget->item(row,2);
	s->setColor( item->data(Qt::BackgroundRole).value<QColor>());

	item = ui->seriesTableWidget->item(row,3);
	s->setSeriesType(  static_cast<QNANDefaultChartSeries::SeriesType>(item->data(Qt::UserRole).toInt()) );

	item = ui->seriesTableWidget->item(row,4);
	s->setLineStyle(  static_cast<QNANDefaultChartSeries::LineStyle>(item->data(Qt::UserRole).toInt()) );

	item = ui->seriesTableWidget->item(row,5);
	s->setMarkerStyle(  static_cast<QNANDefaultChartSeries::MarkerStyle>(item->data(Qt::UserRole).toInt()) );

	item = ui->seriesTableWidget->item(row,6);
	s->setMarkerSize(  item->data(Qt::DisplayRole).toInt() );

	item = ui->seriesTableWidget->item(row,7);
	s->setXAxis(  item->data(Qt::UserRole).toInt() );

	item = ui->seriesTableWidget->item(row,8);
	s->setYAxis(  item->data(Qt::UserRole).toInt() );
}

