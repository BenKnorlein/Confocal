//  ----------------------------------
//  Copyright © 2015, Brown University, Providence, RI.
//  
//  All Rights Reserved
//   
//  Use of the software is provided under the terms of the GNU General Public License version 3 
//  as published by the Free Software Foundation at http://www.gnu.org/licenses/gpl-3.0.html, provided 
//  that this copyright notice appear in all copies and that the name of Brown University not be used in 
//  advertising or publicity pertaining to the use or distribution of the software without specific written 
//  prior permission from Brown University.
//  
//  See license.txt for further information.
//  
//  BROWN UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE WHICH IS 
//  PROVIDED “AS IS”, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
//  FOR ANY PARTICULAR PURPOSE.  IN NO EVENT SHALL BROWN UNIVERSITY BE LIABLE FOR ANY 
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR FOR ANY DAMAGES WHATSOEVER RESULTING 
//  FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
//  OTHER TORTIOUS ACTION, OR ANY OTHER LEGAL THEORY, ARISING OUT OF OR IN CONNECTION 
//  WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
//  ----------------------------------
//  
///\file LoadProcessedDataAction.cpp
///\author Benjamin Knorlein
///\date 06/13/2018

#pragma once

#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
	#include <windows.h>
	#define OS_SLASH "\\"
#include "external/msvc/dirent.h"
#else
	#define OS_SLASH "//"
	#include <dirent.h>
#endif

#include "LoadProcessedDataAction.h"
#include "Data.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>


using namespace conf;

LoadProcessedDataAction::LoadProcessedDataAction(std::string folder) : m_folder(folder)
{

}

void LoadProcessedDataAction::run()
{
	//clear mesh data
	Data<unsigned short>::getInstance()->mesh_points().clear();
	Data<unsigned short>::getInstance()->mesh_indices().clear();

	//load obj file 
	std::cerr << "Load mesh file " << m_folder + OS_SLASH + "surface.obj" << std::endl;
	std::ifstream  myfile;
	myfile.open(m_folder + OS_SLASH + "surface.obj", std::ios::in);
	std::string line;

	while (std::getline(myfile, line))
	{
		std::istringstream iss(line);
		std::vector<std::string> result;
		for (std::string s; iss >> s;)
			result.push_back(s);
		if (result.size() == 4){
			if (result[0] == "v")
			{
				conf::Point pt;
				pt.x = std::stof(result[1]);
				pt.y = std::stof(result[2]);
				pt.z = std::stof(result[3]);
				Data<unsigned short>::getInstance()->mesh_points().push_back(pt);
			}
			else if (result[0] == "f")
			{
				conf::TriangleIndices ti;
				ti.v1 = std::stoi(result[1]) - 1;
				ti.v2 = std::stoi(result[2]) - 1;
				ti.v3 = std::stoi(result[3]) - 1;
				Data<unsigned short>::getInstance()->mesh_indices().push_back(ti);
			}
		}
	}
	myfile.close();
	std::cerr << "Load mesh done" << std::endl;

	//clear distancemap
	Data<unsigned short>::getInstance()->distancemap().clear();

	//read files
	int y_scale = Data<unsigned short>::getInstance()->volume()->get_height();
	int x_scale = Data<unsigned short>::getInstance()->volume()->get_width();
	int z_scale = Data<unsigned short>::getInstance()->volume()->get_depth();

	double * data_array = new double[x_scale * y_scale];
	char * data_ptr = (char*) &data_array[0];
	int y, x;
	double max_distance = 0;
	std::vector<double> max_center(3, 0);
	for (int i = 0; i < z_scale; i++)
	{
		std::cerr << "Load distancemap file " << m_folder + OS_SLASH + "distancemap" + ZeroPadNumber(i, 5) << std::endl;
		//read input
		std::ifstream  image_file;
		image_file.open(m_folder + OS_SLASH + "distancemap" + ZeroPadNumber(i, 5), std::ios::out | std::ios::binary);
		image_file.seekg(0, std::ios::beg);
		image_file.read(data_ptr, x_scale*y_scale*sizeof(double));
		image_file.close();

		//set data to opencv mat
		cv::Mat distance_slice = cv::Mat(y_scale, x_scale, CV_64F);
		for (y = 0; y < y_scale; y++)
		{
			for (x = 0; x < x_scale; x++)
			{
				double val = data_array[x + y*x_scale];
				if (val > max_distance){
					max_distance = val;
					max_center[0] = (x - 1) * Data<unsigned short>::getInstance()->volume()->get_x_scale();
					max_center[1] = (y - 1) * Data<unsigned short>::getInstance()->volume()->get_y_scale();
					max_center[2] = (i) * Data<unsigned short>::getInstance()->volume()->get_z_scale();
				}
				distance_slice.at<double>(y, x) = val;
			}
		}
		Data<unsigned short>::getInstance()->distancemap().push_back(distance_slice);
	}
	delete[] data_array;
	Data<unsigned short>::getInstance()->setModel(max_center, max_distance, false);
	std::cerr << "Maximum Distance " << max_distance << std::endl;

	Data<unsigned short>::getInstance()->results().clear();
#ifdef WITH_UI
	Data<unsigned short>::getInstance()->emitter()->mesh_updated();
#endif
}

std::string LoadProcessedDataAction::ZeroPadNumber(int num, int length)
{
	std::ostringstream ss;
	ss << std::setfill('0') << std::setw(length) << num;
	return ss.str();
}

