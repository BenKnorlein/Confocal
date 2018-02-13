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
///\file IcoSphereCreator.cpp
///\author Benjamin Knorlein
///\date 12/11/2017

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "IcoSphereCreator.h"
#include <list>

using namespace conf;

void IcoSphereCreator::create(int recursionLevel, std::vector<TriangleIndices>& indices, std::vector<Point>& points)
{
	index = 0;
	m_indices = &indices;
	m_points = &points;

	// create 12 vertices of a icosahedron
	double t = (1.0 + sqrt(5.0)) / 2.0;

	addVertex(Point{ -1, t, 0 });
	addVertex(Point{ 1, t, 0 });
	addVertex(Point{ -1, -t, 0 });
	addVertex(Point{ 1, -t, 0 });

	addVertex(Point{ 0, -1, t });
	addVertex(Point{ 0, 1, t });
	addVertex(Point{ 0, -1, -t });
	addVertex(Point{ 0, 1, -t });

	addVertex(Point{ t, 0, -1 });
	addVertex(Point{ t, 0, 1 });
	addVertex(Point{ -t, 0, -1 });
	addVertex(Point{ -t, 0, 1 });

	// create 20 triangles of the icosahedron
	std::list<TriangleIndices> faces = std::list<TriangleIndices>();

	// 5 faces around point 0
	faces.push_back(TriangleIndices{ 0, 11, 5 });
	faces.push_back(TriangleIndices{ 0, 5, 1 });
	faces.push_back(TriangleIndices{ 0, 1, 7 });
	faces.push_back(TriangleIndices{ 0, 7, 10 });
	faces.push_back(TriangleIndices{ 0, 10, 11 });

	// 5 adjacent faces 
	faces.push_back(TriangleIndices{ 1, 5, 9 });
	faces.push_back(TriangleIndices{ 5, 11, 4 });
	faces.push_back(TriangleIndices{ 11, 10, 2 });
	faces.push_back(TriangleIndices{ 10, 7, 6 });
	faces.push_back(TriangleIndices{ 7, 1, 8 });

	// 5 faces around point 3
	faces.push_back(TriangleIndices{ 3, 9, 4 });
	faces.push_back(TriangleIndices{ 3, 4, 2 });
	faces.push_back(TriangleIndices{ 3, 2, 6 });
	faces.push_back(TriangleIndices{ 3, 6, 8 });
	faces.push_back(TriangleIndices{ 3, 8, 9 });

	// 5 adjacent faces 
	faces.push_back(TriangleIndices{ 4, 9, 5 });
	faces.push_back(TriangleIndices{ 2, 4, 11 });
	faces.push_back(TriangleIndices{ 6, 2, 10 });
	faces.push_back(TriangleIndices{ 8, 6, 7 });
	faces.push_back(TriangleIndices{ 9, 8, 1 });


	// refine triangles
	for (int i = 0; i < recursionLevel; i++)
	{
		std::list<TriangleIndices>  faces2 = std::list<TriangleIndices>();
		for(auto tri : faces)
		{
			// replace triangle by 4 triangles
			int a = getMiddlePoint(tri.v1, tri.v2);
			int b = getMiddlePoint(tri.v2, tri.v3);
			int c = getMiddlePoint(tri.v3, tri.v1);

			faces2.push_back(TriangleIndices{ tri.v1, a, c });
			faces2.push_back(TriangleIndices{ tri.v2, b, a });
			faces2.push_back(TriangleIndices{ tri.v3, c, b });
			faces2.push_back(TriangleIndices{ a, b, c });
		}
		faces = faces2;
	}

	// done, now add triangles to mesh
	for(auto tri : faces)
	{
		indices.push_back(tri);
	}
}


int IcoSphereCreator::addVertex(Point p)
{
	double length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	m_points->push_back(Point{ p.x / length, p.y / length, p.z / length });
	return index++;
}

int IcoSphereCreator::getMiddlePoint(int p1, int p2)
{
	// first check if we have it already
	bool firstIsSmaller = p1 < p2;
	int64_t  smallerIndex = firstIsSmaller ? p1 : p2;
	int64_t  greaterIndex = firstIsSmaller ? p2 : p1;
	int64_t  key = (greaterIndex << 32) + smallerIndex;

	auto ret = middlePointIndexCache.find(key);
	if (ret != middlePointIndexCache.end())
	{
		return ret->second;
	}

	// not in cache, calculate it
	Point point1 = (*m_points)[p1];
	Point point2 = (*m_points)[p2];
	Point middle = Point{
		(point1.x + point2.x) / 2.0,
		(point1.y + point2.y) / 2.0,
		(point1.z + point2.z) / 2.0 };

	// add vertex makes sure point is on unit sphere
	int i = addVertex(middle);

	// store it, return index
	middlePointIndexCache.insert(std::make_pair(key, i));
	return i;
}
