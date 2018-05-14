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
///\file Plotwindow.cpp
///\author Benjamin Knorlein
///\date 3/15/2018

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Plotwindow.h"
#include "ui_PlotWindow.h"
#include "ConfocalMainWindow.h"
#include "Data.h"
#include <iostream>
#include "UpdateResultsAction.h"

using namespace conf;

#define USE_INTERPOLATED

PlotWindow::PlotWindow(QWidget* parent) : QDockWidget(parent), dock(new Ui::PlotWindow), channel(0), updating(false), values(NULL)
{
	dock->setupUi(this);
	frameMarker = new QCPItemLine(dock->plotWidget);
	values = new QCPBars(dock->plotWidget->xAxis, dock->plotWidget->yAxis);

	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(mesh_updated()), this, SLOT(init()));	

	installEventFilterToChildren(this);

	dock->frame_Results->setVisible(false);
}

void PlotWindow::installEventFilterToChildren(QObject* object)
{
	QObjectList list = object->children();
	for (int i = 0; i < list.size(); i++)
	{
		installEventFilterToChildren(list.at(i));
	}

	object->installEventFilter(this);
}

bool PlotWindow::eventFilter(QObject* target, QEvent* event)
{
	if (target == dock->plotWidget)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* _mouseEvent = static_cast<QMouseEvent*>(event);
			if (_mouseEvent->buttons() == Qt::LeftButton)
			{
				if (dock->plotWidget->xAxis != NULL){
					double x = dock->plotWidget->xAxis->pixelToCoord(_mouseEvent->pos().x());

					Point center;
					center.x = Data<unsigned short>::getInstance()->center()[0];
					center.y = Data<unsigned short>::getInstance()->center()[1];
					center.z = Data<unsigned short>::getInstance()->center()[2];

					Point point = Data<unsigned short>::getInstance()->mesh_points()[Data<unsigned short>::getInstance()->active_point()];
					Point dir = point - center;
					double norm = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
					dir = 1 / norm * dir;

					Data<unsigned short>::getInstance()->mesh_points()[Data<unsigned short>::getInstance()->active_point()] = center + x * dir;

					plot();
					emit Data<unsigned short>::getInstance()->emitter()->mesh_updated();
					emit Data<unsigned short>::getInstance()->emitter()->updateSliceView();
					return true;
				}
			}
		}
	}
	return false;
}

void PlotWindow::plot()
{
	if (dock->comboBox_type->currentIndex() == 0)
	{
		plot_points();
	} 
	else
	{
		plot_results();
	}
}

void PlotWindow::plot_points()
{
	values->setVisible(false);

	if (dock->comboBox_point->count() == 0)
		return;

	int steps = 100;

	Point center;
	center.x = Data<unsigned short>::getInstance()->center()[0];
	center.y = Data<unsigned short>::getInstance()->center()[1];
	center.z = Data<unsigned short>::getInstance()->center()[2];

	Point point = Data<unsigned short>::getInstance()->mesh_points()[Data<unsigned short>::getInstance()->active_point()];
	Point dir = point - center;
	double norm = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	dir = 1 / norm * dir;
	double stepsize = norm / steps;

	QVector<double> data_y, data_x;

	Point p;
	for (int d = 0; d < steps * 2; d++)
	{
		p = center + d * stepsize* dir;
		data_x.push_back(d* stepsize);

#ifdef USE_INTERPOLATED
		if (channel == 2)
		{
			data_y.push_back(Data<unsigned short>::getInstance()->volume()->getInterpolated(p.x, p.y, p.z, 0) /
				Data<unsigned short>::getInstance()->volume()->getInterpolated(p.x, p.y, p.z, 1));
		}
		else{
			data_y.push_back(Data<unsigned short>::getInstance()->volume()->getInterpolated(p.x, p.y, p.z, channel));
		}
#else
		if (channel == 2)
		{
			data_y.push_back(Data<unsigned short>::getInstance()->volume()->getNonInterpolated(round(p.x), round(p.y), round(p.z), 0) /
				Data<unsigned short>::getInstance()->volume()->getNonInterpolated(round(p.x), round(p.y), round(p.z), 1));
		}
		else{
			data_y.push_back(Data<unsigned short>::getInstance()->volume()->getNonInterpolated(round(p.x), round(p.y), round(p.z), channel));
		}
#endif
	}

	dock->plotWidget->clearGraphs();
	dock->plotWidget->yAxis->setLabel("Value");
	dock->plotWidget->xAxis->setLabel("Distance");

	dock->plotWidget->addGraph();

	double min_y = *std::min_element(data_y.constBegin(), data_y.constEnd());
	double max_y = *std::max_element(data_y.constBegin(), data_y.constEnd());
	dock->plotWidget->graph(0)->setData(data_x, data_y);
	dock->plotWidget->yAxis->setRange(min_y, max_y);
	dock->plotWidget->xAxis->setRange(*std::min_element(data_x.constBegin(), data_x.constEnd()),
		*std::max_element(data_x.constBegin(), data_x.constEnd()));
	frameMarker->setVisible(true);
	frameMarker->start->setCoords(norm, max_y);
	frameMarker->end->setCoords(norm, min_y);

	dock->plotWidget->addGraph();
	dock->plotWidget->graph(1)->setPen(QPen(QColor(Qt::red)));
	dock->plotWidget->yAxis2->setVisible(true);
	QVector<double> data_y3, data_x3;
	filterData(data_x, data_y, data_x3, data_y3);

	QVector<double> data_y2, data_x2;
	filterData(data_x3, data_y3, data_x2, data_y2);

	min_y = *std::min_element(data_y2.constBegin(), data_y2.constEnd());
	max_y = *std::max_element(data_y2.constBegin(), data_y2.constEnd());
	dock->plotWidget->graph(1)->setData(data_x2, data_y2);
	dock->plotWidget->yAxis2->setRange(min_y, max_y);
	dock->plotWidget->graph(1)->setValueAxis(dock->plotWidget->yAxis2);

	dock->plotWidget->replot();
	dock->plotWidget->show();
}

void PlotWindow::plot_results()
{
	frameMarker->setVisible(false);
	dock->plotWidget->clearGraphs();

	if (Data<unsigned short>::getInstance()->results().size() != dock->comboBox_regions->currentIndex())
	{
		UpdateResultsAction(dock->comboBox_regions->currentIndex()).run();
	}

	QVector<double> data_y, data_x;
	QVector<QString> labels;
	double dist = (Data<unsigned short>::getInstance()->results().size() > 0) ? Data<unsigned short>::getInstance()->max_distance() / Data<unsigned short>::getInstance()->results().size() : 0;
	for (int i = 0; i < Data<unsigned short>::getInstance()->results().size(); i++)
	{
		data_x.push_back(i + 1);
		data_y.push_back(Data<unsigned short>::getInstance()->results()[i][dock->comboBox_value->currentIndex()]);
		labels.push_back("Region " + QString::number(i + 1) + "\n" + QString::number(dist*i, 'f', 2) + " - " + QString::number(dist*(i + 1), 'f', 2));
	}

	values->setVisible(true);
	values->setAntialiased(false); // gives more crisp, pixel aligned bar borders
	values->setStackingGap(1);
	values->setPen(QPen(QColor(111, 9, 176).lighter(170)));
	values->setBrush(QColor(111, 9, 176));

	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	textTicker->addTicks(data_x, labels);
	
	dock->plotWidget->xAxis->setTicker(textTicker);
	dock->plotWidget->xAxis->setTickLabelRotation(60);
	dock->plotWidget->xAxis->setSubTicks(false);
	dock->plotWidget->xAxis->setTickLength(0, 4);
	dock->plotWidget->xAxis->setRange(0, Data<unsigned short>::getInstance()->results().size() + 1);
	dock->plotWidget->xAxis->grid()->setVisible(true);
	dock->plotWidget->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	// prepare y axis:
	dock->plotWidget->yAxis->setLabel(dock->comboBox_value->currentText());
	double max_y = *std::max_element(data_y.constBegin(), data_y.constEnd());
	dock->plotWidget->yAxis->setRange(0, max_y);
	dock->plotWidget->yAxis->grid()->setSubGridVisible(true);
	dock->plotWidget->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	dock->plotWidget->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	values->setData(data_x, data_y);

	dock->plotWidget->replot();
	dock->plotWidget->show();
}

void PlotWindow::filterData(QVector<double> & data_x_in, QVector<double> & data_y_in, QVector<double> & data_x_out, QVector<double> & data_y_out)
{
#define kernel_size 15
	double kernel[kernel_size*2 + 1];
	double sigma = 5;
	for (int i = -kernel_size; i <= kernel_size; i++)
	{
		kernel[i + kernel_size] = -i / (sigma * sigma * sigma * sqrt(2 * M_PI)) * exp(-(i*i) / (2 * sigma*sigma));
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

void PlotWindow::on_comboBox_channel_currentIndexChanged(int index)
{
	if (updating)
		return;

	channel = index;

	plot();
}

void PlotWindow::on_comboBox_point_currentIndexChanged(int index)
{
	if (updating)
		return;

	Data<unsigned short>::getInstance()->set_active_point(index);
	emit Data<unsigned short>::getInstance()->emitter()->active_point_changed();

	plot();
}

void PlotWindow::on_comboBox_type_currentIndexChanged(int index)
{
	if (updating)
		return;

	if (index == 0)
	{
		dock->frame_PointPlot->setVisible(true);
		dock->frame_Results->setVisible(false);
	} else
	{
		dock->frame_PointPlot->setVisible(false);
		dock->frame_Results->setVisible(true);
	}

	plot();
}

void PlotWindow::on_comboBox_regions_currentIndexChanged(int index)
{
	if (updating)
		return;

	plot();
}

void PlotWindow::on_comboBox_value_currentIndexChanged(int index)
{
	if (updating)
		return;

	plot();
}

void PlotWindow::init()
{
	updating = true;
	int items = Data<unsigned short>::getInstance()->mesh_points().size();
	if (Data<unsigned short>::getInstance()->active_point() == -1){
		for (int i = 0; i < items; i++){
			//std::cerr << "Add point " << i << std::endl;
			dock->comboBox_point->addItem(QString("Point ") + QString::number(i));
		}

		Data<unsigned short>::getInstance()->set_active_point(0);
	}

	dock->comboBox_regions->clear();
	for (int i = 0; i < 100; i++){
		dock->comboBox_regions->addItem(QString::number(i));
	}

	if (!Data<unsigned short>::getInstance()->results().empty())
		dock->comboBox_point->setCurrentIndex(Data<unsigned short>::getInstance()->results().size());

	updating = false;
	dock->comboBox_point->setCurrentIndex(Data<unsigned short>::getInstance()->active_point());
	plot();
}

PlotWindow::~PlotWindow()
{

}

