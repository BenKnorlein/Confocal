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
///\file ConfocalSliceViewWidget.cpp
///\author Benjamin Knorlein
///\date 3/15/2018

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "ConfocalSliceViewWidget.h"
#include "QMouseEvent"
#include <iostream>
#include "Data.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace conf;

ConfocalSliceViewWidget::ConfocalSliceViewWidget(QWidget* parent, Qt::WindowFlags flags) :QLabel(parent, flags), width{ 10 }, height{ 10 }, currentFrame(-1)
{
	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(texture_updated()), this, SLOT(init()));
	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(updateSliceView()), this, SLOT(redraw()));
	
	setMinimumSize(1, 1);
	setAlignment(Qt::AlignCenter);
}

ConfocalSliceViewWidget::~ConfocalSliceViewWidget()
{

	
}

void ConfocalSliceViewWidget::init()
{
	set_currentFrame(0);
}

void ConfocalSliceViewWidget::redraw()
{
	if (!m_image16.empty()){
		m_image16.convertTo(m_image8, CV_8U, 0.00390625);
		cvtColor(m_image8, m_image8, cv::COLOR_BGR2RGB);

		drawCenter(m_image8, cv::Scalar(255, 255, 255));
		drawCircle(m_image8, cv::Scalar(255, 255, 255));
		drawMesh(m_image8, cv::Scalar(255, 255, 0));
		//drawmask();

		m_image = QImage((const uchar *)m_image8.data, m_image8.cols, m_image8.rows, m_image8.step, QImage::Format_RGB888);

		setPixmap(QPixmap::fromImage(m_image).scaled(size().width(), size().height(), Qt::KeepAspectRatio));
	}
}

void ConfocalSliceViewWidget::set_currentFrame(int i)
{
	if (Data<unsigned short>::getInstance()->image_g().size() == 0)
		return;

	currentFrame = i;
	if (currentFrame < 0) currentFrame = 0;
	if (currentFrame >= Data<unsigned short>::getInstance()->volume()->get_depth())
		currentFrame = Data<unsigned short>::getInstance()->volume()->get_depth() - 1;

	cv::Mat B;
	
	if (Data<unsigned short>::getInstance()->distancemap().empty()){
		B = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[currentFrame].rows, Data<unsigned short>::getInstance()->image_g()[currentFrame].cols, CV_16UC1);
	}
	else
	{
		Data<unsigned short>::getInstance()->distancemap()[currentFrame].convertTo(B, CV_16U, 255.0f * 255.0f / Data<unsigned short>::getInstance()->max_distance());
	}
	std::vector<cv::Mat> array_to_merge;

	array_to_merge.push_back(B);
	array_to_merge.push_back(Data<unsigned short>::getInstance()->image_g()[currentFrame]);
	array_to_merge.push_back(Data<unsigned short>::getInstance()->image_r()[currentFrame]);
	
	cv::merge(array_to_merge, m_image16);
	
	redraw();
}

void ConfocalSliceViewWidget::resizeEvent(QResizeEvent* event)
{
	redraw();
}

void ConfocalSliceViewWidget::drawmask()
{
	//setup tmp variables
	std::vector<std::vector<cv::Point> > contours_tmp;
	std::vector<cv::Vec4i> hierarchy_tmp;
	cv::Mat tmp = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[currentFrame].rows, Data<unsigned short>::getInstance()->image_g()[currentFrame].cols, CV_8UC1);

	//create Circle Mask
	cv::Mat mask_circle = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[currentFrame].rows, Data<unsigned short>::getInstance()->image_g()[currentFrame].cols, CV_8UC1);
	drawCircle(tmp, cv::Scalar(255));
	cv::findContours(tmp, contours_tmp, hierarchy_tmp, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	drawContours(mask_circle, contours_tmp, -1, cv::Scalar(2), CV_FILLED);
	
	//clear tmp variables
	contours_tmp.clear();
	hierarchy_tmp.clear();
	tmp = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[currentFrame].rows, Data<unsigned short>::getInstance()->image_g()[currentFrame].cols, CV_8UC1);
	
	//create Mesh Mask
	cv::Mat mask_mesh = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[currentFrame].rows, Data<unsigned short>::getInstance()->image_g()[currentFrame].cols, CV_8UC1);
	drawMesh(tmp, cv::Scalar(255));
	cv::findContours(tmp, contours_tmp, hierarchy_tmp, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	drawContours(mask_mesh, contours_tmp, -1, cv::Scalar(2), CV_FILLED);

	//generate 8UC3 from input image
	cv::Mat input_tmp;
	Data<unsigned short>::getInstance()->image_g()[currentFrame].convertTo(input_tmp, CV_8UC1, 1.0f / 256.0f);
	cv::Mat input;
	cv::cvtColor(input_tmp, input, CV_GRAY2BGR);

	bool fg_Set = false;
	cv::Mat mask = cv::Mat::zeros(Data<unsigned short>::getInstance()->image_g()[currentFrame].rows, Data<unsigned short>::getInstance()->image_g()[currentFrame].cols, CV_8UC1);
	for (int i = 0; i < mask.cols; i++){
		for (int j = 0; j < mask.rows; j++){
			if (mask_circle.at<unsigned char>(j, i) > 0)
			{
				mask.at<unsigned char>(j, i) = cv::GC_FGD;
				fg_Set = true;
			}
			else if (mask_circle.at<unsigned char>(j, i) > 0)
			{
				mask.at<unsigned char>(j, i) = cv::GC_PR_FGD;
				fg_Set = true;
			} 
			else
			{
				mask.at<unsigned char>(j, i) = cv::GC_PR_BGD;
			}
		}
	}
	//set borders to background
	for (int i = 0; i < mask.cols; i++){
		mask.at<unsigned char>(0, i) = cv::GC_BGD;
		mask.at<unsigned char>(mask.rows - 1, i) = cv::GC_BGD;
	}
	for (int i = 0; i < mask.rows; i++){
		mask.at<unsigned char>(i, 0) = cv::GC_BGD;
		mask.at<unsigned char>(i, mask.cols - 1) = cv::GC_BGD;
	}
	cv::imwrite("mask.tif", mask);
	if (fg_Set){
		cv::Mat bgModel, fgModel; // the models (internally used)
		cv::Rect rect = cv::Rect(0, 0, input.cols, input.rows);
		cv::grabCut(input, mask, rect, bgModel, fgModel, 100, cv::GC_INIT_WITH_MASK);


		contours_tmp.clear();
		cv::Mat bw = (mask & 1);
		cv::findContours(bw, contours_tmp, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		drawContours(m_image8, contours_tmp, -1, cv::Scalar(255, 255, 255), 1);
	}

	/*
	//Watershed 
	cv::Mat mask2;
	mask.convertTo(mask2, CV_32SC1);
	cv::Mat input_tmp;
	Data<unsigned short>::getInstance()->image_g()[currentFrame].convertTo(input_tmp, CV_8UC1, 1.0f / 256.0f);
	cv::Mat input;
	cv::cvtColor(input_tmp, input, CV_GRAY2BGR);

	cv::imwrite("inout.tif", input);
	cv::imwrite("mask2.tif", mask2);

	for (int i = 0; i < mask2.cols; i++){
		mask2.at<unsigned short>(0,i)= 1;
		mask2.at<unsigned short>(mask2.rows -1,i) = 1;
	}
	for (int i = 0; i < mask2.rows; i++){
		mask2.at<unsigned short>(i, 0) = 1;
		mask2.at<unsigned short>(i, mask2.cols - 1) = 1;
	}

	cv::watershed(input, mask2);

	
	for (int i = 0; i < mask2.cols; i++){
		for (int j = 0; j < mask2.rows; j++){
			if (mask2.at<unsigned short>(i, j) != 2){
				mask2.at<unsigned short>(i, j) = 0;
			} else
			{
				mask2.at<unsigned short>(i, j) = 255;
			}
		}
	}
	
	cv::imwrite("mask2_watershed.tif", mask2);

	std::vector<std::vector<cv::Point> > contours2;
	std::vector<cv::Vec4i> hierarchy2;
	cv::Mat B2;
	mask2.convertTo(B2, CV_8UC1);
	cv::findContours(B2, contours2, hierarchy2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	drawContours(m_image8, contours2, -1, cv::Scalar(255, 255, 255), 1);*/

}


void ConfocalSliceViewWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		set_currentFrame(currentFrame + 1);
	}
	if (e->buttons() & Qt::RightButton)	{
		set_currentFrame(currentFrame - 1);
	}
}

void ConfocalSliceViewWidget::drawCenter(cv::Mat & image, cv::Scalar color)
{
	if (Data<unsigned short>::getInstance()->center().size() >= 2){
		
		double x = Data<unsigned short>::getInstance()->center()[0] / Data<unsigned short>::getInstance()->volume()->get_x_scale();
		double y = Data<unsigned short>::getInstance()->center()[1] / Data<unsigned short>::getInstance()->volume()->get_y_scale();

	
		cv::line(image, cv::Point(x - 5, y), cv::Point(x + 5, y), color);
		cv::line(image, cv::Point(x, y - 5), cv::Point(x, y + 5), color);
	}
}

void ConfocalSliceViewWidget::drawCircle(cv::Mat & image, cv::Scalar color)
{
	if (Data<unsigned short>::getInstance()->center().size() >= 3){
		double cx = Data<unsigned short>::getInstance()->center()[0] / Data<unsigned short>::getInstance()->volume()->get_x_scale();
		double cy = Data<unsigned short>::getInstance()->center()[1] / Data<unsigned short>::getInstance()->volume()->get_y_scale();

		double d = Data<unsigned short>::getInstance()->center()[2] - currentFrame * Data<unsigned short>::getInstance()->volume()->get_z_scale();
		d = fabs(d);

		if (d <  Data<unsigned short>::getInstance()->max_distance()){
			double r = d / tan(asin(d / Data<unsigned short>::getInstance()->max_distance()));

			//Convert to pixel. We assume x and y have the same scaling. Otherwise we need to draw an ellipsoid
			r = r / Data<unsigned short>::getInstance()->volume()->get_x_scale();

			cv::circle(image, cv::Point(cx, cy), r, color);
		}
	}
}

void ConfocalSliceViewWidget::drawMesh(cv::Mat & image, cv::Scalar color)
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
			d = (currentFrame * Data<unsigned short>::getInstance()->volume()->get_z_scale() - pts[x1].z) / (pts[x2].z - pts[x1].z);

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