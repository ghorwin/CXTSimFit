// Basic Framework for a CVODE based solver for stiff systems of ODE
// Copyright 2008 Andreas Nicolai <andreas.nicolai@tu-dresden.de>
// This code is released under the GNU Public License.

#ifndef solver_h
#define solver_h

// Standard C++ Headers
#include <string>
#include <iosfwd>
#include <vector>

// includes of the sundials library
#include <sundials/sundials_types.h>
#include <nvector/nvector_serial.h>
#include <cvode/cvode.h>

#include "solverinput.h"

/// Example implementation for a CVODE based solver.
class Solver {
public:
	/// Constructor.
	Solver();
	/// Destructor (to clean up allocated memory)
	~Solver();
	/// Clears all allocated memory and resets pointers.
	void clear();
	/// Initialization function.
	void init(const SolverInput & input);
	/// Starts the solver
	void run();

	/// System function called by the solver.
	/// This function is used to calculate the divergences (right-hand-sides)
	/// of the differential equations. Implement all the physics in this equation.
	int calculateDivergences(double t, N_Vector y, N_Vector ydot);

	std::vector<double>		m_outletT;	///< Vector with time points of outlet data in [s]
	std::vector<double>		m_outletC;	///< Vector with concentrations at outlet in [kg/m3s]

	/// Returns the input data object, that containts all input data for the solver.
	const SolverInput &		input() const { return m_input; }

	const std::vector<std::vector<double> > & ccProfile() const { return m_ccProfile; }
	const std::vector<std::vector<double> > & scProfile() const { return m_scProfile; }
	const std::vector<double> &				  tProfile() const { return m_tProfile; }

private:
	/// Stores output data.
	void storeOutput();

	bool					m_initialized;	///< This variable is set to true, once the solver is successfully initialized

	SolverInput				m_input;		///< Containts all input data for the solver.

	double					m_t;			///< Current time point in s.
	double					m_tEnd;			///< Last time point of simulation in s.

	unsigned int			m_outputCounter;	///< Number of break-through outputs done in s.
	IBK::LinearSpline		m_cInletData;		///< Inlet concentration in kg/m3

	unsigned int			m_n;			///< Number of elements.
	unsigned int			m_nVars;		///< Number of variables per element.

	std::vector<double>		m_cREV;			///< Vector with total mass densities per element in kg/m3
	std::vector<double>		m_cc;			///< Vector with gas/mobile phase mass densities in kg/m3
	std::vector<double>		m_sREV;			///< Vector with total mass densities per element in kg/m3
	std::vector<double>		m_sc;			///< Vector with gas/mobile phase mass densities in kg/m3

	std::vector<double>		m_jdiff;		///< Vector with axial diffusive fluxes in kg/s (n+1)
	std::vector<double>		m_jconv;		///< Vector with convective fluxes in kg/s (n+1)

	std::vector<double>		m_sbeta;		///< Vector with exchange fluxes in kg/s (n) (negative for eq 1, pos. for eq 2)
	std::vector<double>		m_smu_c;		///< Vector with chemical reaction fluxes in kg/s (n)
	std::vector<double>		m_sgamma_c;		///< Vector with sources/sinks in kg/s (n)
	std::vector<double>		m_smu_s;		///< Vector with chemical reaction fluxes in kg/s (n)
	std::vector<double>		m_sgamma_s;		///< Vector with sources/sinks in kg/s (n)

	std::vector<std::vector<double> >	m_ccProfile;	///< Gas/mobile phase concentration kg/m3
	std::vector<std::vector<double> >	m_scProfile;	///< Gas/mobile phase concentration kg/m3
	std::vector<double>					m_tProfile;		///< Time point for output.

	// *** CVODE Variables ***

	/// Vector for state variables, used in the CVODE solver
	/// Do not retrieve state variables from this vector within f(),
	/// CVODE changes memory pointer frequently!!!!
	N_Vector		m_yStorage;
	/// Vector for absolute tolerances, only needed during initialization.
	N_Vector		m_absTolVec;
	/// Relative tolerance.
	double			m_relTol;
	/// CVODE memory pointer.
	void			*m_cvodeMem;
	/// File handle for CVODE monitor variables.
	std::ofstream	*m_cvodeMonitors;
};


#endif //  solver_h
