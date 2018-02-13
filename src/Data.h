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
///\file Data.h
///\author Benjamin Knorlein
///\date 11/22/2017

#pragma once

#ifndef DATA_H
#define DATA_H

#include <memory>
#include "Volume.h"
#include "IcoSphereCreator.h"
#include <vector>
#include <opencv2/core/mat.hpp>
#include <QObject>

namespace conf
{
	class DataEmitter : public QObject
	{
		Q_OBJECT

	public:
	signals :
		void volume_updated();
		void mesh_updated();
		void render_threshold_changed(float val);
		void render_multiplier_changed(float val);
	};

	template<class T>
	class Data
	{
	public:
		static std::unique_ptr<Data <T> > &getInstance(){
			if (!m_instance)
			{
				m_instance.reset(new Data<T>());
			}
			return m_instance;
		}

		~Data()
		{

		}

		std::vector<cv::Mat> &image_r()
		{
			return m_image_r;
		}

		std::vector<cv::Mat> &image_g()
		{
			return m_image_g;
		}

		std::unique_ptr< Volume <T> > &volume(){
			return m_volume;
		}

		void generateVolume()
		{
			m_volume.reset(new conf::Volume<unsigned short>(m_image_r[0].cols, m_image_r[0].rows, m_image_r.size(), 0.645, 0.645, 5, 3)); //TODO offset variable 
	
			//fill vol and points
			std::vector <const cv::Point3d> pts;
			for (int x = 0; x < m_volume->get_width(); x++)
			{
				for (int y = 0; y < m_volume->get_height(); y++)
				{
					for (int z = 0; z < m_volume->get_depth(); z++)
					{
						*(m_volume->get(x, y, z, 0)) = m_image_r[z].at<unsigned short>(y, x);
						*(m_volume->get(x, y, z, 1)) = m_image_g[z].at<unsigned short>(y, x);
					}
				}
			}	
			emit m_emitter.volume_updated();
		}

		void setModel(std::vector<double> &model)
		{
			m_model = model;
			m_mesh_indices.clear();
			m_mesh_points.clear();

			IcoSphereCreator creator; 
			creator.create(1, m_mesh_indices, m_mesh_points);
			for (auto &pt : m_mesh_points)
			{
				pt.x = pt.x * model[3] + model[0];
				pt.y = pt.y * model[3] + model[1];
				pt.z = pt.z * model[3] + model[2];
			}
			emit m_emitter.mesh_updated();
		}

		const std::vector<double> &model() const
		{
			return m_model;
		}

		DataEmitter *emitter()
		{
			return &m_emitter;
		}

		std::vector<TriangleIndices> mesh_indices() const
		{
			return m_mesh_indices;
		}

		std::vector<Point> mesh_points() const
		{
			return m_mesh_points;
		}

	private:
		Data<T>(){};

		static std::unique_ptr<Data <T> > m_instance;

		std::unique_ptr< Volume <T> > m_volume;

		std::vector <cv::Mat> m_image_r;

		std::vector <cv::Mat> m_image_g;

		std::vector <double> m_model;

		std::vector <TriangleIndices> m_mesh_indices;

		std::vector<Point> m_mesh_points;

		DataEmitter m_emitter;
	};

	template<class T>
	std::unique_ptr< Data <T> > Data <T>::m_instance = nullptr;

}
#endif // DATA_H
