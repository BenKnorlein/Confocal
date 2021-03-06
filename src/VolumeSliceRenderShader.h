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
///\file VolumeSliceRenderShader.h
///\author Benjamin Knorlein
///\date 11/30/2017

#pragma once

#ifndef VOLUMESLICERENDERSHADER_H
#define VOLUMESLICERENDERSHADER_H

#include "Shader.h"

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

#include <glm/glm.hpp>

namespace conf
{
	class VolumeSliceRenderShader : public Shader
	{
		
	public:
		VolumeSliceRenderShader();
		virtual ~VolumeSliceRenderShader();

		void render(glm::mat4 &MVP, GLsizei count);
		void initGL();

		void set_threshold(float threshold)
		{
			m_threshold = threshold;
		}

		void set_multiplier(float multiplier)
		{
			m_multiplier = multiplier;
		}

	private:
		float m_threshold;
		float m_multiplier;

		GLuint m_volume_uniform;
		GLuint m_vVertex_attribute;
		GLuint m_MVP_uniform;

		GLuint m_threshold_uniform;
		GLuint m_multiplier_uniform;

	};
}
#endif // VOLUMESLICERENDERSHADER_H
