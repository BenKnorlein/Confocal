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
///\file UpdateResultsAction.cpp
///\author Benjamin Knorlein
///\date 5/10/2018

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "UpdateResultsAction.h"
#include "Data.h"
#include <iostream>

using namespace conf;

void UpdateResultsAction::run()
{
	Data<unsigned short>::getInstance()->set_region_size(m_regions_size);
	m_nb_regions = ceil((Data<unsigned short>::getInstance()->max_distance()  / m_regions_size));
	std::cerr << "UpdateResultsAction for region size " << m_regions_size << " and " << m_nb_regions << " regions" << std::endl;
	if (Data<unsigned short>::getInstance()->distancemap().empty() || m_nb_regions == 0){
		Data<unsigned short>::getInstance()->results().clear();
		return;
	}

	// Red (pixel normalized) - Red (region normalized)- Red - Green - NbPixel - sd Red (normalized) - sd Red - sd Green

	//Prepare out array;

	Data<unsigned short>::getInstance()->results() = std::vector< std::vector< double>>(m_nb_regions, std::vector<double>(8,0));

	//Prepare tmp array;
	std::vector< std::vector< double>> red_n_tmp(m_nb_regions, std::vector<double>());
	std::vector< std::vector< double>> red_tmp(m_nb_regions, std::vector<double>());
	std::vector< std::vector< double>> green_tmp(m_nb_regions, std::vector<double>());

	//compute regionsize
	//std::cerr << "Sort pixel for region size of " << m_regions_size << std::endl;
	double dist, r, g, r_n;
	//get all pixel for all regions
	for (int z = 0; z < Data<unsigned short>::getInstance()->volume()->get_depth(); z++)
	{
		for (int x = 0; x < Data<unsigned short>::getInstance()->volume()->get_width(); x++)
		{
			for (int y = 0; y < Data<unsigned short>::getInstance()->volume()->get_height(); y++)
			{
				dist = Data<unsigned short>::getInstance()->distancemap()[z].at<double>(y, x);
				
				if (dist > 0)
				{
					int region = floor(dist / m_regions_size);
					if (region >= m_nb_regions)
						region = m_nb_regions - 1;
					r = Data<unsigned short>::getInstance()->image_r()[z].at<unsigned short>(y, x);
					g = Data<unsigned short>::getInstance()->image_g()[z].at<unsigned short>(y, x);
					r_n = (g > 0) ? r / g : -1;
					red_tmp[region].push_back(r);
					green_tmp[region].push_back(g);
					red_n_tmp[region].push_back(r_n);
				}
			}
		}
	}

	std::cerr << "Number pixel for Red normalized : ";
	for (int i = 0; i < m_nb_regions; i++)
	{
		std::cerr << red_n_tmp[i].size() << "  ";
	}
	std::cerr << std::endl;
	std::cerr << "Number pixel for Red : ";
	for (int i = 0; i < m_nb_regions; i++)
	{
		std::cerr << red_tmp[i].size() << "  ";
	}
	std::cerr << std::endl;
	std::cerr << "Number pixel for Green : ";
	for (int i = 0; i < m_nb_regions; i++)
	{
		std::cerr << green_tmp[i].size() << "  ";
	}
	std::cerr << std::endl;

	//compute averages and sd
	std::cerr << "Compute averages and sd" << std::endl;
	for (int i = 0; i < Data<unsigned short>::getInstance()->results().size(); i++)
	{
		std::cerr << "Region " << i << " : ";
		//set number of Pixel
		Data<unsigned short>::getInstance()->results()[i][4] = red_tmp[i].size();
		std::cerr << "NbP:" << Data<unsigned short>::getInstance()->results()[i][4] << " , ";

		if (Data<unsigned short>::getInstance()->results()[i][4] > 0){

			//average red pixel normalized
			Data<unsigned short>::getInstance()->results()[i][0] = 0;
			int red_n_not_set = 0;
			for (int j = 0; j < red_tmp[i].size(); j++)
			{
				if (red_n_tmp[i][j] >= 0){
					Data<unsigned short>::getInstance()->results()[i][0] += red_n_tmp[i][j];
				}
				else
				{
					red_n_not_set++;
				}
			}
			Data<unsigned short>::getInstance()->results()[i][0] /= Data<unsigned short>::getInstance()->results()[i][4];
			if (red_n_not_set > 0)
				std::cerr << "Warning : " << red_n_not_set << " points could not be normalized as the normalization factor was zero." << std::endl;

			std::cerr << "R_np:" << Data<unsigned short>::getInstance()->results()[i][0] << " , ";

			//set red
			Data<unsigned short>::getInstance()->results()[i][2] = 0;
			for (int j = 0; j < red_tmp[i].size(); j++)
			{
				Data<unsigned short>::getInstance()->results()[i][2] += red_tmp[i][j];
			}
			Data<unsigned short>::getInstance()->results()[i][2] /= Data<unsigned short>::getInstance()->results()[i][4];
			std::cerr << "R:" << Data<unsigned short>::getInstance()->results()[i][2] << " , ";

			//set green
			Data<unsigned short>::getInstance()->results()[i][3] = 0;
			for (int j = 0; j < red_tmp[i].size(); j++)
			{
				Data<unsigned short>::getInstance()->results()[i][3] += green_tmp[i][j];
			}
			Data<unsigned short>::getInstance()->results()[i][3] /= Data<unsigned short>::getInstance()->results()[i][4];
			std::cerr << "G:" << Data<unsigned short>::getInstance()->results()[i][3] << " , ";

			//set red region normalized
			Data<unsigned short>::getInstance()->results()[i][1] = Data<unsigned short>::getInstance()->results()[i][2] / Data<unsigned short>::getInstance()->results()[i][3];
			std::cerr << "R_nr:" << Data<unsigned short>::getInstance()->results()[i][1] << " , ";

			
			//compute sd - red normalized
			Data<unsigned short>::getInstance()->results()[i][5] = 0;
			for (unsigned int j = 0; j < red_n_tmp[i].size(); j++)
			{
				if (red_n_tmp[i][j] >= 0){
					Data<unsigned short>::getInstance()->results()[i][5] += pow(red_n_tmp[i][j] - Data<unsigned short>::getInstance()->results()[i][0], 2);
				}	
			}
			
			Data<unsigned short>::getInstance()->results()[i][5] = (red_n_tmp[i].size() - red_n_not_set - 1 > 0) ? 
				sqrt(Data<unsigned short>::getInstance()->results()[i][5] / (red_n_tmp[i].size() - red_n_not_set - 1)) 
			: 0;
			std::cerr << "SD_R_n:" << Data<unsigned short>::getInstance()->results()[i][5] << " , ";

			//compute sd - red
			Data<unsigned short>::getInstance()->results()[i][6] = 0;
			for (unsigned int j = 0; j < red_tmp[i].size(); j++)
			{
				Data<unsigned short>::getInstance()->results()[i][6] += pow(red_tmp[i][j] - Data<unsigned short>::getInstance()->results()[i][2], 2);
			}
			Data<unsigned short>::getInstance()->results()[i][6] = (red_tmp[i].size() - 1 > 0) ?
				sqrt(Data<unsigned short>::getInstance()->results()[i][6] / (red_tmp[i].size() - 1))
			: 0;
			std::cerr << "SD_R:" << Data<unsigned short>::getInstance()->results()[i][6] << " , ";

			//compute sd - green
			Data<unsigned short>::getInstance()->results()[i][7] = 0;
			for (unsigned int j = 0; j < green_tmp[i].size(); j++)
			{
				Data<unsigned short>::getInstance()->results()[i][7] += pow(green_tmp[i][j] - Data<unsigned short>::getInstance()->results()[i][3], 2);
			}
			Data<unsigned short>::getInstance()->results()[i][7] = (green_tmp[i].size() - 1 > 0) ?
				sqrt(Data<unsigned short>::getInstance()->results()[i][7] / (green_tmp[i].size() - 1))
				: 0;
			std::cerr << "SD_G:" << Data<unsigned short>::getInstance()->results()[i][7];
		}
		std::cerr << std::endl;
	}
}
