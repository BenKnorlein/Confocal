﻿//  ----------------------------------
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
///\file LoadDataAction.cpp
///\author Benjamin Knorlein
///\date 11/28/2017

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
	#include <windows.h>
	#define OS_SLASH "\\"
#elif defined(__APPLE__)
	#define OS_SLASH "/"
#endif

#include "LoadDataAction.h"
#include "Data.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "dirent.h"
#include <iostream>


using namespace conf;

LoadDataAction::LoadDataAction(std::string folder) : m_folder{folder}
{

}

void LoadDataAction::run()
{
	std::vector <std::string> filenames = readTiffs(m_folder);
	
	for( auto name : filenames)
	{
		if (contains_string(name, "ch1"))
		{
			Data<unsigned short>::getInstance()->image_r().push_back(std::move(cv::imread(name, CV_LOAD_IMAGE_ANYDEPTH)));
		}
		else if (contains_string(name, "ch2"))
		{
			Data<unsigned short>::getInstance()->image_g().push_back(std::move(cv::imread(name, CV_LOAD_IMAGE_ANYDEPTH)));
		}
	}
	
	Data<unsigned short>::getInstance()->generateVolume();
}


bool LoadDataAction::ends_with_string(std::string const& str, std::string const& what) {
	return what.size() <= str.size()
		&& str.find(what, str.size() - what.size()) != str.npos;
}

bool LoadDataAction::contains_string(std::string const& str, std::string const& what) {
	return str.find(what) != std::string::npos;
}


std::vector<std::string> LoadDataAction::readTiffs(std::string foldername)
{
	std::vector <std::string> out_vector;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(foldername.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			printf("%s\n", ent->d_name);
			if (ends_with_string(ent->d_name, "tiff"))
			{
				out_vector.push_back(foldername + OS_SLASH + ent->d_name);
			}
		}
		closedir(dir);
	}
	else {
		std::cerr << "Could not open folder" << std::endl; 
	}
	std::sort(out_vector.begin(), out_vector.end());
	return out_vector;
}