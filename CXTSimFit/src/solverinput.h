#ifndef solverinput_h
#define solverinput_h

#include "IBK_linearspline.h"

/// This class encapsulates all data needed by the solver.
/// The solver itself supports several calculation models, identified by the member variable 'model'.
class SolverInput {
public:
	/// The different supported models for simulating the filter.
	///
	/// DIFF_CONV_PARTITION is the simplest model, treating the whole filter as homogeneous material.
	/// Diffusion and Convection happens in the gas phase. Sorption is modeled through an equilibrium
	/// model.<br>
	/// The following parameters can be fitted:<ul>
	/// <li>the mobile phase diffusion coefficient</li>
	/// <li>the retention coefficient</li>
	/// <li>the porosity</li>
	/// </ul>
	///
	/// PLUS_EXCHANGE extends the DIFF_CONV_PARTITION by a kinetic storage term, where the storage
	/// in the pellets is not instantaneous, but happens via a mass exchange coefficient. The mass
	/// of all pellets inside a bed node is taken together as uniform storage medium (no gradient
	/// within pellets). In this model, the bed itself only consists of gas and has therefore very 
	/// little storage capacity on its own.<br>
	/// The following parameters can also be fitted:<ul>
	/// <li>the mass transfer coefficient</li>
	/// </ul>
	enum model_t {
		DIFF_CONV_PARTITION,
		PLUS_EXCHANGE
	};
	
	/// Constructor, initializes all variables with some meaningful defaults.
	SolverInput();

	// Numerical input parameters 
	int					n;			///< Number of elements for spatial discretization
	double				tEnd;		///< Simulation end time point in s
	double				relTol;		///< Relative tolerance permitted
	double				absTol;		///< Absolute tolerance requested (for decaying processes)
	double				minDt;		///< Minimum time step in s
	double				maxDt;		///< Maximum time step in s
	double				outputDt;	///< Output time steps for break-through in s
	int					outputN;	///< Every nth break-through output a field output is written.
	double				digits;		///< Accuracy required for the LevMar algorithm.

	// Physical parameters
	double				A;		///< Cross section in m2
	double				L;		///< Length in m
	double				q;		///< Free stream air flow rate in m3/s
	double				p;		///< Porosity in m3/m3
	double				v;		///< Convection flow rate inside REV in m3/m2s (derived quantity)

	// Model parameters
	model_t				model;	///< The currently selected model type
	double				D;		///< Diffusion coefficient in m2(REV)/s (along the filter thickness, _not_ gas phase diffusion coefficient)
	double				Rc;		///< Retention coefficient in kg/kg
	double				muc;	///< Reaction coefficient (linear) in 1/s
	double				gammac;	///< Source/sink in kg/m3s
	double				Rs;		///< Retention coefficient in kg/kg
	double				mus;	///< Reaction coefficient (linear) in 1/s
	double				gammas;	///< Source/sink in kg/m3s
	double				beta;	///< Mass transfer coefficient in 1/s

	double				cInlet;			///< Inlet concentration in kg/m3, only used if cInletData is empty
	IBK::LinearSpline	cInletData;		///< Inlet concentration in kg/m3
};

#endif // solverinput_h
