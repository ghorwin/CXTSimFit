#include "solverresults.h"
#include <numeric>
using namespace std;

void SolverResults::clear() {
	data.clear(); // also marks the solver results as invalid
	ccProfiles.clear();
	scProfiles.clear();
}

double SolverResults::calculateRSquare(const IBK::LinearSpline & other) {
	std::string errmsg;
	if (!data.makeSpline(errmsg))
		return R2 = -1;

	// check if outlet data is available
	if (!other.valid() || !data.valid()) {
		return R2 = -1;
	}

	// get max value
	double max_x = other.x().back();
	if (data.x().back() < max_x)
		max_x = data.x().back();

	// get mean value of y
	double mean_y = std::accumulate(other.y().begin(), other.y().end(), 0.0)/other.y().size();

	double dx = 0.1; // TODO : adjust fixed value
	double x = 0;
	int intervals = 0;
	double SSR = 0;
	double SSY = 0;
	while (x < max_x) {
		double y1 = data.value(x);
		double y2 = other.value(x);
		SSR += (y1-y2)*(y1-y2);
		SSY += (y1-mean_y)*(y1-mean_y);
		x += dx;
		intervals++;
	}
	R2 = 1 - SSR/SSY;
	return R2;
}



