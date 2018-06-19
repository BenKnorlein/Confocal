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
///\file GenerateMasksAction.h
///\author Benjamin Knorlein
///\date 5/7/2018

#pragma once

#ifndef GENERATEMASKSACTION_H
#define GENERATEMASKSACTION_H
#include <string>
#include <opencv2/core/mat.hpp>

namespace conf
{
	class GenerateMasksAction
	{
	public:
		GenerateMasksAction(int iterations = 300, double contractionbias = 0, double smoothness = 0);
		virtual ~GenerateMasksAction();
		void run();
	private:
		void drawMesh(int slice, cv::Mat & image, cv::Scalar color);
		void drawCircle(int slice, cv::Mat & image, cv::Scalar color);

		bool m_Initialized_ISO;
		int m_iterations;
		double m_contractionbias;
		double m_smoothness;
	};
}
#endif // GENERATEMASKSACTION_H
