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
 * @file 	base_particles.hpp
 * @brief 	This is the implementation of the template functions in base_particles.h 
 * @author	Xiangyu Hu
 */

#ifndef BASE_PARTICLES_HPP
#define BASE_PARTICLES_HPP

#include "base_particles.h"
#include "particle_dynamics_algorithms.h"

namespace SPH
{
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::
        registerAVariable(StdLargeVec<VariableType> &variable_addrs,
                          const std::string &variable_name, VariableType initial_value)
    {
        constexpr int type_index = ParticleDataTypeIndex<VariableType>::value;

        if (all_variable_maps_[type_index].find(variable_name) == all_variable_maps_[type_index].end())
        {
            variable_addrs.resize(real_particles_bound_, initial_value);
            std::get<type_index>(all_particle_data_).push_back(&variable_addrs);
            all_variable_maps_[type_index].insert(make_pair(variable_name, std::get<type_index>(all_particle_data_).size() - 1));
        }
        else
        {
            std::cout << "\n Error: the variable '" << variable_name << "' has already been registered!" << std::endl;
            std::cout << __FILE__ << ':' << __LINE__ << std::endl;
            exit(1);
        }
    }
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::
        registerAVariable(StdLargeVec<VariableType> &variable_addrs,
                          const std::string &new_variable_name, const std::string &old_variable_name)
    {
        constexpr int type_index = ParticleDataTypeIndex<VariableType>::value;

        if (all_variable_maps_[type_index].find(old_variable_name) != all_variable_maps_[type_index].end())
        {
            StdLargeVec<VariableType> *old_variable =
                std::get<type_index>(all_particle_data_)[all_variable_maps_[type_index][old_variable_name]];
            for (size_t i = 0; i != real_particles_bound_; ++i)
                variable_addrs[i] = (*old_variable)[i];
        }
        else
        {
            std::cout << "\n Error: the old variable '" << old_variable_name << "' is not registered!" << std::endl;
            std::cout << __FILE__ << ':' << __LINE__ << std::endl;
            exit(1);
        }
    }
    //=================================================================================================//
    template <typename VariableType>
    StdLargeVec<VariableType> *BaseParticles::getVariableByName(std::string variable_name)
    {
        constexpr int type_index = ParticleDataTypeIndex<VariableType>::value;

        if (all_variable_maps_[type_index].find(variable_name) != all_variable_maps_[type_index].end())
            return std::get<type_index>(all_particle_data_)[all_variable_maps_[type_index][variable_name]];

        std::cout << "\n Error: the variable '" << variable_name << "' is not registered!" << std::endl;
        std::cout << __FILE__ << ':' << __LINE__ << std::endl;
        exit(1);
        return nullptr;
    }
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::
        addAVariableNameToList(ParticleVariableList &variable_name_list, std::string variable_name)
    {
        constexpr int type_index = ParticleDataTypeIndex<VariableType>::value;

        if (all_variable_maps_[type_index].find(variable_name) != all_variable_maps_[type_index].end())
        {
            bool is_to_add = true;
            for (size_t i = 0; i != variable_name_list[type_index].size(); ++i)
            {
                if (variable_name_list[type_index][i].first == variable_name)
                    is_to_add = false;
            }
            if (is_to_add)
            {
                size_t variable_index = all_variable_maps_[type_index][variable_name];
                variable_name_list[type_index].push_back(make_pair(variable_name, variable_index));
            }
        }
        else
        {
            std::cout << "\n Error: the variable '" << variable_name << "' to write is not particle data!" << std::endl;
            std::cout << __FILE__ << ':' << __LINE__ << std::endl;
            exit(1);
        }
    }
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::addAVariableToWrite(std::string variable_name)
    {
        addAVariableNameToList<VariableType>(variables_to_write_, variable_name);
    }
    //=================================================================================================//
    template <class DerivedVariableMethod>
    void BaseParticles::addDerivedVariableToWrite()
    {
        SimpleDynamics<DerivedVariableMethod> *derived_data = derived_particle_data_.createPtr<SimpleDynamics<DerivedVariableMethod>>(*sph_body_);
        derived_variables_.push_back(derived_data);
        using DerivedVariableType = typename DerivedVariableMethod::DerivedVariableType;
        addAVariableNameToList<DerivedVariableType>(variables_to_write_, derived_data->local_dynamics_.variable_name_);
    }
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::addAVariableToRestart(std::string variable_name)
    {
        addAVariableNameToList<VariableType>(variables_to_restart_, variable_name);
    }
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::registerASortableVariable(std::string variable_name)
    {
        constexpr int type_index = ParticleDataTypeIndex<VariableType>::value;

        if (sortable_variable_maps_[type_index].find(variable_name) == sortable_variable_maps_[type_index].end())
        {
            if (all_variable_maps_[type_index].find(variable_name) != all_variable_maps_[type_index].end())
            {
                StdLargeVec<VariableType> *variable =
                    std::get<type_index>(all_particle_data_)[all_variable_maps_[type_index][variable_name]];
                std::get<type_index>(sortable_data_).push_back(variable);
                sortable_variable_maps_[type_index].insert(make_pair(variable_name, std::get<type_index>(sortable_data_).size() - 1));
            }
            else
            {
                std::cout << "\n Error: the variable '" << variable_name << "' for sorting is not registered!" << std::endl;
                std::cout << __FILE__ << ':' << __LINE__ << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cout << "\n Warning: the variable '" << variable_name << "' is already a sortabele variable!" << std::endl;
            std::cout << __FILE__ << ':' << __LINE__ << std::endl;
        }
    }
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::addAParticleDataValue<VariableType>::
    operator()(ParticleData &particle_data) const
    {
        constexpr int type_index = ParticleDataTypeIndex<VariableType>::value;

        for (size_t i = 0; i != std::get<type_index>(particle_data).size(); ++i)
            std::get<type_index>(particle_data)[i]->push_back(VariableType(0));
    }
    //=================================================================================================//
    template <typename VariableType>
    void BaseParticles::copyAParticleDataValue<VariableType>::
    operator()(ParticleData &particle_data, size_t this_index, size_t another_index) const
    {
        constexpr int type_index = ParticleDataTypeIndex<VariableType>::value;

        for (size_t i = 0; i != std::get<type_index>(particle_data).size(); ++i)
            (*std::get<type_index>(particle_data)[i])[this_index] =
                (*std::get<type_index>(particle_data)[i])[another_index];
    }
    //=================================================================================================//
	template<typename StreamType>
	void BaseParticles::writeParticlesToVtk(StreamType &output_stream)
    {
		size_t total_real_particles = total_real_particles_;

		//write current/final particle positions first
		output_stream << "   <Points>\n";
		output_stream << "    <DataArray Name=\"Position\" type=\"Float32\"  NumberOfComponents=\"3\" Format=\"ascii\">\n";
		output_stream << "    ";
		for (size_t i = 0; i != total_real_particles; ++i)
		{
			Vec3d particle_position = upgradeToVector3D(pos_n_[i]);
			output_stream << particle_position[0] << " " << particle_position[1] << " " << particle_position[2] << " ";
		}
		output_stream << std::endl;
		output_stream << "    </DataArray>\n";
		output_stream << "   </Points>\n";

		//write header of particles data
		output_stream << "   <PointData  Vectors=\"vector\">\n";

		//write sorted particles ID
		output_stream << "    <DataArray Name=\"SortedParticle_ID\" type=\"Int32\" Format=\"ascii\">\n";
		output_stream << "    ";
		for (size_t i = 0; i != total_real_particles; ++i)
		{
			output_stream << i << " ";
		}
		output_stream << std::endl;
		output_stream << "    </DataArray>\n";

		//write unsorted particles ID
		output_stream << "    <DataArray Name=\"UnsortedParticle_ID\" type=\"Int32\" Format=\"ascii\">\n";
		output_stream << "    ";
		for (size_t i = 0; i != total_real_particles; ++i)
		{
			output_stream << unsorted_id_[i] << " ";
		}
		output_stream << std::endl;
		output_stream << "    </DataArray>\n";

		//compute derived particle variables
		for (ParticleDynamics<void> *derived_variable : derived_variables_)
		{
			derived_variable->parallel_exec();
		}

		//write matrices
		for (std::pair<std::string, size_t>& name_index : variables_to_write_[2])
		{
			std::string variable_name = name_index.first;
			StdLargeVec<Matd>& variable = *(std::get<2>(all_particle_data_)[name_index.second]);
			output_stream << "    <DataArray Name=\"" << variable_name << "\" type=\"Float32\"  NumberOfComponents=\"9\" Format=\"ascii\">\n";
			output_stream << "    ";
			for (size_t i = 0; i != total_real_particles; ++i)
			{
				Mat3d matrix_value = upgradeToMatrix3D(variable[i]);
				for (int k = 0; k != 3; ++k)
				{
					Vec3d col_vector = matrix_value.col(k);
					output_stream << std::fixed << std::setprecision(9) << col_vector[0] << " " << col_vector[1] << " " << col_vector[2] << " ";
				}
			}
			output_stream << std::endl;
			output_stream << "    </DataArray>\n";
		}

		//write vectors
		for (std::pair<std::string, size_t>& name_index : variables_to_write_[1])
		{
			std::string variable_name = name_index.first;
			StdLargeVec<Vecd>& variable = *(std::get<1>(all_particle_data_)[name_index.second]);
			output_stream << "    <DataArray Name=\"" << variable_name << "\" type=\"Float32\"  NumberOfComponents=\"3\" Format=\"ascii\">\n";
			output_stream << "    ";
			for (size_t i = 0; i != total_real_particles; ++i)
			{
				Vec3d vector_value = upgradeToVector3D(variable[i]);
				output_stream << std::fixed << std::setprecision(9) << vector_value[0] << " " << vector_value[1] << " " << vector_value[2] << " ";
			}
			output_stream << std::endl;
			output_stream << "    </DataArray>\n";
		}

		//write scalars
		for (std::pair<std::string, size_t>& name_index : variables_to_write_[0])
		{
			std::string variable_name = name_index.first;
			StdLargeVec<Real>& variable = *(std::get<0>(all_particle_data_)[name_index.second]);
			output_stream << "    <DataArray Name=\"" << variable_name << "\" type=\"Float32\" Format=\"ascii\">\n";
			output_stream << "    ";
			for (size_t i = 0; i != total_real_particles; ++i)
			{
				output_stream << std::fixed << std::setprecision(9) << variable[i] << " ";
			}
			output_stream << std::endl;
			output_stream << "    </DataArray>\n";
		}

		//write integers
		for (std::pair<std::string, size_t>& name_index : variables_to_write_[3])
		{
			std::string variable_name = name_index.first;
			StdLargeVec<int>& variable = *(std::get<3>(all_particle_data_)[name_index.second]);
			output_stream << "    <DataArray Name=\"" << variable_name << "\" type=\"Int32\" Format=\"ascii\">\n";
			output_stream << "    ";
			for (size_t i = 0; i != total_real_particles; ++i)
			{
				output_stream << std::fixed << std::setprecision(9) << variable[i] << " ";
			}
			output_stream << std::endl;
			output_stream << "    </DataArray>\n";
		}
    }
    //=================================================================================================//
    template <typename VariableType>
    void WriteAParticleVariableToXml::
    operator()(std::string &variable_name, StdLargeVec<VariableType> &variable) const
    {
        SimTK::Xml::element_iterator ele_ite = xml_engine_.root_element_.element_begin();
        for (size_t i = 0; i != total_real_particles_; ++i)
        {
            xml_engine_.setAttributeToElement(ele_ite, variable_name, variable[i]);
            ele_ite++;
        }
    }
    //=================================================================================================//
    template <typename VariableType>
    void ReadAParticleVariableFromXml::
    operator()(std::string &variable_name, StdLargeVec<VariableType> &variable) const
    {
        SimTK::Xml::element_iterator ele_ite = xml_engine_.root_element_.element_begin();
        for (size_t i = 0; i != total_real_particles_; ++i)
        {
            xml_engine_.getRequiredAttributeValue(ele_ite, variable_name, variable[i]);
            ele_ite++;
        }
    }
    //=================================================================================================//
    template <typename VariableType>
    BaseDerivedVariable<VariableType>::
        BaseDerivedVariable(const SPHBody &sph_body, const std::string &variable_name)
        : variable_name_(variable_name)
    {
        sph_body.base_particles_->template registerAVariable<VariableType>(derived_variable_, variable_name_);
    };
    //=================================================================================================//
}
#endif //BASE_PARTICLES_HPP