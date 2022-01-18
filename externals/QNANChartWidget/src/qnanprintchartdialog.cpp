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

#include "qnanprintchartdialog.h"
#include "ui_qnanprintchartdialog.h"

#include <QDialog>
#include <QPrinter>
#include <QPrintDialog>

#include "qnanchartwidget.h"

QNANPrintChartDialog::QNANPrintChartDialog(QNANChartWidget * chart, QWidget * parent)
	: QDialog(parent), ui(new Ui::QNANPrintChartDialog), m_printer(0), m_chart(chart)
{
	ui->setupUi(this);
	ui->widthEdit->setText("160");
	ui->heightEdit->setText("120");
}

QNANPrintChartDialog::~QNANPrintChartDialog() {
	delete m_printer;
}

void QNANPrintChartDialog::setupDialog() {
	if (m_printer==0)
		m_printer = new QPrinter(QPrinter::PrinterResolution);
	// fill chart dialog with data from chart
	on_screenResolutionSpin_valueChanged(ui->screenResolutionSpin->value());
}

void QNANPrintChartDialog::on_printButton_clicked() {
	QPrintDialog printDialog(m_printer, this);
	if (printDialog.exec() == QDialog::Accepted) {
//		m_printer->setResolution(ui->printResolutionSpin->value());
		m_printer->setResolution(72);
		QPainter painter;
		painter.begin(m_printer);
		QRect s = m_printer->pageRect();
		// TODO : adjust for the margin
/*		double scale_x = double(s.width())/m_chart->width();
		double scale_y = double(s.height())/m_chart->height();
		double scale = std::min(scale_x, scale_y);
		painter.scale(scale, scale);
*/
		m_chart->m_penWidth = 0.8;
		painter.save();
		painter.translate(m_chart->m_x[0], m_chart->m_y[0]);
		m_chart->drawChart(1, &painter);
		painter.restore();
		m_chart->drawAll(&painter, false, false);
		painter.end();
		m_chart->m_penWidth = 0;
	}
}

void QNANPrintChartDialog::on_screenResolutionSpin_valueChanged(int res) {
	// convert chart margins from pixels to mm
	double dpi = res;
	double dpmm = dpi/25.4;
	ui->widthEdit->setText(QString("%1").arg(m_chart->width()/dpmm,0,'f',2));
	ui->heightEdit->setText(QString("%1").arg(m_chart->height()/dpmm,0,'f',2));
	ui->leftMarginEdit->setText(QString("%1").arg(m_chart->marginLeft()/dpmm,0,'f',2));
	ui->rightMarginEdit->setText(QString("%1").arg(m_chart->marginRight()/dpmm,0,'f',2));
	ui->topMarginEdit->setText(QString("%1").arg(m_chart->marginTop()/dpmm,0,'f',2));
	ui->bottomMarginEdit->setText(QString("%1").arg(m_chart->marginBottom()/dpmm,0,'f',2));
}

