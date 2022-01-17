#include <stdexcept>
#include <iostream>
#include <cmath>
using namespace std;

#include <QDebug>

#include "optimizer.h"

#include "filterfit.h"
#include "curvedata.h"
#include "solverinput.h"
#include "solverresults.h"
#include "solver.h"

Optimizer::Optimizer(FilterFit * parent, SolverInput * input)
  : m_parent(parent), m_input(input), m_evals(0)
{
	// TODO : transfer other options
}

double Optimizer::operator()(double x) {
	++m_evals;
	if (x < 0)
		return 10000*fabs(x) + 10000;
	// for now, x is the diffusion coefficient

	m_input->D = x;

	Solver solv;
	try {
		std::cout << "Diffusion coefficient = " << x << "\t ";
		m_input->model = SolverInput::DIFF_CONV_PARTITION;
		m_input->tEnd = 30*3600;
		solv.init(*m_input);
	}
	catch (std::exception& ex) {
		qDebug() << "Error initializing the solver: "<< QString::fromLatin1(ex.what()) << endl;
		throw std::runtime_error("Can't continiue minimization!");
	}

	try {
		solv.run();
		std::cout << std::endl; 
	}
	catch (std::exception& ex) {
		qDebug() << "Error running the solver: "<< QString::fromLatin1(ex.what()) << endl;
		throw std::runtime_error("Can't continiue minimization!");
	}

	// store results
	SolverResults res;
	// then we compute and return the R2 value
	res.input = *m_input;
	res.data.m_x = solv.m_outletT;
	res.data.m_y = solv.m_outletC;
	res.calculateRSquare(m_parent->outletCurveSpline);
	
	return res.R2;
}
