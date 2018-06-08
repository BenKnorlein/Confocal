﻿//  ----------------------------------
// Copyright © 2015, Brown University, Providence, RI.
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
///\file MSAC.h
///\author Benjamin Knorlein
///\date 11/7/2017

#pragma once

#ifndef MSAC_H
#define MSAC_H

#include <functional>
#include <vector>
#include <opencv2/core/mat.hpp>


namespace conf
{
	class MSAC
	{
		public:
			std::vector<double> run(const unsigned int &samples, std::vector<cv::Point3d> &all_pts,
				std::function<std::vector<double>(std::vector<cv::Point3d*>&)> fit_function,
				std::function<std::vector<double>(std::vector<double>, std::vector<cv::Point3d> &)> eval_function,
				std::function<bool(std::vector<double>)> check_function,
				const double &max_distance, const unsigned int &max_trials = 1000, const double &confidence = 99, bool compute_with_all_inlier = false);

	private:
		static unsigned int computeRemainingLoops(const unsigned int &_samples, const double &confidence, const unsigned int &nb_points, const unsigned int &nb_inlier);
	};
}
#endif // MSAC_H
