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
///\file MSAC.cpp
///\author Benjamin Knorlein
///\date 11/7/2017

#pragma once
#define NOMINMAX
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include "MSAC.h"
#include <math.h>
#include <random>
#include <chrono>
#include <opencv2/core.hpp>
#include <iostream>

using namespace conf;

std::vector <unsigned int> generate_unique_random_numbers(unsigned number_samples,  std::uniform_int_distribution<unsigned int> &d,  std::default_random_engine &e)
{
	std::vector<unsigned int> numbers;
	while (numbers.size() < number_samples)
	{
		unsigned int number = d(e);

		if (std::find(numbers.begin(), numbers.end(), number) == numbers.end())
		{
			numbers.push_back(number);
		}
	}
	return numbers;
}

unsigned int MSAC::computeRemainingLoops(const unsigned int &_samples, const double &confidence, const unsigned int &nb_points, const unsigned int &nb_inlier)
{

	double nb_inlier_d = static_cast<double>(nb_inlier);
	double inlierProbability = pow(nb_inlier_d / nb_points, _samples);

	if (inlierProbability < std::numeric_limits<double>::epsilon())
		return std::numeric_limits<unsigned int>::max();

	double conf = 0.01 * confidence;

	double num = log10(1.0f - conf);
	double den = log10(1.0f - inlierProbability);
	return ceil(num / den);

}

std::vector<double> MSAC::run(const unsigned int &samples, std::vector<const cv::Point3d> &all_pts,
		std::function<std::vector<double>(std::vector<const cv::Point3d*>&)> fit_function, 
		std::function<std::vector<double>(std::vector<double>, std::vector<const cv::Point3d> &)> eval_function,
		std::function<bool(std::vector<double>)> check_function,
		const double &max_distance, const unsigned int &max_trials, const double &confidence, bool compute_with_all_inlier)
{
	//unsigned int max_skip_trials = max_trials * 10;
	unsigned int skipped_trials = 0;
	unsigned int trial_count = 0;
	unsigned int max_trials_count = max_trials;
	std::vector<double> best_model;
	double best_distance = std::numeric_limits<double>::max();
	unsigned int best_nb_inlier = 0;
	std::vector<bool> inlier(all_pts.size());
	std::vector<bool> best_inlier(all_pts.size());

	std::vector<const cv::Point3d * > sample_pts(samples);

	std::default_random_engine e{ static_cast <unsigned int> (std::chrono::system_clock::now().time_since_epoch().count()) };
	std::uniform_int_distribution<unsigned int> d{ 0, static_cast <unsigned int>(all_pts.size() - 1) };

	while (trial_count < max_trials_count && skipped_trials < max_trials_count * 10)
	{

		std::vector<unsigned int> idx = generate_unique_random_numbers(samples, d, e) ;
		for (int i = 0; i < samples; i++)
			sample_pts[i] = &all_pts[idx[i]];

		std::vector<double> model = fit_function(sample_pts);
		
		if (check_function(model))
		{
			std::vector<double> error = eval_function(model, all_pts);
			double dist = 0;
			int nb_inlier = 0;
			for (int i = 0; i < error.size(); i++)
			{
				inlier[i] = (error[i] < max_distance);
				dist += inlier[i] ? error[i] : max_distance;
				nb_inlier += inlier[i];
			}

			if (dist < best_distance)
			{
				best_distance = dist;
				best_inlier = inlier;
				best_model = model;
				best_nb_inlier = nb_inlier;
				unsigned int loops = computeRemainingLoops(samples, confidence, all_pts.size(), nb_inlier);
				max_trials_count = std::min(loops, max_trials_count);
			}
			trial_count++;
		}
		else
		{
			skipped_trials++;
		}
	}

	if (compute_with_all_inlier && best_nb_inlier >  samples && check_function(best_model))
	{
		sample_pts.clear();
		for (int i = 0; i < best_inlier.size(); i++){
			if (best_inlier[i])
				sample_pts.push_back(&all_pts[i]);
		}
		best_model = fit_function(sample_pts);
	}

	return best_model;
}
