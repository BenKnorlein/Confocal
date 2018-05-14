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
// Code taken from http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
///\file IcoSphereCreator.h
///\author Benjamin Knorlein
///\date 12/11/2017

#pragma once

#ifndef ICOSPHERECREATOR_H
#define ICOSPHERECREATOR_H
#include <vector>
#include <opencv2/core/types.hpp>
#include <map>
#include <cstdint>

namespace conf
{
	struct TriangleIndices
	{
		int v1;
		int v2;
		int v3;
	};

	struct Point
	{
		float x;
		float y;
		float z;
	};

	inline Point operator+(Point a,const Point b)
	{
		Point p;
		p.x = a.x + b.x;
		p.y = a.y + b.y;
		p.z = a.z + b.z;
		return p;
	}
	inline Point operator-(Point a, const Point b)
	{
		Point p;
		p.x = a.x - b.x;
		p.y = a.y - b.y;
		p.z = a.z - b.z;
		return p;
	}

	inline Point operator*(double a, const Point b)
	{
		Point p;
		p.x = a * b.x;
		p.y = a * b.y;
		p.z = a * b.z;
		return p;
	}
	
	
	class IcoSphereCreator
	{
	public:
		void create(int recursionLevel, std::vector <TriangleIndices> &indices, std::vector<Point> &points);

	private:
		int addVertex(Point p);
		int getMiddlePoint(int p1, int p2);

		int index;
		std::vector <TriangleIndices>* m_indices;
		std::vector<Point>* m_points;
		std::map<int64_t, int> middlePointIndexCache;
	};
}
#endif // ICOSPHERECREATOR_H
