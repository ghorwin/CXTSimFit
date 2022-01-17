# CXTSimFit
A tracer / break-through analysis and diffusion and convenction coefficient fit tool - simulation based, as opposed to the original CXTFIT

This is a re-design of the original CXTFit software using a modern PDE solver to support measured, variable concentration pulses.

Internally, it uses the CVODE integrator (SUNDIALS Suite) with error-controlled adaptive time integration. The sample is automatically discretized (equidistant) and the progression of the concentration pulse is simulated dynamically.


## Screenshots

Mind, this software was first published in 2008, hence the screenshots show OS versions that are long out of date. However, I still like the window manager decorations and color schemes from back then :-)

### Kubuntu 8.10
![CXTSimFit on Kubuntu 8.10](doc/snapshot_kubuntu8.10.png)

### OpenSuse 10.3
![CXTSimFit on OpenSuse 10.3](doc/snapshot_openSuse_10.3.png)

### OpenSuse 11.1
![CXTSimFit on OpenSuse 11.1](doc/snapshot_openSuse_11.1.png)

### Windows XP
![CXTSimFit on Windows XP](doc/snapshot_windows_XP.png)
