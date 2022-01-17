#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;

#include "levmaroptimizer.h"

#include "IBK_linearspline.h"

#include "solverinput.h"
#include "solver.h"

/// Function that get's passed to the levmar library
void solver_fit(double *p, double *x, int m, int n, void *data) {
	// relay the call to our member function
	reinterpret_cast<LevMarOptimizer*>(data)->calculate(p, x);
}

LevMarOptimizer::LevMarOptimizer(const SolverInput & input, 
								 const std::vector<double> & t, 
								 const std::vector<double> & c_out)
	: m_input(&input), m_t(t), m_c(c_out), max_iters(1000)
{
}

void LevMarOptimizer::optimize(std::vector<double> & parameters) {
	m_p = parameters;

	// set options 
	// opts = [\mu, \epsilon1, \epsilon2, \epsilon3]. 
	// Respectively the scale factor for initial \mu, 
	// stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2
	opts[0]=LM_INIT_MU; 
	opts[1]=1E-15; 
	opts[2]=m_input->digits; // how many digits accuracy?
	opts[3]=1E-20;
	opts[4]=LM_DIFF_DELTA; // for the finite difference Jacobian

	// use implementation without Jacobian
	int ret = dlevmar_dif(
		solver_fit, /* Function pointer to minimization function */
		&m_p[0],	/* Pointer to memory array with parameters to be optimized */
		&m_c[0],	/* Pointer to memory array with measurement locations */
		(int)m_p.size(),	/* Number of parameters */
		(int)m_c.size(),	/* Number of measurement locations */
		max_iters,	/* Number of iterations */
		opts,		/* Options for the solver */
		info,		/* Contains information about convergence once done */
		NULL,		/* Pointer to work array (NULL means it is automatically allocated within the function) */
		NULL,		/* Pointer to covariance matrix, NULL if unused */
		this);		/* Pointer that gets passed to the optimization function */
	if (ret == LM_ERROR) {
		throw std::runtime_error("Levenberg-Marquardt returned with an error. Optimization failed.");
	}
	else {
		cout.setf( ios_base::floatfield );
		cout << "Levenberg-Marquardt returned after " << ret << " iterations." << endl;
		cout << "    " << info[7] << " function evaluations (solver runs)" << endl;
		cout << "    " << info[8] << " Jacobian evaluations" << endl;
		cout << "Reason for terminating:";
		switch ((int)(info[6])) { 
			case 1 : cout << "   Stopped by small gradient J^T e"; break;
			case 2 : cout << "   Stopped by small Dp"; break;
			case 3 : cout << "   Stopped by itmax"; break;
			case 4 : cout << "   Singular matrix. Restart from current p with increased mu ";break;
			case 5 : cout << "   No further error reduction is possible. Restart with increased mu"; break;
			case 6 : cout << "   Stopped by small ||e||_2"; break;
			case 7 : cout << "   Stopped by invalid (i.e. NaN or Inf) 'func' values. This is a user error."; break;
		}
		cout << endl;

		// store the optimized parameters
		parameters = m_p;
	}
}

void LevMarOptimizer::calculate(double * p, double * c) {
	SolverInput input = *m_input;

	double penalty = 0;
	cout << scientific << setprecision(14);
	for (size_t i=0; i<optimizablePars.size(); ++i) {
		switch (optimizablePars[i]) {
			case LevMarOptimizer::PAR_p :
				penalty += std::max(0.0, 1e-10-p[i]);
				input.p = std::max(1e-10, p[i]);
				std::cout << "Porosity = " << input.p << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_D :
				penalty += std::max(0.0, 1e-14-p[i]);
				input.D = std::max(1e-14, p[i]);
				std::cout << "Diffusion coefficient = " << input.D << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_R_c :
				penalty += std::max(0.0, 1.0-p[i]);
				input.Rc = std::max(1.0, p[i]);
				std::cout << "Retention coefficient R_c = " << input.Rc << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_mu_c :
				input.muc = p[i];
				std::cout << "Reaction coeff. mu_c = " << input.muc << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_gamma_c :
				input.gammac = p[i];
				std::cout << "Source/sink gamma_c = " << input.gammac << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_R_s :
				penalty += std::max(0.0, 1.0-p[i]);
				input.Rs = std::max(1.0, p[i]);
				std::cout << "Retention coefficient R_s = " << input.Rs << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_mu_s :
				input.mus = p[i];
				std::cout << "Reaction coeff. mu_s = " << input.mus << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_gamma_s :
				input.gammas = p[i];
				std::cout << "Source/sink gamma_s = " << input.gammas << "{" << p[i] << "}" << "\n";
				break;
			case LevMarOptimizer::PAR_beta :
				input.beta = p[i];
				std::cout << "Mass tranfer coefficient beta = " << input.beta << "{" << p[i] << "}" << "\n";
				break;
		}
	}

	input.tEnd = 30*3600;

	Solver solv;
	try {
		solv.init(input);
	}
	catch (std::exception& ex) {
		cout << "Error initializing the solver: "<< ex.what() << endl;
		throw std::runtime_error("Can't continiue minimization!");
	}

	try {
		solv.run();
		std::cout << std::endl; 
	}
	catch (std::exception& ex) {
		cout << "Error running the solver: "<< ex.what() << endl;
		throw std::runtime_error("Can't continiue minimization!");
	}

	// compute concentrations at measurment locations
	IBK::LinearSpline spl;
	spl.m_x = solv.m_outletT;
	spl.m_y = solv.m_outletC;
	spl.makeSpline();

	// we need to interpolate the results at the measurement locations
	for (unsigned int i=0; i<m_t.size(); ++i) {
		double t = m_t[i];
		double val = spl.value(t);
		c[i] = val + penalty*penalty*1e6;
	}
}
