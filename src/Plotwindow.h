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
///\file Plotwindow.h
///\author Benjamin Knorlein
///\date 3/15/2018

#pragma once

#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QDockWidget>
#include "external/qcustomplot.h"

namespace Ui
{
	class PlotWindow;
}

namespace conf
{
	class PlotWindow : public QDockWidget
	{
		Q_OBJECT

	public:
		virtual ~PlotWindow();
		//static PlotWindow* getInstance();
	//private:
		PlotWindow(QWidget* parent = 0);
		//static PlotWindow* instance;
		void installEventFilterToChildren(QObject* object);

	protected:
		bool eventFilter(QObject* target, QEvent* event) override;

	private:
		Ui::PlotWindow* dock;
		int channel;

		void plot();
		void plot_points();
		void plot_results();
		bool updating;
		QCPItemLine* frameMarker;
		QCPBars *values;
		void filterData(QVector<double> & data_x_in, QVector<double> & data_y_in, QVector<double> & data_x_out, QVector<double> & data_y_out);

	public slots:
		void on_comboBox_channel_currentIndexChanged(int index);
		void on_comboBox_point_currentIndexChanged(int index);
		void on_comboBox_type_currentIndexChanged(int index);
		void on_comboBox_regions_currentIndexChanged(int index);
		void on_comboBox_value_currentIndexChanged(int index);

		void init();

	};
}
#endif // PLOTWINDOW_H
