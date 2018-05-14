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
///\file ConfocalOpenGLWidget.cpp
///\author Benjamin Knorlein
///\date 11/22/2017

#pragma once

#include "GL/glew.h"

#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
	#include <windows.h>
	#include <GL/gl.h>
	#include <gl/GLU.h>
	#define M_PI 3.14159265358979323846
#elif defined(__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/glu.h>
#else
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#ifndef M_PI
#define M_PI 3.141592653
#endif

#include "ConfocalOpenGLWidget.h"
#include "QMouseEvent"
#include <iostream>
#include <Data.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace conf;


//GLfloat LightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // Ambient Light Values
//GLfloat LightDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f }; // Diffuse Light Values
//GLfloat LightPosition[] = { 0.0f, 10.0f, 0.0f, 1.0f }; // Light Position

ConfocalOpenGLWidget::ConfocalOpenGLWidget(QWidget* parent, Qt::WindowFlags flags) :QOpenGLWidget(parent, flags), eyedistance{ -2 }, azimuth{ 50 }, polar{ 4 }, width{ 10 }
, height{ 10 }, textureID{ 0 }, m_texture_update{ false }, m_mesh_update{ false }, m_mesh_index_buffer{ 0 }, m_mesh_vertex_buffer{ 0 }, m_texture_loaded{ false }, m_mesh_loaded{ false }, m_mesh_size{0}
{
	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(volume_updated()), this, SLOT(update_texture_posted()));
	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(mesh_updated()), this, SLOT(update_mesh_posted()));
	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(active_point_changed()), this, SLOT(update()));
	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(render_threshold_changed(float)), this, SLOT(render_threshold_posted(float)));
	connect(Data<unsigned short>::getInstance()->emitter(), SIGNAL(render_multiplier_changed(float)), this, SLOT(render_multiplier_posted(float)));
}

ConfocalOpenGLWidget::~ConfocalOpenGLWidget()
{
	if (textureID != 0) 
		glDeleteTextures(1, &textureID);
	
}

void ConfocalOpenGLWidget::mousePressEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		prev_y = e->localPos().y();
		prev_x = e->localPos().x();
	}
}

void ConfocalOpenGLWidget::mouseMoveEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		polar += (e->localPos().y() - prev_y) / 5.0f;
		azimuth += (e->localPos().x() - prev_x) / 5.0f;

		prev_y = e->localPos().y();
		prev_x = e->localPos().x();
		update();
	}
}

void ConfocalOpenGLWidget::wheelEvent(QWheelEvent* e)
{

	eyedistance += e->delta() / 5000.0f;
	update();
}

void ConfocalOpenGLWidget::updateTexture()
{ 
	if (textureID != 0) glDeleteTextures(1, &textureID);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,
		GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,
		GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R,
		GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 
		Data<unsigned short>::getInstance()->volume()->get_width(), 
		Data<unsigned short>::getInstance()->volume()->get_height(), 
		Data<unsigned short>::getInstance()->volume()->get_depth(),
		0, GL_RGB, GL_UNSIGNED_SHORT, Data<unsigned short>::getInstance()->volume()->get(0,0,0,0));
	glGenerateMipmap(GL_TEXTURE_3D);

	m_texture_scale[0] = 1.0f / (Data<unsigned short>::getInstance()->volume()->get_x_scale() * Data<unsigned short>::getInstance()->volume()->get_width());
	m_texture_scale[1] = 1.0f / (Data<unsigned short>::getInstance()->volume()->get_y_scale() * Data<unsigned short>::getInstance()->volume()->get_height());
	m_texture_scale[2] = 1.0f / (Data<unsigned short>::getInstance()->volume()->get_z_scale() * Data<unsigned short>::getInstance()->volume()->get_depth());

	m_texture_update = false;
	m_texture_loaded = true;

	Data<unsigned short>::getInstance()->set_texture_id(textureID);

	emit Data<unsigned short>::getInstance()->emitter()->texture_updated();
}

void ConfocalOpenGLWidget::updateMesh()
{
	if (m_mesh_index_buffer) glDeleteBuffers(1, &m_mesh_index_buffer);
	if (m_mesh_vertex_buffer) glDeleteBuffers(1, &m_mesh_vertex_buffer);

	m_mesh_size = Data<unsigned short>::getInstance()->mesh_indices().size() * 3;

	glGenBuffers(1, &m_mesh_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh_size * sizeof(GLuint), &Data<unsigned short>::getInstance()->mesh_indices()[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_mesh_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_mesh_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, Data<unsigned short>::getInstance()->mesh_points().size() * 3 * sizeof(float), &Data<unsigned short>::getInstance()->mesh_points()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_mesh_update = false;
	m_mesh_loaded = true;
}

void ConfocalOpenGLWidget::update_texture_posted()
{
	m_texture_update = true;
	update();
}

void ConfocalOpenGLWidget::update_mesh_posted()
{
	m_mesh_update = true;
	update();
}

void ConfocalOpenGLWidget::render_threshold_posted(float val)
{
	m_slice_render.set_threshold(val);
	update();
}

void ConfocalOpenGLWidget::render_multiplier_posted(float val)
{
	m_slice_render.set_multiplier(val);
	update();
}

void ConfocalOpenGLWidget::paintGL()
{
	if (m_texture_update)
		updateTexture();

	if (m_mesh_update)
		updateMesh();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MV = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, eyedistance));
	MV = glm::rotate(MV, glm::radians(polar), glm::vec3(1.0f, 0.0f, 0.0f));
	MV = glm::rotate(MV, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
	MV = glm::scale(MV, glm::vec3(m_texture_scale[0] / m_texture_scale[0], m_texture_scale[0] / m_texture_scale[1], m_texture_scale[0] / m_texture_scale[2]));

	if (m_mesh_loaded){
		to_volume = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));
		to_volume = glm::scale(to_volume, glm::vec3(m_texture_scale[0], m_texture_scale[1], m_texture_scale[2]));
		//to_volume = glm::rotate(to_volume, glm::radians(90.0f), glm::vec3(1, 0, 0));
		//to_volume = glm::rotate(to_volume, glm::radians(90.0f), glm::vec3(0, 1, 0));

		glm::vec4 center = glm::vec4(Data<unsigned short>::getInstance()->center()[0],
			Data<unsigned short>::getInstance()->center()[1],
			Data<unsigned short>::getInstance()->center()[2], 1);

		center = to_volume * center;
		MV = glm::translate(MV, -glm::vec3(center.x / center.w, center.y / center.w, center.z / center.w));
	}

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(P));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(MV));

	if (m_mesh_loaded)
	{
		//scale
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(to_volume));

		glEnableClientState(GL_VERTEX_ARRAY);
		////vertices
		glBindBuffer(GL_ARRAY_BUFFER, m_mesh_vertex_buffer);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh_index_buffer);
		////Draw the mesh
		glDrawElements(GL_TRIANGLES, m_mesh_size, GL_UNSIGNED_INT, NULL);

		//unload
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//Draw Line
		if (Data<unsigned short>::getInstance()->active_point() >= 0)
		{
			Point center;
			center.x = Data<unsigned short>::getInstance()->center()[0];
			center.y = Data<unsigned short>::getInstance()->center()[1];
			center.z = Data<unsigned short>::getInstance()->center()[2];

			Point point = Data<unsigned short>::getInstance()->mesh_points()[Data<unsigned short>::getInstance()->active_point()];
			Point dir = point - center;
			
			glBegin(GL_LINES);
			glColor3f(1.0, 1.0, 1.0);
			glVertex3f(center.x, center.y, center.z);
			glVertex3f(center.x + dir.x * 1.5,
					center.y + dir.y * 1.5,
					center.z + dir.z * 1.5);
			glEnd();
		}
		
		glPopMatrix();
	}
	//glClear(GL_DEPTH_BUFFER_BIT);

	glBegin(GL_LINES);
	glColor3f(1.0, 0, 0);
	glVertex3f(0.5,0.5,0.5);
	glVertex3f(-0.5, 0.5, 0.5);

	glColor3f(0.0, 1.0, 0);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);

	glColor3f(0, 0, 1.0);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, -0.5);

	glColor3f(1.0,1.0, 1.0);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);

	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, 0.5);

	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);

	glEnd();

	if (m_texture_loaded){
		glBindTexture(GL_TEXTURE_3D, textureID);

		m_slice_render.render(MV, P);

		glBindTexture(GL_TEXTURE_3D, 0);
	}

	glFlush();
}

void ConfocalOpenGLWidget::resizeGL(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);
	P = glm::perspective(glm::radians(60.0f), static_cast<float>(w) / h, 0.1f, 1000.0f);
}

void ConfocalOpenGLWidget::initializeGL()
{
	int i;
	i = glewInit();
	
	m_slice_render.initGL();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glClearDepth(1.0f); // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
}
