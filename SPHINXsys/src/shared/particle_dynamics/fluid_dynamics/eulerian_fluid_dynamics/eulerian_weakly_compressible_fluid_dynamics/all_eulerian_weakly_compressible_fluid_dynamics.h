/* -------------------------------------------------------------------------*
*								SPHinXsys									*
* --------------------------------------------------------------------------*
* SPHinXsys (pronunciation: s'finksis) is an acronym from Smoothed Particle	*
* Hydrodynamics for industrial compleX systems. It provides C++ APIs for	*
* physical accurate simulation and aims to model coupled industrial dynamic *
* systems including fluid, solid, multi-body dynamics and beyond with SPH	*
* (smoothed particle hydrodynamics), a meshless computational method using	*
* particle discretization.													*
*																			*
* SPHinXsys is partially funded by German Research Foundation				*
* (Deutsche Forschungsgemeinschaft) DFG HU1527/6-1, HU1527/10-1				*
* and HU1527/12-1.															*
*                                                                           *
* Portions copyright (c) 2017-2020 Technical University of Munich and		*
* the authors' affiliations.												*
*                                                                           *
* Licensed under the Apache License, Version 2.0 (the "License"); you may   *
* not use this file except in compliance with the License. You may obtain a *
* copy of the License at http://www.apache.org/licenses/LICENSE-2.0.        *
*                                                                           *
* --------------------------------------------------------------------------*/
/**
* @file all_weakly_compressible_eulerian_fluid_dynamics.h
* @brief This is the header file that user code should include to pick up all eulerian
* fluid dynamics used in SPHinXsys.
* @details The eulerian fluid dynamics algorithms begin for fluid bulk without boundary condition,
* then algorithm interacting with wall is defined.
* @author	Zhentong Wang
*/
#pragma once

#include "eulerian_weakly_compressible_fluid_dynamics_inner.h"
#include "eulerian_weakly_compressible_fluid_dynamics_inner.hpp"
#include "eulerian_weakly_compressible_fluid_dynamics_complex.h"
#include "eulerian_weakly_compressible_fluid_dynamics_complex.hpp"