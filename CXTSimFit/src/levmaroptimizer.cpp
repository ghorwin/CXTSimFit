#include <iostream>
#include <iomanip>
#include <stdexcept>

#include <levmaroptimizer.h>

#include <IBK_LinearSpline.h>
#include <IBK_Exception.h>

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
	: m_input(&input), max_iters(1000), m_c(c_out), m_t(t)
{
}


void LevMarOptimizer::optimize(std::vector<double> & parameters) {
//	FUNCID(LevMarOptimizer::optimize);
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
		std::cout.setf( std::ios_base::floatfield );
		std::cout << "Levenberg-Marquardt returned after " << ret << " iterations." << std::endl;
		std::cout << "    " << info[7] << " function evaluations (solver runs)" << std::endl;
		std::cout << "    " << info[8] << " Jacobian evaluations" << std::endl;
		std::cout << "Reason for terminating:";
		switch ((int)(info[6])) {
			case 1 : std::cout << "   Stopped by small gradient J^T e"; break;
			case 2 : std::cout << "   Stopped by small Dp"; break;
			case 3 : std::cout << "   Stopped by itmax"; break;
			case 4 : std::cout << "   Singular matrix. Restart from current p with increased mu ";break;
			case 5 : std::cout << "   No further error reduction is possible. Restart with increased mu"; break;
			case 6 : std::cout << "   Stopped by small ||e||_2"; break;
			case 7 : std::cout << "   Stopped by invalid (i.e. NaN or Inf) 'func' values. This is a user error."; break;
		}
		std::cout << std::endl;

		// store the optimized parameters
		parameters = m_p;
	}
}

void LevMarOptimizer::calculate(double * p, double * c) {
	FUNCID(LevMarOptimizer::calculate);

	SolverInput input = *m_input;

	double penalty = 0;
	std::cout << std::scientific << std::setprecision(14);
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
		std::cout << "Error initializing the solver: "<< ex.what() << std::endl;
		throw std::runtime_error("Can't continiue minimization!");
	}

	try {
		solv.run();
		std::cout << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << "Error running the solver: "<< ex.what() << std::endl;
		throw std::runtime_error("Can't continue minimization!");
	}

	// compute concentrations at measurment locations
	IBK::LinearSpline spl;
	try {
		spl.setValues(solv.m_outletT, solv.m_outletC);
	} catch (IBK::Exception & ex) {
		throw IBK::Exception(ex, "Error creating linear spline.", FUNC_ID);

	}

	// we need to interpolate the results at the measurement locations
	for (unsigned int i=0; i<m_t.size(); ++i) {
		double t = m_t[i];
		double val = spl.value(t);
		c[i] = val + penalty*penalty*1e6;
	}
}

