#ifndef solverresults_h
#define solverresults_h

#include <vector>
#include "solverinput.h"

#include "IBK_linearspline.h"

/// Contains all results from a solver run.
class SolverResults {
public:
	/// Resets the member variables to a state prior to simulation.
	void clear();

	/// Calculates the R-Square value for the given results and the given other curve.
	double calculateRSquare(const IBK::LinearSpline & other);

	SolverInput 			input; /// The input data used for this run.

	IBK::LinearSpline		data;
	double					R2;

	std::vector<std::vector<double> >	ccProfiles;
	std::vector<std::vector<double> >	scProfiles;
	std::vector<double>					tProfiles;

};

#endif // solverresults_h
