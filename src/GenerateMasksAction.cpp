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
///\file GenerateMasksAction.cpp
///\author Benjamin Knorlein
///\date 5/7/2018

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "GenerateMasksAction.h"
#include <vector>
#include <opencv2/core/mat.hpp>
#include "Data.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "matlab/libIso.h"
using namespace conf;

#ifdef _WIN32
#define OS_SEP "\\"
#else
#define OS_SEP "/"
#endif

extern bool m_Initialized_MCR;

GenerateMasksAction::GenerateMasksAction() : m_Initialized_ISO(false)
{
	// Call application and library initialization. Perform this 
	// initialization before calling any API functions or
	// Compiler SDK-generated libraries.
	if (!m_Initialized_MCR){
		if (!mclInitializeApplication(nullptr, 0))
		{
			std::cerr << "Could not initialize Matlab runtime properly" << std::endl;
		}
		else
		{
			std::cerr << "Matlab runtime initialized" << std::endl;
			m_Initialized_MCR = true;
		}
	}
	if (m_Initialized_MCR)
	{
		if (!libIsoInitialize())
		{
			std::cerr << "Could not initialize libIso properly"	<< std::endl;
		} 
		else
		{
			std::cerr << "libIso initialized" << std::endl;
			m_Initialized_ISO = true;
		}
	}
}

GenerateMasksAction::~GenerateMasksAction()
{
	if (m_Initialized_ISO)
	{
		//libIsoTerminate();
		//std::cerr << "libIso terminated" << std::endl;
	}
}

void GenerateMasksAction::drawCircle(int slice, cv::Mat & image, cv::Scalar color)
{
	if (Data<unsigned short>::getInstance()->center().size() >= 3){
		double cx = Data<unsigned short>::getInstance()->center()[0] / Data<unsigned short>::getInstance()->volume()->get_x_scale();
		double cy = Data<unsigned short>::getInstance()->center()[1] / Data<unsigned short>::getInstance()->volume()->get_y_scale();

		double d = Data<unsigned short>::getInstance()->center()[2] - slice * Data<unsigned short>::getInstance()->volume()->get_z_scale();
		d = fabs(d);

		if (d <  Data<unsigned short>::getInstance()->max_distance()){
			double r = d / tan(asin(d / Data<unsigned short>::getInstance()->max_distance()));

			//Convert to pixel. We assume x and y have the same scaling. Otherwise we need to draw an ellipsoid
			r = r / Data<unsigned short>::getInstance()->volume()->get_x_scale();

			cv::circle(image, cv::Point(cx, cy), r, color);
		}
	}
}

void GenerateMasksAction::drawMesh(int slice, cv::Mat & image, cv::Scalar color)
{
	int tri = Data<unsigned short>::getInstance()->mesh_indices().size();

	Point pts[3];
	int pt_count;
	double pt2d[2][2];
	double d;
	for (int i = 0; i < tri; i++)
	{
		pt_count = 0;

		pts[0] = Data<unsigned short>::getInstance()->mesh_points()[Data<unsigned short>::getInstance()->mesh_indices()[i].v1];
		pts[1] = Data<unsigned short>::getInstance()->mesh_points()[Data<unsigned short>::getInstance()->mesh_indices()[i].v2];
		pts[2] = Data<unsigned short>::getInstance()->mesh_points()[Data<unsigned short>::getInstance()->mesh_indices()[i].v3];

		for (int x1 = 0; x1 < 3; x1++){
			int x2 = (x1 + 1 > 2) ? 0 : x1 + 1;
			d = (slice * Data<unsigned short>::getInstance()->volume()->get_z_scale() - pts[x1].z) / (pts[x2].z - pts[x1].z);

			if (d >= 0 && d <= 1)
			{

				pt2d[pt_count][0] = pts[x1].x + (pts[x2].x - pts[x1].x) * d;
				pt2d[pt_count][1] = pts[x1].y + (pts[x2].y - pts[x1].y) * d;
				pt_count++;
			}
		}
		if (pt_count == 2)
		{
			double scale = Data<unsigned short>::getInstance()->volume()->get_x_scale();
			cv::line(image, cv::Point(pt2d[0][0] / scale, pt2d[0][1] / scale), cv::Point(pt2d[1][0] / scale, pt2d[1][1] / scale), color);
		}
	}
}

void GenerateMasksAction::run()
{
	if (m_Initialized_ISO)
	{
		const mwSize msize[3] = {Data<unsigned short>::getInstance()->image_g()[0].cols
			, Data<unsigned short>::getInstance()->image_g()[0].rows
			, Data<unsigned short>::getInstance()->image_g().size() };

		unsigned int imageSize = Data<unsigned short>::getInstance()->image_g()[0].cols * Data<unsigned short>::getInstance()->image_g()[0].rows;
		unsigned int rows = Data<unsigned short>::getInstance()->image_g()[0].rows;

		mwArray images = mwArray(3, msize, mxUINT16_CLASS);
		mwArray mask = mwArray(3, msize, mxUINT8_CLASS);
		
		mxUint16 * data = new mxUint16[msize[0] * msize[1] * msize[2]];
		mxUint8 * data_mask = new mxUint8[msize[0] * msize[1] * msize[2]];

		for (unsigned int z = 0; z < msize[2]; z++)
		{
			cv::Mat mask_circle = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[z].rows, Data<unsigned short>::getInstance()->image_g()[z].cols, CV_8UC1);

			{
				std::vector<std::vector<cv::Point> > contours_tmp;
				std::vector<cv::Vec4i> hierarchy_tmp;
				cv::Mat tmp = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[z].rows, Data<unsigned short>::getInstance()->image_g()[z].cols, CV_8UC1);

				drawCircle(z, tmp, cv::Scalar(255));
				cv::findContours(tmp, contours_tmp, hierarchy_tmp, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
				drawContours(mask_circle, contours_tmp, -1, cv::Scalar(255), CV_FILLED);
			}

			for (unsigned int x = 0; x < msize[1]; x++)
			{
				for (unsigned int y = 0; y < msize[0]; y++)
				{
					data[z*imageSize + y + x*rows] = Data<unsigned short>::getInstance()->image_g()[z].at<unsigned short>(y,x);
					data_mask[z*imageSize + y + x*rows] = mask_circle.at<unsigned char>(y, x);
				}
			}
		}
			
		images.SetData(data, msize[0] * msize[1] * msize[2]);
		mask.SetData(data_mask, msize[0] * msize[1] * msize[2]);

		delete[] data;
		delete[] data_mask;

		
		//parameter
		mwArray params(1, 3, mxINT32_CLASS);
		int params_data[3] = { 300, 0 ,18};
		params.SetData(params_data,3);
		
		mwArray aspect(1, 3, mxDOUBLE_CLASS);
		double aspect_data[3] = { Data<unsigned short>::getInstance()->volume()->get_x_scale(),
								Data<unsigned short>::getInstance()->volume()->get_y_scale(),
								Data<unsigned short>::getInstance()->volume()->get_z_scale() };
		aspect.SetData(aspect_data, 3);

		//output
		int n_out = 2;
		mwArray mesh;
		mwArray distancemap;

		//run8
		std::cerr << "Start Processing" << std::endl;
		processActiveContour(n_out, mesh, distancemap, images, mask, params, aspect);

		mwArray vertices_size = mesh.Get("vertices", 1, 1).GetDimensions();
		std::cerr << "Vertices " << (int) vertices_size.Get(1, 1) << std::endl;
		Data<unsigned short>::getInstance()->mesh_points().clear();
		for (int pt = 1; pt <= (int) vertices_size.Get(1, 1); pt++)
		{
			Point p;
			p.x = mesh.Get("vertices", 1, 1).Get(2, pt, 1);
			p.y = mesh.Get("vertices", 1, 1).Get(2, pt, 2);
			p.z = mesh.Get("vertices", 1, 1).Get(2, pt, 3);

			p.x = (p.x - 1) * Data<unsigned short>::getInstance()->volume()->get_x_scale();
			p.y = (p.y - 1) * Data<unsigned short>::getInstance()->volume()->get_y_scale();
			p.z = (p.z - 1) * Data<unsigned short>::getInstance()->volume()->get_z_scale();

			Data<unsigned short>::getInstance()->mesh_points().push_back(p);
		}
		
		mwArray mesh_size = mesh.Get("faces", 1, 1).GetDimensions();
		std::cerr << "Faces " << (int) mesh_size.Get(1, 1) << std::endl;
		Data<unsigned short>::getInstance()->mesh_indices().clear();
		Data<unsigned short>::getInstance()->mesh_indices().reserve((int)mesh_size.Get(1, 1));
		for (int ind = 1; ind <= (int) mesh_size.Get(1, 1); ind++)
		{
			TriangleIndices triangle;
			triangle.v1 = ((int) mesh.Get("faces", 1, 1).Get(2, ind, 1)) - 1;
			triangle.v2 = ((int) mesh.Get("faces", 1, 1).Get(2, ind, 2)) - 1;
			triangle.v3 = ((int) mesh.Get("faces", 1, 1).Get(2, ind, 3)) - 1;
			Data<unsigned short>::getInstance()->mesh_indices().push_back(triangle);
		}
		int y_scale = distancemap.GetDimensions().Get(1, 1);
		int x_scale = distancemap.GetDimensions().Get(1, 2);
		int z_scale = distancemap.GetDimensions().Get(1, 3);

		std::cerr << "Set distancemap - Size : " << x_scale << " , " << y_scale << " , " << z_scale << std::endl;

		mxDouble * data_distancemap = new mxDouble[x_scale * y_scale * z_scale];
		distancemap.GetData(data_distancemap, x_scale * y_scale * z_scale);
		imageSize = x_scale * y_scale;

		Data<unsigned short>::getInstance()->distancemap().clear();
		double max_distance = 0;
		std::vector<double> max_center(3,0);
		int y, x;
		for (int z = 0; z < z_scale; z++)
		{
			cv::Mat distance_slice = cv::Mat(y_scale, x_scale, CV_64F);
			
			for (x = 0; x < x_scale; x++)
			{
				for (y = 0; y < y_scale; y++)
				{
					double val = data_distancemap[z*imageSize + y + x*y_scale];
					if (val > max_distance){
						max_distance = val;
						max_center[0] = (x - 1) * Data<unsigned short>::getInstance()->volume()->get_x_scale();
						max_center[1] = (y - 1) * Data<unsigned short>::getInstance()->volume()->get_y_scale();
						max_center[2] = (z - 1) * Data<unsigned short>::getInstance()->volume()->get_z_scale();
					}
					distance_slice.at<double>(y, x) = val;
				}
			}
			
			Data<unsigned short>::getInstance()->distancemap().push_back(distance_slice);
		}
		delete[] data_distancemap;
		Data<unsigned short>::getInstance()->setModel(max_center, max_distance,false);
		std::cerr << "Maximum Distance " << max_distance << std::endl;

		Data<unsigned short>::getInstance()->results().clear();
		Data<unsigned short>::getInstance()->emitter()->mesh_updated();
	}




	//for (int im = 0; im < Data<unsigned short>::getInstance()->image_g().size(); im++){
	//	//setup tmp variables
	//	std::vector<std::vector<cv::Point> > contours_tmp;
	//	std::vector<cv::Vec4i> hierarchy_tmp;
	//	cv::Mat tmp = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[im].rows, Data<unsigned short>::getInstance()->image_g()[im].cols, CV_8UC1);

	//	//create Circle Mask
	//	cv::Mat mask_circle = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[im].rows, Data<unsigned short>::getInstance()->image_g()[im].cols, CV_8UC1);
	//	drawCircle(im, tmp, cv::Scalar(255));
	//	cv::findContours(tmp, contours_tmp, hierarchy_tmp, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	//	drawContours(mask_circle, contours_tmp, -1, cv::Scalar(255), CV_FILLED);

	//	//clear tmp variables
	//	contours_tmp.clear();
	//	hierarchy_tmp.clear();
	//	tmp = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[im].rows, Data<unsigned short>::getInstance()->image_g()[im].cols, CV_8UC1);

	//	//create Mesh Mask
	//	cv::Mat mask_mesh = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[im].rows, Data<unsigned short>::getInstance()->image_g()[im].cols, CV_8UC1);
	//	drawMesh(im,tmp, cv::Scalar(255));
	//	cv::findContours(tmp, contours_tmp, hierarchy_tmp, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	//	drawContours(mask_mesh, contours_tmp, -1, cv::Scalar(255), CV_FILLED);

	//	QString filename = QString::fromStdString(m_folder) + OS_SEP + "mask_circle" + QString::number(im).rightJustified(5, '0') + ".tif";
	//	cv::imwrite(filename.toStdString(), mask_circle);

	//	filename = QString::fromStdString(m_folder) + OS_SEP + "mask_mesh" + QString::number(im).rightJustified(5, '0') + ".tif";
	//	cv::imwrite(filename.toStdString(), mask_mesh);

	//	filename = QString::fromStdString(m_folder) + OS_SEP + "image" + QString::number(im).rightJustified(5, '0') + ".tif";
	//	cv::imwrite(filename.toStdString(), Data<unsigned short>::getInstance()->image_g()[im]);

	//}
}
