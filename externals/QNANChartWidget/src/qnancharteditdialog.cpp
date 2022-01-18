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

#include "qnancharteditdialog.h"
#include "ui_qnancharteditdialog.h"

#include <QtGui>

#include "qnanchartwidget.h"
#include "qnanchartaxis.h"

QNANChartEditDialog::QNANChartEditDialog(QNANChartWidget * chart, QWidget * parent)
	: QDialog(parent), ui(new Ui::QNANChartEditDialog), m_chart(chart)
{
	ui->setupUi(this);

	// create and connect actions
	connect(ui->optionsTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(optionSelected()) );
	connect(ui->numberFormatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNumberFormatExamples()) );
	connect(ui->precisionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateNumberFormatExamples()) );
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()) );
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(dialogRejected()) );
	connect(ui->axisAutomaticCheck, SIGNAL(clicked()), this, SLOT(checkBoxClicked()) );
	connect(ui->automaticTickDistanceCheck, SIGNAL(clicked()), this, SLOT(checkBoxClicked()) );

	// build options tree widget
	ui->optionsTreeWidget->clear();
	QTreeWidgetItem * axisRoot = new QTreeWidgetItem(ui->optionsTreeWidget, QStringList( tr("Axis properties") ) );
	ui->optionsTreeWidget->insertTopLevelItem(0, axisRoot);
	ui->optionsTreeWidget->setHeaderLabel( tr("Chart properties") );
	axisRoot->setFlags(Qt::ItemIsEnabled);

	m_leftAxisItem = new QTreeWidgetItem(axisRoot, QStringList( tr("Left axis (primary Y axis)") ));
	m_bottomAxisItem = new QTreeWidgetItem(axisRoot, QStringList( tr("Bottom axis (primary X axis)") ));
	m_rightAxisItem = new QTreeWidgetItem(axisRoot, QStringList( tr("Right axis (secondary Y axis)") ));
	m_topAxisItem = new QTreeWidgetItem(axisRoot, QStringList( tr("Top axis (secondary X axis)") ));
	ui->optionsTreeWidget->expandAll();
	m_currentItem = 0;
	m_leftAxisItem->setSelected(true);

	QPalette p = ui->exampleLineEdit1->palette();
	p.setColor(QPalette::Base, p.color(QPalette::Disabled, QPalette::Base) );
	ui->exampleLineEdit1->setPalette(p);
	ui->exampleLineEdit2->setPalette(p);
	ui->exampleLineEdit3->setPalette(p);
}

void QNANChartEditDialog::setData(QTreeWidgetItem * section) {
	QNANChartAxis * axis = NULL;
	// select correct page
	if (section == m_leftAxisItem) {
		ui->stackedWidget->setCurrentIndex(0);
		ui->showAxisCheckBox->setEnabled(false);
		ui->showAxisCheckBox->setChecked(true);
		axis = m_chart->y1Axis();
	}
	else if (section == m_bottomAxisItem) {
		ui->stackedWidget->setCurrentIndex(0);
		ui->showAxisCheckBox->setEnabled(false);
		ui->showAxisCheckBox->setChecked(true);
		axis = m_chart->x1Axis();
	}
	else if (section == m_rightAxisItem) {
		ui->stackedWidget->setCurrentIndex(0);
		ui->showAxisCheckBox->setEnabled(true);
		ui->showAxisCheckBox->setChecked(m_chart->y2AxisVisible());
		axis = m_chart->y2Axis();
	}
	else if (section == m_topAxisItem) {
		ui->stackedWidget->setCurrentIndex(0);
		ui->showAxisCheckBox->setEnabled(true);
		ui->showAxisCheckBox->setChecked(m_chart->x2AxisVisible());
		axis = m_chart->x2Axis();
	}
	if (axis != NULL) {
		ui->axisLabelEdit->setText(axis->label());
		ui->axisAutomaticCheck->setChecked(axis->automaticScaling());
		ui->automaticTickDistanceCheck->setChecked(axis->automaticTickMarkDistance());
		ui->maxEdit->setText(QString("%1").arg(axis->maxVal()));
		ui->minEdit->setText(QString("%1").arg(axis->minVal()));
		ui->tickDistanceEdit->setText(QString("%1").arg(axis->tickMarkDistance()));
		ui->numberFormatComboBox->setCurrentIndex( numberFormatToIndex(axis->numberFormat()) );
		ui->precisionSpinBox->setValue(axis->precision() );
		updateNumberFormatExamples();
		checkBoxClicked();
	}

	m_currentItem = section;
}

void QNANChartEditDialog::storeData(QTreeWidgetItem * section) {
	if (section == 0) return;
	QNANChartAxis * axis = NULL;
	if (section == m_leftAxisItem) {
		axis = m_chart->y1Axis();
	}
	else if (section == m_bottomAxisItem) {
		axis = m_chart->x1Axis();
	}
	else if (section == m_rightAxisItem) {
		axis = m_chart->y2Axis();
		m_chart->setY2AxisVisible( ui->showAxisCheckBox->isChecked() );
	}
	else if (section == m_topAxisItem) {
		axis = m_chart->x2Axis();
		m_chart->setX2AxisVisible( ui->showAxisCheckBox->isChecked() );
	}
	if (axis != NULL) {
		axis->setLabel(ui->axisLabelEdit->text());
		axis->setAutomaticScaling(ui->axisAutomaticCheck->isChecked());
		axis->setAutomaticTickMarkDistance( ui->automaticTickDistanceCheck->isChecked());
		axis->setMaxVal(ui->maxEdit->text().toDouble());
		axis->setMinVal(ui->minEdit->text().toDouble());
		axis->setTickMarkDistance(ui->tickDistanceEdit->text().toDouble());
		axis->setNumberFormat( numberFormatFromIndex(ui->numberFormatComboBox->currentIndex()) );
		axis->setPrecision( ui->precisionSpinBox->value() );
	}
}

void QNANChartEditDialog::optionSelected() {
//	qDebug() << "[QNANChartEditDialog::optionSelected]";
	// get selected tree list widgets
	QList<QTreeWidgetItem *> items = ui->optionsTreeWidget->selectedItems();
	if (items.isEmpty()) return;
	// store current data
	storeData(m_currentItem);
	// put data to widget
	setData(items.first());
}

void QNANChartEditDialog::updateNumberFormatExamples() {
	char format = numberFormatFromIndex(ui->numberFormatComboBox->currentIndex());
	int precision = ui->precisionSpinBox->value();
	ui->exampleLineEdit1->setText( QString("%1").arg(1.23456e-2, 0, format, precision) );
	ui->exampleLineEdit2->setText( QString("%1").arg(12.3456, 0, format, precision) );
	ui->exampleLineEdit3->setText( QString("%1").arg(123456000.0, 0, format, precision) );
}

void QNANChartEditDialog::checkBoxClicked() {
	ui->maxLabel->setEnabled( !ui->axisAutomaticCheck->isChecked() );
	ui->minLabel->setEnabled( !ui->axisAutomaticCheck->isChecked() );
	ui->maxEdit->setEnabled( !ui->axisAutomaticCheck->isChecked() );
	ui->minEdit->setEnabled( !ui->axisAutomaticCheck->isChecked() );
	ui->tickDistanceLabel->setEnabled( !ui->automaticTickDistanceCheck->isChecked());
	ui->tickDistanceEdit->setEnabled( !ui->automaticTickDistanceCheck->isChecked());
}

void QNANChartEditDialog::dialogAccepted() {
	storeData(m_currentItem);
}

void QNANChartEditDialog::dialogRejected() {
	// TODO : restore state of chart before editing
}


int QNANChartEditDialog::numberFormatToIndex(char numberFormat) {
	switch (numberFormat) {
		case 'g' : return 0;
		case 'e' : return 1;
		case 'f' :
		default	 : return 2;
	}
}

char QNANChartEditDialog::numberFormatFromIndex(int index) {
	switch (index) {
		case 0 : return 'g';
		case 1 : return 'e';
		case 2 :
		default : return 'f';
	}
}
