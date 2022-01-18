// Basic Framework for a CVODE based solver for stiff systems of ODE
// Copyright 2008 Andreas Nicolai <andreas.nicolai@tu-dresden.de>
// This code is released under the GNU Public License.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <cmath>

#include <cvode/cvode_band.h>

#include <IBK_Exception.h>

#include "solver.h"

// Wrapper function called from CVode solver which calls the
// actual solver routine in the solver class.
inline int f_solver(realtype t, N_Vector y, N_Vector ydot, void *f_data) {
	// relay call to solver member function
	return static_cast<Solver*>(f_data)->calculateDivergences(t, y, ydot);
}


Solver::Solver() {
	// initialize pointers to zero
//	m_outputFile = nullptr;
	m_yStorage = nullptr;
	m_absTolVec = nullptr;
	m_cvodeMem = nullptr;
	m_cvodeMonitors = nullptr;
}


Solver::~Solver() {
	clear();
}


void Solver::clear() {
	// free any allocated memory
	if (m_cvodeMem!=nullptr) {
		CVodeFree(&m_cvodeMem);
		m_cvodeMem = nullptr;
	}
	if (m_yStorage!=nullptr) {
		N_VDestroy_Serial(m_yStorage);
		m_yStorage = nullptr;
	}
	if (m_absTolVec!=nullptr) {
		N_VDestroy_Serial(m_absTolVec);
		m_absTolVec = nullptr;
	}
	delete m_cvodeMonitors;
	m_cvodeMonitors = nullptr;
}


void Solver::init(const SolverInput & input) {
	FUNCID(Solver::init);
	m_input = input;
	m_initialized = false;
	clear();

	// set simulation start and end time
	m_t = 0;
	m_tEnd = m_input.tEnd; // in seconds

	// set number of elements and variables per element
	m_n = m_input.n;

	// depending on problem to solve, set number of variables and determine the bandwidth
	unsigned int bandwidth;
	// bandwidth is the maximum difference of neighboring element numbers * 2 + 1
	switch (m_input.model) {
		case SolverInput::DIFF_CONV_PARTITION :
			m_nVars = 1; // total VOC mass density per bed node
			break;

		case SolverInput::PLUS_EXCHANGE :
			m_nVars = 2; // gaseous VOC mass density and adsorbed VOC mass density per bed node
			break;
	}

	bandwidth = (m_nVars+1)*2 - 1;

	// set relative and absolute tolerances
	m_relTol = input.relTol;
	m_absTolVec = N_VNew_Serial(m_nVars*m_n);
	if (!m_absTolVec)
		throw IBK::Exception("Absolute tolerances vector allocation error!", FUNC_ID);

	// now loop over all elements and set absolute tolerances
	for (unsigned int i=0; i<m_n*m_nVars; ++i) {
		NV_DATA_S(m_absTolVec)[i] = input.absTol;
	}

	// create solution vector and set initial conditions
	m_yStorage = N_VNew_Serial(m_n*m_nVars);
	if (!m_yStorage)
		throw IBK::Exception("Solution vector allocation error!", FUNC_ID);

	// for now, the initial condition is completely empty
	for (unsigned int i=0; i<m_n*m_nVars; ++i) {
		NV_DATA_S(m_yStorage)[i] = 0;
	}

	// init CVODE solver
	m_cvodeMem = CVodeCreate(CV_BDF, CV_NEWTON);
	// Initialize cvode memory with equation specific absolute tolerances
	int result = CVodeInit(m_cvodeMem,
						   f_solver,
						   m_t,
						   m_yStorage);
	if (result != CV_SUCCESS)
		throw IBK::Exception("CVodeInit init error.", FUNC_ID);

	// setup matrix, tridiagonal for diffusion/convection model, larger bandwidth for model with dual porosity
	result = CVBand(m_cvodeMem, m_n*m_nVars, bandwidth, bandwidth);
	switch (result) {
		case CVDLS_SUCCESS		: break;
		case CVDLS_MEM_FAIL		: throw IBK::Exception("CVBand memory initialization error (problem too large?)", FUNC_ID);
		case CVDLS_ILL_INPUT	: throw IBK::Exception("CVBand init error (wrong input?)", FUNC_ID);
		default					: throw IBK::Exception("CVBand init error", FUNC_ID);
	}

	// set CVODE parameters
	CVodeSetUserData(m_cvodeMem, (void*)this);
	// set CVODE Max-order
	CVodeSetMaxOrd(m_cvodeMem, 5);
	// set CVODE maximum steps before reaching tout
	CVodeSetMaxNumSteps(m_cvodeMem, 100000);
	// set CVODE initial step size
	CVodeSetInitStep(m_cvodeMem, 1e-6/m_n);
	// set CVODE maximum step size
	CVodeSetMaxStep(m_cvodeMem, input.maxDt);
	// set CVODE minimum step size
	CVodeSetMinStep(m_cvodeMem, input.minDt);
	// set tolerances
	CVodeSVtolerances(m_cvodeMem, m_relTol, m_absTolVec);


	m_cInletData = input.cInletData; // Note: makespline() was already done!

	// initialization of working variables
	m_cREV.resize(m_n);
	m_cc.resize(m_n);
	m_smu_c.resize(m_n);
	m_sgamma_c.resize(m_n);
	m_jdiff.resize(m_n+1);
	m_jconv.resize(m_n+1);

	if (input.model == SolverInput::PLUS_EXCHANGE) {
		m_sREV.resize(m_n);
		m_sc.resize(m_n);
		m_smu_s.resize(m_n);
		m_sgamma_s.resize(m_n);
		m_sbeta.resize(m_n);
	}

	m_outputCounter = 0;

	// initialization complete
	m_initialized = true;
}


void Solver::run() {
	FUNCID(Solver::run);
	if (!m_initialized) return;
	// calculate everything for first step so that we can write the inital output
	storeOutput();
	// call CVODE in steps
	double dt_out = m_input.outputDt;
	double t_out = dt_out;
	int progress = 0; // for the progress indicator
	while (m_t < m_tEnd) {
		// run CVODE
		int result = CVode(m_cvodeMem, t_out, m_yStorage, &m_t, CV_NORMAL);
		if (result < 0)
			throw IBK::Exception("Error while integrating solution.", FUNC_ID);
		int section = static_cast<int>(m_t/m_tEnd*10);
		if (section > progress) {
			std::cout << ".";
			progress = section;
		}
		storeOutput();
		t_out += dt_out;
	}
	m_outputCounter = 0; // force storage of profiles
	storeOutput();
}

int Solver::calculateDivergences(double t, N_Vector y_vec, N_Vector ydot_vec) {
	// readability improvements
	double * y = NV_DATA_S(y_vec);
	double * ydot = nullptr;
	if (ydot_vec != nullptr)
		ydot = NV_DATA_S(ydot_vec);
	double	A		= m_input.A;
	double	L		= m_input.L;
//	double	p		= m_input.p;
	double	v		= m_input.v;
	double	D		= m_input.D;
	double	Rc		= m_input.Rc;
	double	muc		= m_input.muc;
	double	gammac	= m_input.gammac;
	double	Rs		= m_input.Rs;
	double	mus		= m_input.mus;
	double	gammas	= m_input.gammas;
	double	beta	= m_input.beta;

	// calculate inlet concentration
	double	cIn = m_input.cInlet;
	if (!m_cInletData.empty())
		cIn = m_cInletData.value(t/3600.0); // don't forget to convert to h

	double V_rev = A * L/m_n;
	double dx = L/m_n;

	// first extract the primary and secondary state variables from the solution vector
	switch (m_input.model) {
		case SolverInput::DIFF_CONV_PARTITION :
			{
				// equilibrium sorption, y contains total mass densities in kg/m3,
				for (unsigned int i=0; i<m_n; ++i) {
					// store total mass density in kg/m3
					m_cREV[i] = y[i];
					// calculate gas/mobile phase mass densities by dividing by the retention coefficient
					m_cc[i] = m_cREV[i]/Rc;
				}
			}
			break;

		case SolverInput::PLUS_EXCHANGE :
			{
				// separate flow domains
				// y1 contains total gas/mobile mass densities with respect to REV
				// y2 contains immobile mass densities with respect to REV
				for (unsigned int i=0; i<m_n; ++i) {
					// store total mass density in kg/m3
					m_cREV[i] = y[i*m_nVars];
					// store gas/mobile phase mass density in kg/m3(gas)
					m_cc[i] = m_cREV[i]/Rc;
					// store total sorbed phase VOC mass density in kg/m3
					m_sREV[i] = y[i*m_nVars + 1];
					// store sorbed phase VOC mass density in kg/m3
					m_sc[i] = m_sREV[i]/Rs;
				}
			}
			break;
	} // switch

	// ensure all mass densities are non-negative (by clipping)
	for (size_t i=0; i<m_cc.size(); ++i)
		m_cc[i] = std::max(0.0, m_cc[i]);
	for (size_t i=0; i<m_cREV.size(); ++i)
		m_cREV[i] = std::max(0.0, m_cREV[i]);

	for (size_t i=0; i<m_sc.size(); ++i)
		m_sc[i] = std::max(0.0, m_sc[i]);
	for (size_t i=0; i<m_sREV.size(); ++i)
		m_sREV[i] = std::max(0.0, m_sREV[i]);

	// Node numbering
	//    0                 - first bed node
	//    m_nVars           - second bed node
	//    i*m_nVars + j     - pellet shell j of bed element i
	//    (m_n-1)*m_nVars   - last bed node

	// m_cc numbering
	//    0                 - first bed node
	//    1                 - second bed node
	//    m_n-1             - last bed node

	// Flux vector numbering:
	//    0                 - first interface upstream (inlet)
	//    1                 - interface between first and second bed node
	//    m_n               - last interface downstream (outlet)

	// calculate bed fluxes
	unsigned int i_lastBedNode = m_n-1;

	// ** Boundary conditions **

	// at the inlet we have convection and axial diffusion
	// only downwind fluxes permitted
	m_jdiff[0] = D * A * (cIn - m_cc[0])/dx;		// m2/s * m2 * kg/m3 / m = kg/s
	m_jconv[0] = v * A * cIn;						// m3/m2s * m2 * kg/m3 = kg/s

	// at the filter outlet we only consider convection, no back diffusion
	m_jdiff[m_n] = 0;
	m_jconv[m_n] = v * A * m_cc[i_lastBedNode];

	// now calculate the internal convection and axial diffusion fluxes
	for (unsigned int i=1; i<m_n; ++i) {
		m_jdiff[i] = D * A * (m_cc[i-1] - m_cc[i])/dx;
		m_jconv[i] = v * A * m_cc[i-1]; // first order upwind
	}

	// calculate sources/sinks
	for (unsigned int i=0; i<m_n; ++i) {
		m_smu_c[i] = muc*m_cc[i];		// 1/s * kg/m3 = kg/m3s
		m_sgamma_c[i] = gammac;			// kg/m3s
		if (m_input.model == SolverInput::PLUS_EXCHANGE) {
			m_smu_s[i] = mus*m_sc[i];		// 1/s * kg/m3 = kg/m3s
			m_sgamma_s[i] = gammas;			// kg/m3s
			m_sbeta[i] = beta*(m_cc[i] - m_sc[i]);	// kg/m3s - negative for eq 1, positive for eq 2
		}
	}

	// store divergences back in ydot vector, if we have one given
	if (ydot != nullptr) {
		switch (m_input.model) {
			case SolverInput::DIFF_CONV_PARTITION :
				{
					for (unsigned int i=0; i<m_n; ++i) {
						// calculate divergences
						double div = (m_jdiff[i] + m_jconv[i] - m_jdiff[i+1] - m_jconv[i+1]
							- m_smu_c[i] + m_sgamma_c[i])/V_rev;
						ydot[i] = div;
					}
				}
				break;

			case SolverInput::PLUS_EXCHANGE :
				{
					for (unsigned int i=0; i<m_n; ++i) {
						// calculate divergence for mobile phase
						double div_c = (m_jdiff[i] + m_jconv[i] - m_jdiff[i+1] - m_jconv[i+1]
							 - m_sbeta[i] - m_smu_c[i] + m_sgamma_c[i])/V_rev;
						// calculate divergence for mobile phase
						double div_s = (m_sbeta[i] - m_smu_s[i] + m_sgamma_s[i])/V_rev;
						ydot[i*m_nVars] = div_c;
						ydot[i*m_nVars + 1] = div_s;
					}
				}
				break;

		} // switch
/*
#ifdef _DEBUG
		std::vector<double> y_debug(y, y + m_n*m_nVars);
		std::vector<double> ydot_debug(ydot, ydot + m_n*m_nVars);
#endif // _DEBUG
*/
	}
	return 0;
}


void Solver::storeOutput() {
	// don't add, if we just added a profile for this point
	if (!m_outletT.empty() && fabs(m_outletT.back() - m_t/3600) < 1e-10)
		return;
	// re-calculate the temporary variables again for the
	// current output values in m_yStorage
	calculateDivergences(0, m_yStorage, nullptr);
	// store the outlet concentration along with the current time point in a vector
	m_outletT.push_back(m_t/3600.0);
	m_outletC.push_back(m_cc[m_n-1]);
	// also store field outputs if counter matches multiplier
	if (m_outputCounter % m_input.outputN == 0) {
		m_ccProfile.push_back(m_cc);
		m_scProfile.push_back(m_sc);
		m_tProfile.push_back(m_t/3600);
	}
	++m_outputCounter;
}
