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
///\file ConfocalMainWindow.h
///\author Benjamin Knorlein
///\date 11/22/2017

#pragma once

#ifndef CONFOCALMAINWINDOW_H
#define CONFOCALMAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
	class ConfocalMainWindow;
}

namespace conf
{
	class ConfocalDockWidget;
	class ConfocalSliceView;
	class PlotWindow;

	class ConfocalMainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		static ConfocalMainWindow* getInstance();
		virtual ~ConfocalMainWindow();

	private:
		ConfocalMainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
		static ConfocalMainWindow* instance;

		Ui::ConfocalMainWindow* main;

		ConfocalDockWidget *dockWidget;
		ConfocalSliceView* sliceview;
		PlotWindow* plotwindow;
	};
}
#endif // CONFOCALMAINWINDOW_H
