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
///\file LoadDataAction.cpp
///\author Benjamin Knorlein
///\date 06/08/2018

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

#include "SaveProcessedDataAction.h"
#include "Data.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace conf;

SaveProcessedDataAction::SaveProcessedDataAction(std::string folder) : m_folder(folder)
{

}

std::string SaveProcessedDataAction::ZeroPadNumber(int num, int length)
{
	std::ostringstream ss;
	ss << std::setfill('0') << std::setw(length) << num;
	return ss.str();
}

void SaveProcessedDataAction::run()
{
	std::cerr << "Save to " << m_folder.c_str();
#ifdef _MSC_VER
	CreateDirectory(m_folder.c_str(), NULL);
#else
	system(("mkdir -p " + m_folder).c_str());
#endif
	//save mesh as obj
	std::ofstream myfile;
	myfile.open(m_folder + OS_SLASH + "surface.obj", std::ios::out);
	for (auto &pt :  Data<unsigned short>::getInstance()->mesh_points())
	{
		myfile << "v " << pt.x << " " << pt.y << " " << pt.z << std::endl;
	}
	myfile << std::endl;
	for (auto &f : Data<unsigned short>::getInstance()->mesh_indices())
	{
		myfile << "f " << f.v1 + 1 << " " << f.v2 + 1 << " " << f.v3 + 1 << std::endl;
	}
	myfile.close();

	
	//save distance map as binary images
	int y_scale = Data<unsigned short>::getInstance()->volume()->get_height();
	int x_scale = Data<unsigned short>::getInstance()->volume()->get_width();
	int z_scale = Data<unsigned short>::getInstance()->volume()->get_depth();

	double * data_array = new double[x_scale * y_scale];
	char * data_ptr = (char*)&data_array[0];
	int y, x;
	for (int i = 0; i < Data<unsigned short>::getInstance()->distancemap().size(); i++)
	{
		std::ofstream image_file;

		std::cerr << "Saving map " << i << " to " << m_folder + OS_SLASH + "distancemap" + ZeroPadNumber(i, 5) << std::endl;
		image_file.open(m_folder + OS_SLASH + "distancemap" + ZeroPadNumber(i, 5), std::ios::out | std::ios::binary);

		for (y = 0; y < y_scale; y++)
		{
			for (x = 0; x < x_scale; x++)
			{
				data_array[x + y*x_scale] = Data<unsigned short>::getInstance()->distancemap()[i].at<double>(y, x);
			}
		}

		image_file.write(&data_ptr[0], sizeof(double) * x_scale*y_scale);

		myfile.close();
	}

	std::ofstream report_file;
	// Red (pixel normalized) - Red (region normalized)- Red - Green - NbPixel - sd Red (normalized) - sd Red - sd Green
	report_file.open(m_folder + OS_SLASH + "results.csv", std::ios::out);
	report_file << "Center , " << Data<unsigned short>::getInstance()->center()[0] << " , " << Data<unsigned short>::getInstance()->center()[1] << " , " << Data<unsigned short>::getInstance()->center()[2] << std::endl;
	report_file << "Maximum Distance , " << Data<unsigned short>::getInstance()->max_distance() << std::endl << std::endl;
	report_file << "Analysis" << std::endl;
	report_file << "Region , Red (pixel normalized) , Red (region normalized) , Red , Green , NbPixel , sd Red (normalized) , sd Red , sd Green" << std::endl;
	for (int i = 0; i < Data<unsigned short>::getInstance()->results().size(); i++)
	{
		report_file << "Region " << i << " (" << i*Data<unsigned short>::getInstance()->region_size() << " - " << (i+1)*Data<unsigned short>::getInstance()->region_size() << ") , ";
		for (int j = 0; j < 7; j++)
			report_file << Data<unsigned short>::getInstance()->results()[i][j] << " , ";
		report_file << Data<unsigned short>::getInstance()->results()[i][7] << std::endl;
	}
	report_file.close();
}
