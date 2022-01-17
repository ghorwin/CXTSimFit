#include <QtGui>
#include <QNANChartWidget>
#include "inspectprofiledialog.h"

InspectProfileDialog::InspectProfileDialog(QWidget *parent, const SolverResults & res)
	: QDialog(parent, Qt::WindowMinMaxButtonsHint), results(res)
{
	ui.setupUi(this);

	resize(600,500);

	// setup the time range
	ui.horizontalSlider->setValue(0);
	ui.horizontalSlider->setMaximum(results.ccProfiles.size()-1);

	// set the maximum time label
	ui.labelMaxSimTime->setText(QString("%1 h").arg(results.input.tEnd/3600));

	// setup the chart
	ui.chart->setMarginRight(60);

	// create data series for measured data
	ccCurve.init(ui.chart);
	scCurve.init(ui.chart);

	ccCurve.series->setColor(QColor("Brown"));
	ccCurve.series->setMarkerStyle(QNANDefaultChartSeries::Box);
	ccCurve.series->setMarkerSize(4);
	ccCurve.series->setSeriesType(QNANDefaultChartSeries::LineAndMarker);

	scCurve.series->setColor(QColor("Indigo"));
	scCurve.series->setMarkerStyle(QNANDefaultChartSeries::Diamond);
	scCurve.series->setMarkerSize(4);
	scCurve.series->setYAxis(1);
	scCurve.series->setSeriesType(QNANDefaultChartSeries::LineAndMarker);

	// customize chart
	ui.chart->x1Axis()->setLabel(tr("Location [m]"));
	ui.chart->y1Axis()->setLabel(tr("Concentration [kg/m3]"));
	if (results.input.model == SolverInput::PLUS_EXCHANGE) {
		ui.chart->y1Axis()->setLabel(tr("Concentration c [kg/m3]"));
		ui.chart->y2Axis()->setLabel(tr("Concentration s [kg/m3]"));
		ui.chart->setY2AxisVisible(true);
	}

	// generate x coordinates
	double dx = results.input.L/results.input.n;
	for (int i=0; i<results.input.n; ++i) {
		x.push_back(dx*(i + 0.5));
	}
	x.front() = 0;
	x.back() = results.input.L; // to avoid rounding errors

	// find global max values
	double maxcc = 0;
	double maxsc = 0;
	for (int i=0; i<results.ccProfiles.size(); ++i) {
		for (int j=0; j<results.input.n; ++j) {
			maxcc = std::max(maxcc, results.ccProfiles[i][j]);
			if (results.input.model == SolverInput::PLUS_EXCHANGE) {
				maxsc = std::max(maxsc, results.scProfiles[i][j]);
			}
		}
	}

	ui.chart->x1Axis()->setAutomaticScaling(false);
	ui.chart->x1Axis()->setMinVal(0);
	ui.chart->x1Axis()->setMaxVal(results.input.L);
	ui.chart->y1Axis()->setAutomaticScaling(false);
	ui.chart->y1Axis()->setMinVal(0);
	ui.chart->y1Axis()->setMaxVal(maxcc);
	if (results.input.model == SolverInput::PLUS_EXCHANGE) {
		ui.chart->y2Axis()->setAutomaticScaling(false);
		ui.chart->y2Axis()->setMinVal(0);
		ui.chart->y2Axis()->setMaxVal(maxsc);
	}

	// finally update the chart 
	on_horizontalSlider_valueChanged(0);
}

InspectProfileDialog::~InspectProfileDialog() {
}

void InspectProfileDialog::on_pushButtonClose_clicked() {
	close();
}

void InspectProfileDialog::on_horizontalSlider_valueChanged(int) {
	// update chart caption
	int val = ui.horizontalSlider->value();
	Q_ASSERT(val < results.tProfiles.size());
	double t = results.tProfiles[val];

	ui.labelTitle->setText(QString("Profiles at %1 h").arg(t));

	// now set the data values in the series
	ccCurve.series->setData(results.input.n, &x[0], &(results.ccProfiles[val][0]));
	if (results.input.model == SolverInput::PLUS_EXCHANGE) {
		scCurve.series->setData(results.input.n, &x[0], &(results.scProfiles[val][0]));
	}
	ui.chart->updateChart();
}
