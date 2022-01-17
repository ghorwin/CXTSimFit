#ifndef INSPECTPROFILEDIALOG_H
#define INSPECTPROFILEDIALOG_H

#include <QDialog>
#include "ui_inspectprofiledialog.h"

#include <vector>

#include "solverresults.h"
#include "curvedata.h"

/// This dialog shows calculated profiles.
class InspectProfileDialog : public QDialog {
	Q_OBJECT

public:
	InspectProfileDialog(QWidget *parent, const SolverResults & res);
	~InspectProfileDialog();

private:
	Ui::InspectProfileDialog ui;

	SolverResults results;

	CurveData				ccCurve;	///< Data for the mobile concentration.
	CurveData				scCurve;	///< Data for the mobile concentration.

	std::vector<double>		x;			///< The x-coordinates in [m].

private slots:
	void on_horizontalSlider_valueChanged(int);
	void on_pushButtonClose_clicked();
};

#endif // INSPECTPROFILEDIALOG_H
