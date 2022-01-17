# CXTSimFit
A tracer / break-through analysis and diffusion and convenction coefficient fit tool - simulation based, as opposed to the original CXTFIT

This is a re-design of the original CXTFit software using a modern PDE solver to support measured, variable concentration pulses.

Internally, it uses the CVODE integrator (SUNDIALS Suite) with error-controlled adaptive time integration. The sample is automatically discretized (equidistant) and the progression of the concentration pulse is simulated dynamically.

