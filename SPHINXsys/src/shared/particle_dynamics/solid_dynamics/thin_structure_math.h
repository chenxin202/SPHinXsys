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
* @file 	thin_structure_math.h
* @brief 	Here, we define the math operation for thin structure dynamics. 
* @author	Dong Wu and Xiangyu Hu
* @version	0.1
*/

#ifndef THIN_STRUCTURE_MATH_H
#define THIN_STRUCTURE_MATH_H


#include "all_particle_dynamics.h"
#include "elastic_solid.h"
#include "weakly_compressible_fluid.h"
#include "base_kernel.h"

namespace SPH
{
	namespace thin_structure_dynamics
	{
		/**
		* @function getVectorAfterThinStructureRotation
		* @brief Each of these basic vector rotations appears counterclockwise
		* @brief when the axis about which they occur points toward the observer,
		* @brief and the coordinate system is right-handed.
		*/
		Vec2d getVectorAfterThinStructureRotation(const Vec2d &initial_vector, const Vec2d &rotation_angles);
		Vec3d getVectorAfterThinStructureRotation(const Vec3d &initial_vector, const Vec3d &rotation_angles);

		/** Vector change rate after rotation. */
		Vec2d getVectorChangeRateAfterThinStructureRotation(const Vec2d &initial_vector, const Vec2d &rotation_angles, const Vec2d &angular_vel);
		Vec3d getVectorChangeRateAfterThinStructureRotation(const Vec3d &initial_vector, const Vec3d &rotation_angles, const Vec3d &angular_vel);

		/** get the rotation from pseudo-normal for finite deformation. */
		Vec2d getRotationFromPseudoNormalForFiniteDeformation(const Vec2d &dpseudo_n_d2t, const Vec2d &rotation, const Vec2d &angular_vel, Real dt);
		Vec3d getRotationFromPseudoNormalForFiniteDeformation(const Vec3d &dpseudo_n_d2t, const Vec3d &rotation, const Vec3d &angular_vel, Real dt);

		/** get the rotation from pseudo-normal for small deformation. */
		Vec2d getRotationFromPseudoNormalForSmallDeformation(const Vec2d &dpseudo_n_d2t, const Vec2d &rotation, const Vec2d &angular_vel, Real dt);
		Vec3d getRotationFromPseudoNormalForSmallDeformation(const Vec3d &dpseudo_n_d2t, const Vec3d &rotation, const Vec3d &angular_vel, Real dt);

		/** get the current normal direction from deformation gradient tensor. */
		Vec2d getNormalFromDeformationGradientTensor(const Mat2d &F);
		Vec3d getNormalFromDeformationGradientTensor(const Mat3d &F);

		/** get variable jump form gradient tensor. */
		Vecd getLinearVariableJump(const Vecd &e_ij, const Real &r_ij, const Vecd &particle_i_value,
			const Matd &gradient_particle_i_value, const Vecd &particle_j_value, const Matd &gradient_particle_j_value);
		Vecd getWENOVariableJump(const Vecd &e_ij, const Real &r_ij, const Vecd &particle_i_value,
			const Matd &gradient_particle_i_value, const Vecd &particle_j_value, const Matd &gradient_particle_j_value);

		/** predict mid-point value by applying WENO reconstruction. */
		Vecd getWENOStateWithStencilPoints(const Vecd &v1, const Vecd &v2, const Vecd &v3, const Vecd &v4);
		Vecd getWENOLeftState(const Vecd &e_ij, const Real &r_ij, const Vecd &particle_i_value,
			const Matd &gradient_particle_i_value, const Vecd &particle_j_value, const Matd &gradient_particle_j_value);
		Vecd getWENORightState(const Vecd &e_ij, const Real &r_ij, const Vecd &particle_i_value,
			const Matd &gradient_particle_i_value, const Vecd &particle_j_value, const Matd &gradient_particle_j_value);

		/** get the artificial rotation from the pseudo-normal jump. */
		Vec2d getRotationJump(const Vec2d &pseudo_n_jump, const Mat2d &transformation_matrix);
		Vec3d getRotationJump(const Vec3d &pseudo_n_jump, const Mat3d &transformation_matrix);

		/** get the corrected Eulerian Almansi strain tensor according to plane stress problem. */
		Mat2d getCorrectedAlmansiStrain(const Mat2d &current_local_almansi_strain, const Real &nu_);
		Mat3d getCorrectedAlmansiStrain(const Mat3d &current_local_almansi_strain, const Real &nu_);
	}
}
#endif //THIN_STRUCTURE_MATH_H