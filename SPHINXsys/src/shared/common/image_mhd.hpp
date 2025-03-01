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
* @file image_mhd.hpp
* @brief x
* @details x
*			x
* @author	Yijin Mao
*/
#ifndef IMAGE_MHD_HPP
#define IMAGE_MHD_HPP

#ifndef __EMSCRIPTEN__

#include "image_mhd.h"
#include "boost/algorithm/string.hpp"

namespace SPH {

	template<typename T, int nDims>
	ImageMHD<T, nDims>::ImageMHD(std::string full_path_to_file):
		objectType_("Image"),
		ndims_(nDims),
		binaryData_(true),
		binaryDataByteOrderMSB_(false),
		compressedData_(false),
		transformMatrix_(Mat3d(1.0)),
		offset_(Vec3d(0.0,0.0,0.0)),
		centerOfRotation_(Vec3d(0.0,0.0,0.0)),
		elementSpacing_(Vec3d(1.0,1.0,1.0)),
		dimSize_(Vec3i(1,1,1)),
		width_(dimSize_[0]),
		height_(dimSize_[1]),
		depth_(dimSize_[2]),
		size_(dimSize_[0] * dimSize_[1] * dimSize_[2]),
		anatomicalOrientation_("???"),
		elementType_(MET_FLOAT),
		elementDataFile_(""),
		min_value_(Infinity),
		max_value_(-Infinity),
		data_(nullptr)
	{
		//- read mhd file
		std::ifstream dataFile(full_path_to_file, std::ifstream::in);
		std::string file_path_to_raw_file;
		if (dataFile.is_open())
		{
			std::string line;
			std::vector<std::string> values;
			while (std::getline(dataFile, line))
			{
				std::stringstream ss(line);
				std::vector<std::string> elements;
				split(line, '=', elements);
				if (elements.size() == 2)
				{
					boost::trim(elements[0]);
					boost::trim(elements[1]);
					if (elements[0].compare("TransformMatrix") == 0)
					{
						std::vector<std::string> values;
						split(elements[1], ' ', values);
						transformMatrix_[0][0] = std::stof(values[0]);
						transformMatrix_[0][1] = std::stof(values[1]);
						transformMatrix_[0][2] = std::stof(values[2]);
						transformMatrix_[1][0] = std::stof(values[3]);
						transformMatrix_[1][1] = std::stof(values[4]);
						transformMatrix_[1][2] = std::stof(values[5]);
						transformMatrix_[2][0] = std::stof(values[6]);
						transformMatrix_[2][1] = std::stof(values[7]);
						transformMatrix_[2][2] = std::stof(values[8]);
					}
					else if (elements[0].compare("Offset") == 0)
					{
						std::vector<std::string> values;
						split(elements[1], ' ', values);
						offset_[0] = std::stof(values[0]);
						offset_[1] = std::stof(values[1]);
						offset_[2] = std::stof(values[2]);
					}
					else if (elements[0].compare("ElementSpacing") == 0)
					{
						std::vector<std::string> values;
						split(elements[1], ' ', values);
						elementSpacing_[0] = std::stof(values[0]);
						elementSpacing_[1] = std::stof(values[1]);
						elementSpacing_[2] = std::stof(values[2]);
					}
					else if (elements[0].compare("DimSize") == 0)
					{
						std::vector<std::string> values;
						split(elements[1], ' ', values);
						dimSize_[0] = std::stoi(values[0]);
						dimSize_[1] = std::stoi(values[1]);
						dimSize_[2] = std::stoi(values[2]);
						width_ = dimSize_[0];
						height_ = dimSize_[1];
						depth_ = dimSize_[2];
						size_ = width_ * height_*depth_;
						if(data_ == nullptr)
							data_ = new T[dimSize_[0] * dimSize_[1] * dimSize_[2]];
					}
					else if (elements[0].compare("ElementDataFile") == 0)
					{
						full_path_to_file = full_path_to_file.substr(0, full_path_to_file.find_last_of("\\/"));
						file_path_to_raw_file = full_path_to_file + '/' + elements[1];
					}
				}
			}
		}

		dataFile.close();
		std::cout << "dimensions: " << dimSize_ << std::endl;
		std::cout << "spacing: " << elementSpacing_ << std::endl;
		std::cout << "offset: " << offset_ << std::endl;
		std::cout << "transformMatrix: " << transformMatrix_ << std::endl;

		//- read raw file
		std::ifstream dataFileRaw(file_path_to_raw_file, std::ios::in | std::ios::binary);

		if (dataFileRaw.is_open())
		{
			dataFileRaw.read((char*)data_, sizeof(T)*size_);
			T distance = 0;
			for(int index = 0; index<size_; index++)
			{
				distance = data_[index];
				data_[index] = distance;
				//std::cout <<index <<" "<< distance << '\n';
				if (distance < min_value_) min_value_ = distance;
				if (distance > max_value_) max_value_ = distance;
			}
		}
		dataFileRaw.close();

		//write(std::string("sphere-binary"),ASCII);
	}
	template<typename T, int nDims>
	ImageMHD<T, nDims>::ImageMHD(Real radius, Vec3i NxNyNz, Vec3d spacings):
		objectType_("Image"),
		ndims_(nDims),
		binaryData_(true),
		binaryDataByteOrderMSB_(false),
		compressedData_(false),
		transformMatrix_(Mat3d(1.0)),
		offset_(Vec3d(-0.5*NxNyNz[0]*spacings[0], -0.5*NxNyNz[1] * spacings[1], -0.5*NxNyNz[2] * spacings[2])),
		centerOfRotation_(Vec3d(0.0, 0.0, 0.0)),
		elementSpacing_(spacings),
		dimSize_(NxNyNz),
		width_(dimSize_[0]),
		height_(dimSize_[1]),
		depth_(dimSize_[2]),
		size_(width_*height_*depth_),
		anatomicalOrientation_("???"),
		elementType_(MET_FLOAT),
		elementDataFile_(""),
		min_value_(Infinity),
		max_value_(-Infinity),
		data_(nullptr)
	{
		if(data_ == nullptr) 
			data_ = new float[size_];

		Vec3d center(0.5*width_, 0.5*height_, 0.5*depth_);

		for (int z = 0; z < depth_; z++)
		{
			for (int y = 0; y < height_; y++)
			{
				for (int x = 0; x < width_; x++)
				{
					int index = z * width_*height_ + y * width_ + x;
					double distance = (Vec3d(x, y, z) - center).norm() - radius;
					if (distance < min_value_) min_value_ = distance;
					if (distance > max_value_) max_value_ = distance;
					data_[index] = float(distance);
				}
			}
		}
		write(std::string("sphere"), BINARY);
	}

	template<typename T, int nDims>
	ImageMHD<T, nDims>::~ImageMHD()
	{
		if (data_)
		{
			delete data_;
			data_ = nullptr;
		}
	}

	//=================================================================================================//
	template<typename T, int nDims>
	std::vector<int> ImageMHD<T, nDims>::findNeighbors(const Vec3d& input_pnt, Vec3i& this_cell)
	{
		std::vector<int> neighbors;

		Vec3d image_coord = transformMatrix_.invert()*(input_pnt - offset_);
		// std::cout <<"findNeighbor of " << input_pnt << " ........... " << image_coord << std::endl;

		int z = int(floor(image_coord[2]));
		int y = int(floor(image_coord[1]));
		int x = int(floor(image_coord[0]));

		//- cannot count cells in buffer zone
		if (x < 0 || x > width_ - 1 || y < 0 || y > height_ - 1 || z < 0 || z > depth_ - 1) return neighbors;

		for (int k = z - 1; k < z + 2; k = k + 2)
		{
			for (int j = y - 1; j < y + 2; j = j + 2)
			{
				for (int i = x - 1; i < x + 2; i = i + 2)
				{
					if (i<0 || i >width_ - 1 || j <0 || j >height_ - 1 || k<0 || k >depth_)
						continue;
					int index = z * width_*height_ + y * width_ + x;
					neighbors.push_back(index);
				}
			}

		}

		return neighbors;

	}
	//=================================================================================================//
	template<typename T, int nDims>
	Vec3d ImageMHD<T, nDims>::computeGradientAtCell(int i)
	{
		//- translate 1D index to 3D index
		int width = width_;
		int height = height_;
		int depth = depth_;
		int sliceSize = width * height;
		int z = i / sliceSize;
		int y = (i%sliceSize) / width;
		int x = (i%sliceSize) % width;

		double gradx = 0.0; double grady = 0.0; double gradz = 0.0;
		//- cds (if inner cell)
		//- otherwise back/forward scheme
		if (x == 0)
		{
			int indexHigh = z * sliceSize + y * width + (x + 1);
			gradx = (getValueAtCell(indexHigh) - getValueAtCell(i));
		}
		else if (x == width - 1)
		{
			int indexLow = z * sliceSize + y * width + (x - 1);
			gradx = -(getValueAtCell(indexLow) - getValueAtCell(i));
		}
		else if (x > 0 && x < width_ - 1)
		{
			int indexHigh = z * sliceSize + y * width + (x + 1);
			int indexLow = z * sliceSize + y * width + (x - 1);
			gradx = (getValueAtCell(indexHigh) - getValueAtCell(indexLow)) / 2.0;
		}

		if (y == 0)
		{
			int indexHigh = z * sliceSize + (y + 1)*width + x;
			grady = (getValueAtCell(indexHigh) - getValueAtCell(i));
		}
		else if (y == height - 1)
		{
			int indexLow = z * sliceSize + (y - 1)*width + x;
			grady = -(getValueAtCell(indexLow) - getValueAtCell(i));
		}
		else if (y > 0 && y < height_ - 1)
		{
			int indexHigh = z * sliceSize + (y + 1)*width + x;
			int indexLow = z * sliceSize + (y - 1)*width + x;
			grady = (getValueAtCell(indexHigh) - getValueAtCell(indexLow)) / 2.0;
		}

		if (z == 0)
		{
			int indexHigh = (z + 1)*sliceSize + y * width + x;
			gradz = (getValueAtCell(indexHigh) - getValueAtCell(i));
		}
		else if (z == depth - 1)
		{
			int indexLow = (z - 1)*sliceSize + y * width + x;
			gradz = -(getValueAtCell(indexLow) - getValueAtCell(i));
		}
		else if (z > 0 && z < depth_ - 1)
		{
			int indexHigh = (z + 1)*sliceSize + y * width + x;
			int indexLow = (z - 1)*sliceSize + y * width + x;
			gradz = (getValueAtCell(indexHigh) - getValueAtCell(indexLow)) / 2.0;
		}
		gradx = gradx / elementSpacing_[0];
		grady = grady / elementSpacing_[1];
		gradz = gradz / elementSpacing_[2];
		return Vec3d(gradx, grady, gradz);

	}
	//=================================================================================================//
	template<typename T, int nDims>
	Vec3d ImageMHD<T, nDims>::computeNormalAtCell(int i)
	{
		Vec3d grad_phi = computeGradientAtCell(i);
		Vec3d n = grad_phi.normalize();
		return n;
	}

	template<typename T, int nDims>
	T ImageMHD<T, nDims>::getValueAtCell(int i)
	{
		if (i < 0 || i > size_)
		{
			return float(max_value_);
		}
		else
		{
			return data_[i];
		}

	}
	//=================================================================================================//
	template<typename T, int nDims>
	Vec3d ImageMHD<T, nDims>::convertToPhysicalSpace(Vec3d p)
	{
		Vec3d position = transformMatrix_ * p + offset_;
		for (int i = 0; i < position.size(); i++)
		{
			position[i] = position[i] * elementSpacing_[i];
		}
		return position;

	}
	//=================================================================================================//
	template<typename T, int nDims>
	void ImageMHD<T, nDims>::split(const std::string &s, char delim, \
		std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			if (item.length() > 0) elems.push_back(item);
		}
	}
	//=================================================================================================//
	template<typename T, int nDims>
	Vec3d ImageMHD<T, nDims>::findClosestPoint(const Vec3d& input_pnt)
	{
		Vec3i this_cell;
		std::vector<int> neighbors = findNeighbors(input_pnt, this_cell);
		Vec3d n_sum(0.0, 0.0, 0.0);
		double weight_sum = 0.0;
		double d_sum = 0.0;
		for (const int& i : neighbors)
		{
			// checkIndexBound(i);
			Vec3d nCj = computeNormalAtCell(i);
			double dCj = float(getValueAtCell(i));
			double weight_Cj = 1.0 / (fabs(dCj) + Eps);
			n_sum = n_sum + weight_Cj * nCj;
			weight_sum = weight_sum + weight_Cj;
			d_sum = d_sum + dCj;
		}
		Vec3d n = n_sum / (weight_sum + Eps);
		double d = d_sum / (weight_sum + Eps);

		Vec3d p_image = Vec3d(this_cell[0], this_cell[1], this_cell[2]) + n.normalize()*d;
		Vec3d p = convertToPhysicalSpace(p_image);
		return p;

	}

	template<typename T, int nDims>
	BoundingBox ImageMHD<T, nDims>::findBounds()
	{
		//initial reference values
		Vec3d lower_bound = Vec3d(Infinity);
		Vec3d upper_bound = Vec3d(-Infinity);

		for (int z = 0; z < depth_ + 1; z++)
		{
			for (int y = 0; y < height_ + 1; y++)
			{
				for (int x = 0; x < width_ + 1; x++)
				{
					Vec3d p_image = Vec3d(x, y, z);
					Vec3d vertex_position = convertToPhysicalSpace(p_image);
					for (int j = 0; j != 3; ++j) {
						lower_bound[j] = SMIN(lower_bound[j], vertex_position[j]);
						upper_bound[j] = SMAX(upper_bound[j], vertex_position[j]);
					}
				}
			}
		}
		return BoundingBox(lower_bound, upper_bound);
	}

	template<typename T, int nDims>
	Real ImageMHD<T, nDims>::findValueAtPoint(const Vec3d& input_pnt)
	{
		Vec3i this_cell;
		std::vector<int> neighbors = findNeighbors(input_pnt, this_cell);
		double weight_sum = 0.0;
		double d_sum = 0.0;
		if (neighbors.size() > 0)
		{
			for (const int& i : neighbors)
			{
				// checkIndexBound(i);
				double dCj = float(getValueAtCell(i));
				double weight_Cj = 1.0 / (fabs(dCj) + Eps);
				weight_sum = weight_sum + weight_Cj;
				d_sum = d_sum + dCj;
			}
			return d_sum / (weight_sum + Eps);
		}
		else
		{
			return max_value_;
		}

	}
	//=================================================================================================//
	template<typename T, int nDims>
	Vec3d ImageMHD<T, nDims>::findNormalAtPoint(const Vec3d & input_pnt)
	{
		Vec3i this_cell;
		std::vector<int> neighbors = findNeighbors(input_pnt, this_cell);
		Vec3d n_sum(0.0, 0.0, 0.0);
		double weight_sum = 0.0;
		double d_sum = 0.0;
		if (neighbors.size() > 0)
		{
			for (const int& i : neighbors)
			{
				// checkIndexBound(i);
				Vec3d nCj = computeNormalAtCell(i);
				double dCj = float(getValueAtCell(i));
				double weight_Cj = 1.0 / (fabs(dCj) + Eps);
				n_sum = n_sum + weight_Cj * nCj;
				weight_sum = weight_sum + weight_Cj;
				d_sum = d_sum + dCj;
			}
			Vec3d n = n_sum / (weight_sum + Eps);
			return n.normalize();
		}
		else
		{
			return Vec3d(1.0, 1.0, 1.0).normalize();
		}
	}

	//=================================================================================================//
	template<typename T, int nDims>
	void ImageMHD<T, nDims>::write(std::string filename, Output_Mode mode)
	{
		std::ofstream output_file(filename+".mhd", std::ofstream::out);
		output_file << "ObjectType = " << objectType_ << "\n";
		output_file << "NDims = " << ndims_ << "\n";
		if (mode == BINARY)
			output_file << "BinaryData = True" << "\n";
		else
			output_file << "BinaryData = False" << "\n";
		output_file << "BinaryDataByteOrderMSB = " << binaryDataByteOrderMSB_ << "\n";
		output_file << "CompressedData = " << compressedData_ << "\n";
		output_file << "TransformMatrix = "
			<< transformMatrix_[0][0] << " " << transformMatrix_[0][1] << " " << transformMatrix_[0][2] << " "
			<< transformMatrix_[1][0] << " " << transformMatrix_[1][1] << " " << transformMatrix_[1][2] << " "
			<< transformMatrix_[2][0] << " " << transformMatrix_[2][1] << " " << transformMatrix_[2][2] << "\n";
		output_file << "Offset = "
			<< offset_[0] << " " << offset_[1] << " " << offset_[2] << "\n";
		output_file << "CenterOfRotation = "
			<< centerOfRotation_[0] << " " << centerOfRotation_[1] << " " << centerOfRotation_[2] << "\n";
		output_file << "ElementSpacing = "
			<< elementSpacing_[0] << " " << elementSpacing_[1] << " " << elementSpacing_[2] << "\n";
		output_file << "DimSize = "
			<< dimSize_[0] << " " << dimSize_[1] << " " << dimSize_[2] << "\n";
		output_file << "AnatomicalOrientation = " << anatomicalOrientation_ << "\n";
		if (elementType_ == MET_FLOAT)
			output_file << "ElementType = MET_FLOAT" << "\n";
		else if (elementType_ == MET_UCHAR)
			output_file << "ElementType = MET_UCHAR" << "\n";
		if (elementType_ == MET_LONG)
			output_file << "ElementType = MET_LONG" << "\n";
		output_file << "ElementDataFile = "<< filename+".raw" << "\n";

		output_file.close();
		
		if (mode == BINARY)
		{
			std::ofstream output_file_raw(filename + ".raw", std::ios::binary | std::ios::out);
			output_file_raw.write((const char*)data_, sizeof(T)*size_);
			output_file_raw.close();
		}
		else
		{
			std::ofstream output_file_raw(filename + ".raw");
			for (int index = 0; index < size_; index++)
			{
				output_file_raw << data_[index] << std::endl;
			}
			output_file_raw.close();
		}

		
	}
}
#endif //__EMSCRIPTEN__

#endif //IMAGE_MHD_HPP