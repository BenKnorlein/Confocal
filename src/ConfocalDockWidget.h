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
///\file ConfocalDockWidget.h
///\author Benjamin Knorlein
///\date 11/22/2017

#pragma once

#ifndef CONFOCALDOCKWIDGET_H
#define CONFOCALDOCKWIDGET_H
#include <QDockWidget>
namespace Ui
{
	class ConfocalDockWidget;
}

namespace conf
{
	class ConfocalDockWidget : public QDockWidget
	{
		Q_OBJECT

	public:
		ConfocalDockWidget(QWidget * parent = 0, Qt::WindowFlags flags = 0);
		virtual ~ConfocalDockWidget();

	private:
		Ui::ConfocalDockWidget * dock;
		
	public slots:
		void on_pushButton_Load_clicked();
		void on_pushButton_Compute_clicked();
		void on_pushButton_Refine_clicked();
		void on_pushButton_Masks_clicked();
		void on_horizontalSlider_Multiplier_valueChanged(int value);
		void on_horizontalSlider_Threshold_valueChanged(int value);
	};
}
#endif // CONFOCALDOCKWIDGET_H
