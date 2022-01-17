#include <QtGui>
#include <qnanchartwidget.h>
#include <qnandefaultchartseries.h>

#include "curvedata.h"

CurveData::CurveData() 
	: series(NULL), curveID(-1)
{
}

void CurveData::init(QNANChartWidget * w) {
	Q_ASSERT(w != 0);
	series = new QNANDefaultChartSeries(w);
	curveID = w->addSeries(series);
}

