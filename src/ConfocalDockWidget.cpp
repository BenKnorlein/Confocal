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
///\file ConfocalDockWidget.cpp
///\author Benjamin Knorlein
///\date 11/22/2017

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "ConfocalDockWidget.h"
#include "ui_ConfocalDockWidget.h"
#include <iostream>
#include "LoadDataAction.h"
#include "SphereFit.h"
#include "Data.h"
#include <QFileDialog>
#include "LoadProcessedDataAction.h"
#include "SaveProcessedDataAction.h"
#include "GenerateMasksAction.h"
#include "ui_ProcessingDialog.h"
#include "ProcessingDialog.h"

using namespace conf;

ConfocalDockWidget::ConfocalDockWidget(QWidget* parent, Qt::WindowFlags flags) :QDockWidget(parent, flags), dock(new Ui::ConfocalDockWidget)
{
	dock->setupUi(this);
	dock->groupBox->setVisible(false);
}

ConfocalDockWidget::~ConfocalDockWidget()
{
}

void ConfocalDockWidget::on_pushButton_Load_clicked()
{
	QString folder = "D:\\data\\Beth\\row7\\row7\\r07c03f03"; //TODO folder variable
	folder = QFileDialog::getExistingDirectory(this, "Select datafolder", folder);
	LoadDataAction(folder.toStdString()).run();
}

void ConfocalDockWidget::on_pushButton_Compute_clicked()
{
	SphereFit().run();
}

void ConfocalDockWidget::on_pushButton_Masks_clicked()
{
	ProcessingDialog * diag = new ProcessingDialog;
	int ok = diag->exec();

	if (ok){
		GenerateMasksAction(diag->getIterations(),diag->getContractionBias(),diag->getSmoothness()).run();
	}
	delete diag;
}

void ConfocalDockWidget::on_pushButton_LoadProcessed_clicked()
{
	QString folder = "D:\\data\\Beth\\row7\\row7\\r07c03f03"; //TODO folder variable
	folder = QFileDialog::getExistingDirectory(this, "Select datafolder", folder);
	LoadProcessedDataAction(folder.toStdString()).run();
}

void ConfocalDockWidget::on_pushButton_SaveProcessed_clicked()
{
	QString folder = "D:\\data\\Beth\\row7\\row7\\r07c03f03"; //TODO folder variable
	folder = QFileDialog::getExistingDirectory(this, "Select datafolder", folder);
	SaveProcessedDataAction(folder.toStdString()).run();
}

void ConfocalDockWidget::on_horizontalSlider_Multiplier_valueChanged(int value)
{
	dock->label_Multiplier->setText(QString::number(static_cast<float>(value) / 10));
	emit Data<unsigned short>::getInstance()->emitter()->render_multiplier_changed(static_cast<float>(value) / 10 );
}

void ConfocalDockWidget::on_horizontalSlider_Threshold_valueChanged(int value)
{
	dock->label_Threshold->setText(QString::number(static_cast<float>(value) / 1000));
	emit Data<unsigned short>::getInstance()->emitter()->render_threshold_changed(static_cast<float>(value) / 1000);
}
