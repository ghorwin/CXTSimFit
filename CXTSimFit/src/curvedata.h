#ifndef curvedata_h
#define curvedata_h

class QNANChartWidget;
class QNANDefaultChartSeries;

class CurveData {
public:
	CurveData();
	void init(QNANChartWidget * w);
	
	QNANDefaultChartSeries * series;
	int curveID;
};

#endif // curvedata_h
