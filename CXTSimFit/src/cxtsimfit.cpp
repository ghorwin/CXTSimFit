#include <QtGui>
#include <iostream>
using namespace std;

#include "cxtsimfit.h"
#include "aboutdialog.h"
#include "inspectprofiledialog.h"

#include "qnandefaultchartseries.h"
#include "qnanchartaxis.h"

#include "solverresults.h"
#include "solver.h"

#include "optimizer.h"
#include "levmaroptimizer.h"

const char * PROGRAM_NAME = "CXT Sim-Fit";
const char * PROGRAM_VERSION = "1.0";

#define USE_LEVMAR

CXTSimFit::CXTSimFit(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags | Qt::WindowMinMaxButtonsHint | Qt::Dialog)
{
	ui.setupUi(this);

#ifdef Q_WS_WIN
	ui.pushButtonConfigQt->setVisible(false);
#endif

	setWindowTitle(QString("%1 %2").arg(PROGRAM_NAME).arg(PROGRAM_VERSION));

	ui.tabWidget->setCurrentIndex(0);

	// create data series for measured data
	inletCurve.init(ui.chart);
	outletCurve.init(ui.chart);

	ui.chart->setMarginRight(60);

	inletCurve.series->setColor(QColor( 45, 42, 204));
	inletCurve.series->setMarkerStyle(QNANDefaultChartSeries::Box);
	inletCurve.series->setMarkerSize(4);
	inletCurve.series->setSeriesType(QNANDefaultChartSeries::LineAndMarker);
	outletCurve.series->setColor(QColor(204, 42, 45));
	outletCurve.series->setMarkerStyle(QNANDefaultChartSeries::Box);
	outletCurve.series->setMarkerSize(4);
	outletCurve.series->setSeriesType(QNANDefaultChartSeries::LineAndMarker);

	// customize chart
	ui.chart->x1Axis()->setLabel(tr("Time [h]"));
	ui.chart->y1Axis()->setLabel(tr("Concentration [kg/m3]"));

	// setup the combo box
	ui.comboBoxModel->addItem(tr("Equilibrium Sorption (1 BE)") );
	ui.comboBoxModel->addItem(tr("+ mass transfer (2 BE)") );
	
	ui.lineEditOutletData->setText("outlet.txt");
	ui.lineEditInletData->setText("inlet.txt");
	loadDataFiles();

	ui.checkBoxInletC->setChecked(true);
	ui.lineEditInletC->setText("71");

	// set some default parameters
	double A = 1;
	ui.lineEditA->setText( QString("%1").arg(A) );
	ui.lineEditL->setText( "0.3");
	ui.lineEditq->setText( "0.1" );
	ui.lineEditp->setText( "0.2" );
	on_lineEditq_textChanged("");

	ui.lineEditD->setText("0");
	ui.lineEditRC->setText( "100000" );
	ui.lineEditMuC->setText( "0" );
	ui.lineEditGammaC->setText( "0" );

	ui.lineEditBeta->setText( "0" );
	ui.lineEditRS->setText( "100000" );
	ui.lineEditMuS->setText( "0" );
	ui.lineEditGammaS->setText( "0" );

	ui.lineEditTEnd->setText("24"); 
	ui.lineEditOutputDt->setText("1800");
	ui.spinBoxOutputN->setValue(2);
	ui.lineEditMaxDt->setText("600");
	ui.lineEditMinDt->setText("1e-12");
	ui.lineEditRelTol->setText("1e-5");
	ui.lineEditAbsTol->setText("1e-10");
	ui.lineEditDigits->setText("1e-10");

	connect(ui.lineEditp, SIGNAL(textChanged(const QString &)),
		this, SLOT(on_lineEditq_textChanged(const QString &)));
	connect(ui.lineEditA, SIGNAL(textChanged(const QString &)),
		this, SLOT(on_lineEditq_textChanged(const QString &)));
	on_lineEditq_textChanged(QString());
}

CXTSimFit::~CXTSimFit()
{
}

void CXTSimFit::solverRunCompleted(bool add_series, const SolverResults & res) {
	QString desc = QString("R2=%3").arg(res.R2);

	CurveData * c;
	if (add_series) {
		curves.append(CurveData());
		ui.listWidgetCurveInfo->addItem(desc);
		c = &curves.back();
		c->init(ui.chart);
	}
	else {
		// only add if curves list is empty
		if (curves.isEmpty()) {
			curves.append(CurveData());
			ui.listWidgetCurveInfo->addItem(desc);
			c = &curves.back();
			c->init(ui.chart);
		}
		else {
			QListWidgetItem * item = ui.listWidgetCurveInfo->item(ui.listWidgetCurveInfo->count()-1);
			item->setText(desc);
			// TODO : modify last item in list
			c = &curves.back();
		}
	}
	// use the last series
	QList<double> x,y;
	for (int i=0; i<res.data.m_x.size(); ++i) {
		x.append(res.data.m_x[i]);
		y.append(res.data.m_y[i]);
	}
	c->series->setData(x,y);
	ui.chart->updateChart();

	lastResults = res;
}

void CXTSimFit::loadDataFiles() {
	// tell series that we don't have any data momentarily
	outletCurve.series->clear();
	inletCurve.series->clear();

	// first outlet data
	QFile f(ui.lineEditOutletData->text());
	f.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream stream(&f);
	QString line;
	// read first section
	QList<double> x;
	QList<double> y;
	do {
		line = stream.readLine();
		if (line.isEmpty()) break;
		if (line[0] == '#') continue;
		QTextStream linestrm(&line);
		double t, val;
		linestrm >> t >> val;
		if (linestrm.status() != QTextStream::Ok) break;
		x.append(t);
		y.append(val);
	} while (!line.isNull());
	qDebug() << "Outlet data points = " << x.size();

	if (!x.isEmpty()) {
		outletCurve.series->setData(x,y);
		outletCurveSpline.clear();
		for (int i=0; i<x.count(); ++i) {
			outletCurveSpline.m_x.push_back(x[i]);
			outletCurveSpline.m_y.push_back(y[i]);
			try {
				outletCurveSpline.makeSpline();
			}
			catch (...) {
				outletCurveSpline.clear();
			}
		}
	}
	f.close();
	QFile f2(ui.lineEditInletData->text());
	f2.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream stream2(&f2);

	// read second section
	x.clear();
	y.clear();
	inletCurveSpline.clear();
	do {
		line = stream2.readLine();
		if (line.isEmpty()) break;
		if (line[0] == '#') continue;
		QTextStream linestrm(&line);
		double t, val;
		linestrm >> t >> val;
		if (linestrm.status() != QTextStream::Ok) break;
		x.append(t);
		y.append(val);
	} while (!line.isNull());
	qDebug() << "Inlet data points = " << x.size();

	if (!x.isEmpty()) {
		inletCurve.series->setData(x,y);

		for (int i=0; i<x.count(); ++i) {
			inletCurveSpline.m_x.push_back(x[i]);
			inletCurveSpline.m_y.push_back(y[i]);
			try {
				inletCurveSpline.makeSpline();
			}
			catch (...) {
				inletCurveSpline.clear();
			}
		}
//		ui.lineEditInletC->setText(QString("%1").arg(meanInletC));
		ui.checkBoxInletC->setChecked(false);
//		ui.lineEditInletC->setEnabled(false);
	}
	ui.chart->updateChart();
}

bool CXTSimFit::getInput(SolverInput & input, bool silent) {
	input.model = static_cast<SolverInput::model_t>(ui.comboBoxModel->currentIndex());
	// check for valid input
	bool ok;
	input.A = ui.lineEditA->text().toDouble(&ok);
	if (!ok || input.A < 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for cross section!"));
		return false;
	}
	input.L = ui.lineEditL->text().toDouble(&ok);
	if (!ok || input.L <= 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for domain length!"));
		return false;
	}

	input.q = ui.lineEditq->text().toDouble(&ok);
	if (!ok || input.q < 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for flow rate!"));
		return false;
	}
	input.p = ui.lineEditp->text().toDouble(&ok);
	if (!ok || input.p < 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for porosity!"));
		return false;
	}
	input.v = ui.lineEditv->text().toDouble(&ok);
	if (!ok || input.v <= 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for convection velocity!"));
		return false;
	}

	// equation 1 parameters
	input.D = ui.lineEditD->text().toDouble(&ok);
	if (!ok || input.D < 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for diffusion/dispersion coefficient!"));
		return false;
	}
	input.Rc = ui.lineEditRC->text().toDouble(&ok);
	if (!ok || input.Rc < 1) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for retention coefficient!"));
		return false;
	}
	input.muc = ui.lineEditMuC->text().toDouble(&ok);
	if (!ok) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for reaction rate coefficient!"));
		return false;
	}
	input.gammac = ui.lineEditGammaC->text().toDouble(&ok);
	if (!ok) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for mass production rate (source/sink)!"));
		return false;
	}

	// equation 2 parameters
	if (input.model == SolverInput::PLUS_EXCHANGE) {
		input.Rs = ui.lineEditRS->text().toDouble(&ok);
		if (!ok || input.Rs < 1) {
			if (!silent)
				QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for retention coefficient!"));
			return false;
		}
		input.mus = ui.lineEditMuS->text().toDouble(&ok);
		if (!ok) {
			if (!silent)
				QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for reaction rate coefficient!"));
			return false;
		}
		input.gammas = ui.lineEditGammaS->text().toDouble(&ok);
		if (!ok) {
			if (!silent)
				QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for mass production rate (source/sink)!"));
			return false;
		}
		input.beta = ui.lineEditBeta->text().toDouble(&ok);
		if (!ok || input.beta < 0) {
			if (!silent)
				QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for mass transfer coefficient!"));
			return false;
		}
	}

	if (ui.checkBoxInletC->isChecked()) {
		input.cInlet = ui.lineEditInletC->text().toDouble(&ok);
		if (!ok || input.cInlet < 0) {
			if (!silent)
				QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for inlet concentration!"));
			return false;
		}
		input.cInletData.clear();
	}
	else {
		if (inletCurveSpline.empty()) {
			if (!silent)
				QMessageBox::information(this, PROGRAM_NAME, tr("Need inlet concentration data unless constant inlet concentration is used!"));
			return false;
		}
		input.cInlet = 0;
		input.cInletData = inletCurveSpline;
	}
	input.tEnd = ui.lineEditTEnd->text().toDouble(&ok);
	if (!ok || input.tEnd< 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for simulation time!"));
		return false;
	}
	input.outputDt = ui.lineEditOutputDt->text().toDouble(&ok);
	if (!ok || input.outputDt <= 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for output time step!"));
		return false;
	}
	input.outputN = ui.spinBoxOutputN->value();
	input.maxDt = ui.lineEditMaxDt->text().toDouble(&ok);
	if (!ok || input.maxDt <= 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for maximum time step!"));
		return false;
	}
	input.minDt = ui.lineEditMinDt->text().toDouble(&ok);
	if (!ok || input.minDt <= 0 || input.minDt > input.maxDt) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for minimum time step!"));
		return false;
	}
	input.relTol = ui.lineEditRelTol->text().toDouble(&ok);
	if (!ok || input.relTol <= 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for relative tolerance!"));
		return false;
	}
	input.absTol = ui.lineEditAbsTol->text().toDouble(&ok);
	if (!ok || input.absTol <= 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for absolute tolerance!"));
		return false;
	}
	input.digits = ui.lineEditDigits->text().toDouble(&ok);
	if (!ok || input.digits <= 0) {
		if (!silent)
			QMessageBox::information(this, PROGRAM_NAME, tr("Invalid input for optimizer tolerance!"));
		return false;
	}

	input.n = ui.spinBoxBedElements->value();
	return true;
}

void CXTSimFit::calculatePartitionCoefficient() {
/*	SolverInput input;
	if (!getInput(input, true)) return;

	// if we have inlet and outlet curve spline, calculate partition coefficient
	if (!inletCurveSpline.valid() || !outletCurveSpline.valid()) {
		qDebug() << "Need valid spline data for inlet and outlet curves!";
		return;
	}
	//input.calculatePartitionCoefficient();
/*	double t = 0;
		double max_t = std::min(inletCurveSpline.m_x.back(), outletCurveSpline.m_x.back());
		// integrate the difference 
		int N = 10000;
		double dt = max_t/N;
		double cA = 0;
		for (int i=0; i<N; ++i) {
			t += dt;
			double c_in = inletCurveSpline.value(t);
			double c_out = outletCurveSpline.value(t);
			cA += dt*(c_in - c_out);
		}
		cA *= 3600; // convert to ? s/m3, where ? is the concentration unit
		cout << "Normalized area between inlet and outlet concentration from t=0 to " << max_t << " h = " << cA << " in ? s/m3" << endl;
		double q = ui.lineEditFlowRate->text().toDouble(); // in m3/s
		double A = ui.lineEditArea->text().toDouble();
		double eps = ui.lineEditPorosity->text().toDouble();
		double delta_m = q*cA;
		double d = ui.lineEditThickness->text().toDouble();
		double V_solid = d*A*(1-eps);
		double c_ad = delta_m/V_solid; // divide by solid volume
		double K = c_ad/meanInletC;
		cout << "Partition coefficient K = " << K << endl;
	}
*/
}

void CXTSimFit::updateCurve(bool add_series) {
	SolverInput input;
	if (!getInput(input, false)) return;
	// create solver object
	Solver solv;
	try {
		input.tEnd = input.tEnd * 3600;
		solv.init(input);
	}
	catch (std::exception& ex) {
		QMessageBox::critical(this, PROGRAM_NAME, QString::fromLatin1(ex.what()) );
		return;
	}

	try {
		std::cout << "Solver started...";
		solv.run();
		qDebug() << "Done.";
	}
	catch (std::exception& ex) {
		QMessageBox::critical(this, PROGRAM_NAME, QString::fromLatin1(ex.what()) );
		return;
	}

	// store results
	SolverResults res;
	res.input = solv.input();
	res.ccProfiles = solv.ccProfile();
	res.scProfiles = solv.scProfile();
	res.tProfiles = solv.tProfile();
	res.data.m_x = solv.m_outletT;
	res.data.m_y = solv.m_outletC;
	res.calculateRSquare(outletCurveSpline);
	solverRunCompleted(add_series, res);
}

void CXTSimFit::on_pushButtonQuit_clicked() {
	close();
}

void CXTSimFit::on_pushButtonBrowseInletData_clicked() {
	QString datafile = QFileDialog::getOpenFileName(this, tr("Select inlet data file"), QString(), tr("All files (*.*)"));
	if (!datafile.isEmpty()) {
		ui.lineEditInletData->setText(datafile);
		loadDataFiles();
	}
}

void CXTSimFit::on_pushButtonBrowseOutletData_clicked() {
	QString datafile = QFileDialog::getOpenFileName(this, tr("Select outlet data file"), QString(), tr("All files (*.*)"));
	if (!datafile.isEmpty()) {
		ui.lineEditOutletData->setText(datafile);
		loadDataFiles();
	}
}

void CXTSimFit::on_lineEditq_textChanged(const QString & text) {
	bool ok;
	double flowrate = ui.lineEditq->text().toDouble(&ok);
	if (!ok) return;

	double porosity = ui.lineEditp->text().toDouble(&ok);
	if (!ok) return;
	if (porosity <= 0) return;

	double area = ui.lineEditA->text().toDouble(&ok);
	if (!ok) return;
	if (area <= 0) return;

	double vel = flowrate/area/porosity;

	ui.lineEditv->setText( QString("%1").arg(vel) );
}

void CXTSimFit::on_pushButtonUpdateFit_clicked() {
	updateCurve(false);
}

void CXTSimFit::on_pushButtonAddFit_clicked() {
	updateCurve(true);
}

void CXTSimFit::on_pushButtonOptimize_clicked() {
	SolverInput input;
	if (!getInput(input, false)) return;

	qDebug() << "Minimizing deviation of simulated and measured break-through curve:";
	std::vector<LevMarOptimizer::optimizable_parameter_t>	optimizableParams;
	std::vector<double> par;

	// now add all parameters to the set where the checkboxes are checked
	if (ui.checkboxp->isEnabled() && ui.checkboxp->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_p);
		par.push_back(input.p);
	}
	if (ui.checkboxD->isEnabled() && ui.checkboxD->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_D);
		par.push_back(input.D);
	}
	if (ui.checkboxR->isEnabled() && ui.checkboxR->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_R_c);
		par.push_back(input.Rc);
	}
	if (ui.checkboxMuC->isEnabled() && ui.checkboxMuC->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_mu_c);
		par.push_back(input.muc);
	}
	if (ui.checkboxGammaC->isEnabled() && ui.checkboxGammaC->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_gamma_c);
		par.push_back(input.gammac);
	}

	if (ui.checkboxRS->isEnabled() && ui.checkboxRS->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_R_s);
		par.push_back(input.Rs);
	}
	if (ui.checkboxMuS->isEnabled() && ui.checkboxMuS->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_mu_s);
		par.push_back(input.mus);
	}
	if (ui.checkboxGammaS->isEnabled() && ui.checkboxGammaS->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_gamma_s);
		par.push_back(input.gammas);
	}
	if (ui.checkboxBeta->isEnabled() && ui.checkboxBeta->isChecked()) {
		optimizableParams.push_back(LevMarOptimizer::PAR_beta);
		par.push_back(input.beta);
	}

	if (optimizableParams.empty()) {
		QMessageBox::critical(this, tr("Missing fit selection"), tr("Please check at least one parameter to be fitted!"));
		return;
	}

#ifndef USE_LEVMAR
	// TODO : open dialog where user can select optimization parameters

	// create our function to be minimized (actually R2 of the curve fit)
	Optimizer f(this, &input);

	IBK::BrentMinimization minimizer(1e-6);
	try {
		minimizer.bracketMinimum(f,par[0], par[0]+0.0001);
	}
	catch (std::exception & ex) {
		qDebug() << ex.what();
		return;
	}
	qDebug() << "f(" << minimizer.ax << ") = " << minimizer.fa << "   >   "
		     << "f(" << minimizer.bx << ") = " << minimizer.fb << "   <   "
		     << "f(" << minimizer.cx << ") = " << minimizer.fc;
	try {
		par[0] = minimizer.minimize(f);
		qDebug() << "R^2(" << minx << ") = " << minimizer.fmin << " after " 
				<< (minimizer.max_iters - minimizer.iterations) << " Iterations, (" 
				<< f.m_evals << " function evals).";
	}
	catch (std::exception & ex) {
		qDebug() << ex.what();
		return;
	}
#else // USE_LEVMAR

	LevMarOptimizer f(input, outletCurveSpline.m_x, outletCurveSpline.m_y);
	f.optimizablePars = optimizableParams;
	try {
		f.optimize(par);
	}
	catch (std::exception & ex) {
		qDebug() << ex.what();
		return;
	}

#endif // USE_LEVMAR

	QString allVals;
	for (size_t i=0; i<optimizableParams.size(); ++i) {
		switch (optimizableParams[i]) {
			case LevMarOptimizer::PAR_D :
				allVals += tr("Diffusion coefficient [m2/s] = %1 \n").arg(par[i]);
				ui.lineEditD->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_p :
				allVals += tr("Porosity [m3/m3] = %1 \n").arg(par[i]);
				ui.lineEditp->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_R_c :
				allVals += tr("Retention coefficient [kg(ad)/kg(gas)] = %1 \n").arg(par[i]);
				ui.lineEditRC->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_mu_c :
				allVals += tr("Reaction rate [1/s] = %1 \n").arg(par[i]);
				ui.lineEditMuC->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_gamma_c :
				allVals += tr("Source/Sink [kg/m3s] = %1 \n").arg(par[i]);
				ui.lineEditGammaC->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_R_s :
				allVals += tr("Retention coefficient [kg(ad)/kg(gas)] = %1 \n").arg(par[i]);
				ui.lineEditRS->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_mu_s :
				allVals += tr("Reaction rate [1/s] = %1 \n").arg(par[i]);
				ui.lineEditMuS->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_gamma_s :
				allVals += tr("Source/Sink [kg/m3s] = %1 \n").arg(par[i]);
				ui.lineEditGammaS->setText(QString("%1").arg(par[i]));
				break;
			case LevMarOptimizer::PAR_beta :
				allVals += tr("Mass transfer coefficient [1/s] = %1 \n").arg(par[i]);
				ui.lineEditBeta->setText(QString("%1").arg(par[i]));
				break;
		}
	}
//	QMessageBox::information(this, tr("Fit successful"), tr("The following values have been fitted:\n") + allVals);

	on_pushButtonUpdateFit_clicked();
}


void CXTSimFit::on_comboBoxModel_currentIndexChanged(int index) {
	// disable the optional inputs
	ui.lineEditRS->setEnabled(false);
	ui.checkboxRS->setEnabled(false);
	ui.lineEditMuS->setEnabled(false);
	ui.checkboxMuS->setEnabled(false);
	ui.lineEditGammaS->setEnabled(false);
	ui.checkboxGammaS->setEnabled(false);
	ui.lineEditBeta->setEnabled(false);
	ui.checkboxBeta->setEnabled(false);
	ui.labelParameters2->setEnabled(false);
	switch (ui.comboBoxModel->currentIndex()) {
		case 1 :
			ui.lineEditRS->setEnabled(true);
			ui.checkboxRS->setEnabled(true);
			ui.lineEditMuS->setEnabled(true);
			ui.checkboxMuS->setEnabled(true);
			ui.lineEditGammaS->setEnabled(true);
			ui.checkboxGammaS->setEnabled(true);
			ui.lineEditBeta->setEnabled(true);
			ui.checkboxBeta->setEnabled(true);
			ui.labelParameters2->setEnabled(true);
			// special task when switching the first time to second model
			if (ui.lineEditBeta->text() == "0") {
				ui.lineEditBeta->setText( "0.1" );
				ui.lineEditRS->setText( ui.lineEditRC->text() );
				ui.lineEditRC->setText("1");
			}
			break;
	}
}

void CXTSimFit::on_checkBoxInletC_toggled(bool) {
	if (ui.checkBoxInletC->isChecked()) {
		ui.lineEditInletC->setEnabled(true);
		ui.label_3->setEnabled(false);
		ui.lineEditInletData->setEnabled(false);
		ui.pushButtonBrowseInletData->setEnabled(false);
	}
	else {
		ui.lineEditInletC->setEnabled(false);
		ui.label_3->setEnabled(true);
		ui.lineEditInletData->setEnabled(true);
		ui.pushButtonBrowseInletData->setEnabled(true);
	}
}


void CXTSimFit::on_pushButtonAbout_clicked() {
	AboutDialog(this).exec();
}

void CXTSimFit::on_pushButtonConfigQt_clicked() {
	// open external program 'qtconfig'
	QString program = "qtconfig";
	QStringList arguments;

	QProcess *myProcess = new QProcess(this);
	myProcess->start(program, arguments);
}


void CXTSimFit::on_pushButtonProfiles_clicked() {
	if (lastResults.data.empty()) {
		QMessageBox::critical(this, PROGRAM_NAME, tr("A valid simulation run is needed before you can expect profiles. Please update the curve first and fix any error messages you may get!"));
		return;
	}
	InspectProfileDialog dlg(this, lastResults);
	dlg.exec();
}
