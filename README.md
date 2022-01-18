# CXTSimFit

_CXTSimFit_ is a small tool to determine model parameters through inverse modeling of Tracer experiments (or filter tests). It fits the parameters of a typical diffusion and convection equation (with sorption and source term, and optional dual porosity) to measured inlet/outlet data. As opposed to the original CXTFit (see reference below) it can handle much more complex equations and handle non-constant inlet conditions.

Paper of the original CXTFIT:

_The CXTFIT Code for Estimating Transport Parameters from Laboratory or Field Tracer Experiments_, Research Report No. 137, April 1999, N. Toride, F. J. Leij, and M. Th. van Genuchten

*Note:* nor the CXTSimFit code not the model formulation or anything is in this repository is related or uses material from the original code/model. The purpose of the software is, however, very similar, hence the naming.

## What it does

This is a re-design of the original CXTFit software using a modern PDE solver to support measured, variable concentration pulses.

Internally, it uses the CVODE integrator (SUNDIALS Suite) with error-controlled adaptive time integration. The sample is automatically discretized (equidistant) and the progression of the concentration pulse is simulated dynamically.

Also, a dual-porosity model is implemented, to better capture sorption characteristics in filters.

## Documentation

The mathematical theory is written down in a small paper: [CXTSimFit Manual](doc/cxtsimfit_manual.pdf)

## Authors

The tool was a small side project that I (Andreas Nicolai) developed at Syracuse University in the BEESL group together with my colleague Jing Jing Pei. Sorry, Jing Jing, that I made you listen to all my boring explanations on numerics and C++ programming :-)

## Screenshots

Mind, this software was first published in 2009/2010, hence the screenshots show OS versions that are now long out of date. However, I still like the window manager decorations and color schemes from back then :-)

### Kubuntu 8.10
![CXTSimFit on Kubuntu 8.10](doc/snapshot_kubuntu8.10.png)

### OpenSuse 10.3
![CXTSimFit on OpenSuse 10.3](doc/snapshot_openSuse_10.3.png)

### OpenSuse 11.1
![CXTSimFit on OpenSuse 11.1](doc/snapshot_openSuse_11.1.png)

### Windows XP
![CXTSimFit on Windows XP](doc/snapshot_windows_XP.png)
