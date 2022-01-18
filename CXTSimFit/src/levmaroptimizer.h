#ifndef levmaroptimizer_h
#define levmaroptimizer_h

#include <vector>

#include <levmar.h>

class SolverInput;

/// This class nicely wraps the call to the levmar library and the simulation solver.
class LevMarOptimizer {
public:
	enum optimizable_parameter_t {
		PAR_p,
		PAR_D,
		PAR_R_c,
		PAR_mu_c,
		PAR_gamma_c,
		PAR_R_s,
		PAR_mu_s,
		PAR_gamma_s,
		PAR_beta
	};

	/// Constructor, takes all properties required for the simulation later
	/// as arguments.
	/// @param input
	LevMarOptimizer(const SolverInput & input,
		const std::vector<double> & t,
		const std::vector<double> & c_out);

	/// The main optimization function.
	/// Call this function to optimize the parameters passed in the parameters vector.
	/// Once the function returns the parameters vector contains the optimized parameters,
	/// or the original parameters, if the optimization failed.
	void optimize(std::vector<double> & parameters);

	/// The main calculation function.
	/// Simulates the break-through curve using the parameters in p
	/// and calculates solutions at points x.
	/// @param p Contains the parameters adjusted by LevMar.
	/// @param c Vector with calculated outlet concentrations.
	void calculate(double * p, double * c);

	const SolverInput * m_input;	///< Pointer to original solver input data (the physical constants).

	unsigned int max_iters;
	double opts[LM_OPTS_SZ];
	double info[LM_INFO_SZ];

	std::vector<optimizable_parameter_t> optimizablePars;

private:
	std::vector<double>		m_p;	///< Contains the parameters to be optimized.
	std::vector<double>		m_c;	///< Contains the outlet concentrations.
	std::vector<double>		m_t;	///< Contains the time points of the measured outlet concentrations.
};

/// Function that get's passed to the levmar library.
/// It redirects the call to LevMarOptimizer::calculate().
/// @param p Vector with m parameters
/// @param c Vector with c measurement positions (fitting)
/// @param m Number of parameters.
/// @param n Number of measurement positions.
/// @param data Pointer to an instance of LevMarOptimizer.
void solver_fit(double *p, double *c, int m, int n, void *data);

#endif // levmaroptimizer_h
