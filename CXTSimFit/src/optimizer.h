#ifndef optimizer_h
#define optimizer_h

class FilterFit;
class SolverInput;

class Optimizer {
public:
	Optimizer(FilterFit * parent, SolverInput * input);
	double operator()(double x);

	FilterFit *		m_parent;
	SolverInput *	m_input;

	unsigned int m_evals;
};


#endif // optimizer_h
