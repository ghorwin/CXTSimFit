#include "solverinput.h"

SolverInput::SolverInput() : 
	n(3)
{
	relTol = 1e-5;
	absTol = 1e-10;
	minDt = 1e-16;
	maxDt = 600;
	outputDt = 1800;
	outputN = 2;
	digits = 1e-15;
}
