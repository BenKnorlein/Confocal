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
///\file SphereFit.cpp
///\author Benjamin Knorlein
///\date 11/14/2017

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "SphereFit.h"
#include <opencv2/core.hpp>
#include <iostream>
#include "Data.h"
#include "MSAC.h"

using namespace conf;
std::vector<double> SphereFit::fit_function(std::vector<const cv::Point3d*>& points)
{
	std::vector<double> model;

	if (points.size() == 4)
	{
		//   http://steve.hollasch.net/cgindex/geometry/sphere4pts.html
		//	 http://paulbourke.net/geometry/circlesphere/
		cv::Mat X = cv::Mat(4, 4, CV_64FC1, cv::Scalar(1));
		for (int i = 0; i < points.size(); ++i)
		{
			X.at<double>(i, 0) = points[i]->x;
			X.at<double>(i, 1) = points[i]->y;
			X.at<double>(i, 2) = points[i]->z;
		}
		
		double m1 = cv::determinant(X);
		
		if (abs(m1) < std::numeric_limits<double>::epsilon())
			return model;
		

		for (int i = 0; i < points.size(); ++i)
		{
			X.at<double>(i, 0) = points[i]->x * points[i]->x + points[i]->y * points[i]->y + points[i]->z * points[i]->z;
		}
		double m2 = cv::determinant(X);

		for (int i = 0; i < points.size(); ++i)
		{
			X.at<double>(i, 1) = X.at<double>(i, 0);
			X.at<double>(i, 0) = points[i]->x;
		}
		double m3 = cv::determinant(X);

		for (int i = 0; i < points.size(); ++i)
		{
			X.at<double>(i, 2) = X.at<double>(i, 1);
			X.at<double>(i, 1) = points[i]->y;
		}
		double m4 = cv::determinant(X);

		for (int i = 0; i < points.size(); ++i)
		{
			X.at<double>(i, 0) = X.at<double>(i, 2);
			X.at<double>(i, 1) = points[i]->x;
			X.at<double>(i, 2) = points[i]->y;
			X.at<double>(i, 3) = points[i]->z;
		}
		double m5 = cv::determinant(X);

		model.push_back(0.5*m2 / m1);
		model.push_back(0.5*m3 / m1);
		model.push_back(0.5*m4 / m1);
		model.push_back(sqrt(model[0] * model[0] + model[1] * model[1] + model[2] * model[2] - m5 / m1));
	}
	else{

		//https://www.geometrictools.com/Documentation/LeastSquaresFitting.pdf
		//https://www.geometrictools.com/GTEngine/Include/Mathematics/GteApprQuadratic3.h

		cv::Mat A = cv::Mat(5, 5, CV_64FC1, cv::Scalar(0));

		for (int i = 0; i < points.size(); ++i)
		{
			double x = points[i]->x;
			double y = points[i]->y;
			double z = points[i]->z;
			double x2 = x*x;
			double y2 = y*y;
			double z2 = z*z;
			double xy = x*y;
			double xz = x*z;
			double yz = y*z;
			double r2 = x2 + y2 + z2;
			double xr2 = x*r2;
			double yr2 = y*r2;
			double zr2 = z*r2;
			double r4 = r2*r2;

			A.at<double>(0, 1) += x;
			A.at<double>(0, 2) += y;
			A.at<double>(0, 3) += z;
			A.at<double>(0, 4) += r2;
			A.at<double>(1, 1) += x2;
			A.at<double>(1, 2) += xy;
			A.at<double>(1, 3) += xz;
			A.at<double>(1, 4) += xr2;
			A.at<double>(2, 2) += y2;
			A.at<double>(2, 3) += yz;
			A.at<double>(2, 4) += yr2;
			A.at<double>(3, 3) += z2;
			A.at<double>(3, 4) += zr2;
			A.at<double>(4, 4) += r4;
		}

		A.at<double>(0, 0) = static_cast<double>(points.size());

		for (int row = 0; row < 5; ++row)
		{
			for (int col = 0; col < row; ++col)
			{
				A.at<double>(row, col) = A.at<double>(col, row);
			}
		}

		double invNumPoints = (1.0f) / A.at<double>(0, 0);
		for (int row = 0; row < 5; ++row)
		{
			for (int col = 0; col < 5; ++col)
			{
				A.at<double>(row, col) *= invNumPoints;
			}
		}

		cv::Mat eigenvectors;
		cv::Mat eigenvalues;

		cv::eigen(A, eigenvalues, eigenvectors);

		if (fabs(eigenvectors.at<double>(4, 4)) > std::numeric_limits<double>::epsilon()){
			double inv = (1.0f) / eigenvectors.at<double>(4, 4);  // TODO: Guard against zero divide?
			double coefficients[4];
			for (int row = 0; row < 4; ++row)
			{
				coefficients[row] = inv * eigenvectors.at<double>(4, row);
			}

			model.push_back(-0.5 * coefficients[1]);
			model.push_back(-0.5 * coefficients[2]);
			model.push_back(-0.5 * coefficients[3]);
			model.push_back(sqrt(std::abs(model[0] * model[0] + model[1] * model[1] + model[2] * model[2] - coefficients[0])));
		}
	}
	return model;
}

std::vector<double> SphereFit::eval_function(std::vector<double> model, std::vector<const cv::Point3d>& points)
{
	std::vector<double> error;
	for (auto pt : points)
	{
		double e = fabs(sqrt((pt.x - model[0])*(pt.x - model[0]) +
			(pt.y - model[1])*(pt.y - model[1]) +
			(pt.z - model[2])*(pt.z - model[2])) - model[3]);
		error.push_back(e);
	}

	return error;
}

bool SphereFit::check_function(std::vector<double> model)
{
	return (model.size() == 4);
}

void SphereFit::run()
{
	std::vector <const cv::Point3d> pts;
	for (int x = 0; x < Data<unsigned short>::getInstance()->volume()->get_width(); x++)
	{
		for (int y = 0; y < Data<unsigned short>::getInstance()->volume()->get_height(); y++)
		{
			for (int z = 0; z < Data<unsigned short>::getInstance()->volume()->get_depth(); z++)
			{
				if (*Data<unsigned short>::getInstance()->volume()->get(x, y, z, 1) > 65536 * 0.1) //TODO Threshold variable
				{
					pts.push_back(cv::Point3d(x *Data<unsigned short>::getInstance()->volume()->get_x_scale(),
						y * Data<unsigned short>::getInstance()->volume()->get_y_scale(),
						z * Data<unsigned short>::getInstance()->volume()->get_z_scale()));
				}
			}
		}
	}

	auto fit_function = std::bind(&conf::SphereFit::fit_function, this, std::placeholders::_1);
	auto eval_function = std::bind(&conf::SphereFit::eval_function, this, std::placeholders::_1, std::placeholders::_2);
	auto check_function = std::bind(&conf::SphereFit::check_function, this, std::placeholders::_1);

	conf::MSAC msac;
	Data<unsigned short>::getInstance()->setModel(msac.run(getMinSamples(), pts, fit_function, eval_function, check_function, 4, 1000, 99));
	std::cerr << Data<unsigned short>::getInstance()->model()[0] << " " << Data<unsigned short>::getInstance()->model()[1] << " " << Data<unsigned short>::getInstance()->model()[2] << " " << Data<unsigned short>::getInstance()->model()[3] << std::endl;
}