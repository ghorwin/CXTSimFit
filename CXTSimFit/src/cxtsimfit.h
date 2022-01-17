#ifndef CXTSIMFIT_H
#define CXTSIMFIT_H

#include <QtGui/QDialog>
#include <QList>

#include "ui_cxtsimfit.h"

#include "curvedata.h"
#include "solverresults.h"

#include "IBK_linearspline.h"

class QNANDefaultChartSeries;
class SolverInput;

class CXTSimFit : public QDialog {
	Q_OBJECT
public:
	CXTSimFit(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CXTSimFit();

	IBK::LinearSpline		outletCurveSpline;
	IBK::LinearSpline		inletCurveSpline;

public slots:
	void solverRunCompleted(bool add_series, const SolverResults & res);

private:
	void loadDataFiles();

	/// Grabs the input data from the widgets and stores it in 'input'.
	/// If 'silent' is false, the function pops up error messages in case of invalid input.
	/// if 'silent' is true, the function just returns with false on error.
	/// @return Returns true, if all input was correctly transferred into 'input'.
	bool getInput(SolverInput & input, bool silent);

	/// Runs the solver directly (in the GUI thread).
	/// If 'add_series' is true, a new series gets added with the results, otherwise the current series is updated.
	void updateCurve(bool add_series);

	/// Calculates with given parameters and adds a new curve to the list of curves.
	void runSolver(const SolverInput & input, bool add_series);

	/// Calculates the partition coefficient from measured data and input data.
	void calculatePartitionCoefficient();

	Ui::CXTSimFit ui;

	CurveData				inletCurve;
	CurveData				outletCurve;

	double					meanInletC;

	QList<CurveData>		curves;

	SolverResults			lastResults;	///< Caches results from the last solver run.

private slots:
	void on_pushButtonProfiles_clicked();
	void on_pushButtonConfigQt_clicked();
	void on_pushButtonAbout_clicked();
	void on_checkBoxInletC_toggled(bool);
	void on_pushButtonBrowseOutletData_clicked();
	void on_pushButtonBrowseInletData_clicked();
	void on_comboBoxModel_currentIndexChanged(int index);
	void on_pushButtonOptimize_clicked();
	void on_pushButtonAddFit_clicked();
	void on_pushButtonUpdateFit_clicked();
	void on_pushButtonQuit_clicked();
	void on_lineEditq_textChanged(const QString & text);
};

#endif // CXTSIMFIT_H
