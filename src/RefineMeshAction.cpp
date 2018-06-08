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
///\file RefineMeshAction.cpp
///\author Benjamin Knorlein
///\date 5/4/2018

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "RefineMeshAction.h"
#include "Data.h"
# define M_PI           3.14159265358979323846  /* pi */


using namespace conf;

RefineMeshAction::RefineMeshAction()
{

}

void RefineMeshAction::filterData(std::vector<double> & data_x_in, std::vector<double> & data_y_in, std::vector<double> & data_x_out, std::vector<double> & data_y_out)
{
#define kernel_size 15
	double kernel[kernel_size * 2 + 1];
	double sigma = 5;
	for (int i = -kernel_size; i <= kernel_size; i++)
	{
		kernel[i + kernel_size] = -i / (sigma * sigma * sigma * sqrt(2 * M_PI)) * exp(-(i*i) / (2 * sigma*sigma));
	}

	//pad start
	for (int i = 0; i < kernel_size; i++)
	{
		data_y_out.push_back(0);
		data_x_out.push_back(data_x_in[i]);
	}

	for (int i = kernel_size; i < data_y_in.size() - kernel_size; i++)
	{
		double out = 0;
		for (int j = -kernel_size; j <= kernel_size; j++)
		{
			out += kernel[j + kernel_size] * data_y_in[i + j];
		}
		data_y_out.push_back(out);
		data_x_out.push_back(data_x_in[i]);
	}
}


void RefineMeshAction::run()
{
	for (int i = 0; i <Data<unsigned short>::getInstance()->mesh_points().size(); i++)
	{
		int steps = 100;

		Point center;
		center.x = Data<unsigned short>::getInstance()->center()[0];
		center.y = Data<unsigned short>::getInstance()->center()[1];
		center.z = Data<unsigned short>::getInstance()->center()[2];

		Point point = Data<unsigned short>::getInstance()->mesh_points()[i];
		Point dir = point - center;
		double norm = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
		dir = 1 / norm * dir;
		double stepsize = norm / steps;

		std::vector<double> data_y, data_x;

		Point p;
		for (int d = 0; d < steps *2; d++)
		{
			p = center + d * stepsize* dir;
			data_x.push_back(d* stepsize);
			data_y.push_back(Data<unsigned short>::getInstance()->volume()->getInterpolated(p.x, p.y, p.z, 1));
		}
		std::vector<double> data_y3, data_x3;
		filterData(data_x, data_y, data_x3, data_y3);

		std::vector<double> data_y2, data_x2;
		filterData(data_x3, data_y3, data_x2, data_y2);

		bool after_change = false;
		for (int j = 1; j < data_y2.size(); j++)
		{
			if (data_x2[j] > norm * 0.9 && data_y2[j - 1] < 0 && data_y2[j] > 0)
			{
				//Data<unsigned short>::getInstance()->mesh_points()[i] = center + 0.5f *(data_x2[j] + data_x2[j - 1]) * dir;
				after_change = true;
			}
			if (after_change && data_y2[j] < data_y2[j-1])
			{
				if (data_y[j] > 4000){
					after_change = false;
				}
				else{
					Data<unsigned short>::getInstance()->mesh_points()[i] = center + 0.5f *(data_x2[j] + data_x2[j - 1]) * dir;
					break;
				}
			}
		}
		/*for (int j = 1; j < data_y.size(); j++)
		{
			if (data_x[j] > norm * 0.9 && data_y[j] < 4000)
			{
				Data<unsigned short>::getInstance()->mesh_points()[i] = center + (data_x[j]) * dir;
				break;
			}
		}*/
	}
#ifdef WITH_UI
	emit Data<unsigned short>::getInstance()->emitter()->mesh_updated();
	emit Data<unsigned short>::getInstance()->emitter()->updateSliceView();
#endif
}
