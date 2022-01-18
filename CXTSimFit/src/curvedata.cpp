#include <QtGui>
#include <qnanchartwidget.h>
#include <qnandefaultchartseries.h>

#include "curvedata.h"

CurveData::CurveData()
	: series(nullptr), curveID(-1)
{
}

void CurveData::init(QNANChartWidget * w) {
	Q_ASSERT(w != nullptr);
	series = new QNANDefaultChartSeries(w);
	curveID = w->addSeries(series);
}

