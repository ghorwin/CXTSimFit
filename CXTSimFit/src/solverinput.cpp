#include "solverinput.h"

SolverInput::SolverInput() :
	n(3)
{
	relTol = 1e-5;
	absTol = 1e-10;
	minDt = 1e-16;
	maxDt = 100;
	outputDt = 600;
	outputN = 6;
	digits = 1e-15;
}
