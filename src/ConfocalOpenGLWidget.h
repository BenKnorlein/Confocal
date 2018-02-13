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
///\file ConfocalOpenGLWidget.h
///\author Benjamin Knorlein
///\date 11/22/2017

#pragma once

#ifndef CONFOCALOPENGLWIDGET_H
#define CONFOCALOPENGLWIDGET_H

#include "VolumeSliceRender.h"
#include <QOpenGLWidget>
#include <glm/glm.hpp>

namespace conf
{
	class ConfocalOpenGLWidget : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		ConfocalOpenGLWidget(QWidget * parent = 0, Qt::WindowFlags flags = 0);
		virtual ~ConfocalOpenGLWidget();

	protected:
		virtual void paintGL();
		virtual void resizeGL(int w, int h);
		virtual void initializeGL();

		virtual void mousePressEvent(QMouseEvent* e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void wheelEvent(QWheelEvent *e);

	private:
		void updateTexture();
		bool m_texture_update;
		bool m_texture_loaded;
		void updateMesh();
		bool m_mesh_update;
		bool m_mesh_loaded;

		double eyedistance;
		float azimuth;
		float polar;
		float prev_x;
		float prev_y;

		int width;
		int height;

		GLuint textureID;
		float m_texture_scale[3];
		GLuint m_mesh_index_buffer;
		GLuint m_mesh_vertex_buffer;
		unsigned int m_mesh_size;
		
		VolumeSliceRender m_slice_render;

		glm::mat4 P;
		glm::mat4 MV;
		glm::mat4 to_volume;

	public slots:
		void update_texture_posted();
		void update_mesh_posted();
		void render_threshold_posted(float val);
		void render_multiplier_posted(float val);
	};
}
#endif // CONFOCALOPENGLWIDGET_H
