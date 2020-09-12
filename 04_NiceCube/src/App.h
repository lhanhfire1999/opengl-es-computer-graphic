#pragma once

#include <WindowListener.h>
#include <Graphic.h>
#include <Utils.h>
#include <string>
#include <cassert>
#include <glmath.h>
#include <Tga.h>

class App : public WindowListener
{
private:
	Graphic& m_graphic;
	int m_width, m_height;
	int m_matrixLocation;
	Matrix m_rotationMatrix;
	float m_distance;
	bool m_exit;
	bool m_moving;
	bool m_isgoingfar;

public:
	App(Graphic& graphic, int width, int height) : m_graphic(graphic), m_width(width), m_height(height),
		m_rotationMatrix(Matrix::identity())
	{
		auto vsSource = Utils::readFile("vs.glsl");
		auto vs = Utils::compileShader(vsSource, GL_VERTEX_SHADER);
		assert(vs > 0);

		auto fsSource = Utils::readFile("fs.glsl");
		auto fs = Utils::compileShader(fsSource, GL_FRAGMENT_SHADER);
		assert(fs > 0);

		auto program = Utils::linkProgram(vs, fs);
		assert(program > 0);
		glUseProgram(program);

		//
#define A -1.0f, -1.0f, 1.0f
#define B 1.0f, -1.0f, 1.0f
#define C 1.0f, 1.0f, 1.0f
#define D -1.0f, 1.0f, 1.0f

#define E 1.0f, -1.0f, -1.0f
#define F -1.0f, -1.0f, -1.0f
#define G -1.0f, 1.0f, -1.0f
#define H 1.0f, 1.0f, -1.0f

		static float position[] =
		{
			A, B, C, D, //front
			E, F, G, H, //back
			F, A, D, G, //left
			B, E, H, C, //right
			D, C, H, G, //top
			F, E, B, A, // bottom
		};
		auto positionLocation = glGetAttribLocation(program, "a_position");
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, position);
		glEnableVertexAttribArray(positionLocation);

		//
#define FACE_TEX_COORDS\
		0.0f, 0.0f,\
		1.0f, 0.0f,\
		1.0f, 1.0f,\
		0.0f, 1.0f,

		static float texCoords[] =
		{
			FACE_TEX_COORDS
			FACE_TEX_COORDS
			FACE_TEX_COORDS
			FACE_TEX_COORDS
			FACE_TEX_COORDS
			FACE_TEX_COORDS
		};

		auto texCoordLocation = glGetAttribLocation(program, "a_texCoord");
		assert(texCoordLocation >= 0);
		glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
		glEnableVertexAttribArray(texCoordLocation);

		//
		m_matrixLocation = glGetUniformLocation(program, "u_matrix");
		assert(m_matrixLocation >= 0);

		//
		GLuint texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		auto tga = Tga("cat.tga");
		assert(tga.okay());
		auto format = tga.hasAlpha() ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, tga.width(), tga.height(), 0, format, GL_UNSIGNED_BYTE, tga.data());
		auto samplerLocation = glGetUniformLocation(program, "u_sampler");
		assert(samplerLocation >= 0);
		glUniform1i(samplerLocation, 0);

		//
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		//
		m_distance = 0.0f;
		m_exit = false;
		m_moving = false;
		m_isgoingfar = true;


	}
	bool tick()
	{
		render();
		return update();
	}
	void onResixed(int newWidth, int newHeigth)
	{
		m_width = newWidth;
		m_height = newHeigth;
	}

	void onKeyDown(int keycode) {
		const float rotationStep = 2.0f;
		const float distanceStep = 0.1f;
		switch (keycode)
		{
		case VK_DOWN:
			m_rotationMatrix = Matrix::rotation(rotationStep, 1.0f, 0.0f, 0.0f) * m_rotationMatrix;
			break;
		case VK_UP:
			m_rotationMatrix = Matrix::rotation(-rotationStep, 1.0f, 0.0f, 0.0f) * m_rotationMatrix;
			break;
		case VK_LEFT:
			m_rotationMatrix = Matrix::rotation(-rotationStep, 0.0f, 1.0f, 0.0f) * m_rotationMatrix;
			break;
		case VK_RIGHT:
			m_rotationMatrix = Matrix::rotation(rotationStep, 0.0f, 1.0f, 0.0f) * m_rotationMatrix;
			break;
		case VK_SPACE:
			m_rotationMatrix = Matrix::identity();
			m_distance = 0.0f;
			break;
		case VK_RETURN:
			m_distance += distanceStep;
			break;
		case VK_BACK:
			m_distance -= distanceStep;
			break;
		case VK_TAB:
			m_moving = !m_moving;
			break;

		case VK_ESCAPE:
			m_exit = true;
			break;
		}
	}

	const float rotationStep = 2.0f;
	const float distanceStep = 0.1f;
private:
	bool update()
	{
		if (m_moving)
		{

			m_rotationMatrix = Matrix::rotation(-rotationStep, 1.0f, 0.0f, 0.0f) * m_rotationMatrix;
			m_rotationMatrix = Matrix::rotation(-rotationStep, 0.0f, 1.0f, 0.0f) * m_rotationMatrix;
			m_rotationMatrix = Matrix::rotation(-rotationStep, 0.0f, 0.0f, 1.0f) * m_rotationMatrix;

			if (m_isgoingfar)
			{
				m_distance -= distanceStep;
				if (m_distance <= -5.0f)
				{
					m_isgoingfar = false;
				}

			}
			else
			{
				m_distance += distanceStep;
				if (m_distance >= 0.0f)
				{
					m_isgoingfar = true;
				}
			}
		}
		return !m_exit;



	}

	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_width, m_height);

		const float h = 1.0f;
		const float w = h * m_width / m_height;

		const Matrix matrix =
			Matrix::frustum(-w / 2, w / 2, -h / 2, h / 2, 1.0f, 50.0f)
			* Matrix::translate(0.0f, 0.0f, -4.0f + m_distance)
			* m_rotationMatrix;
		glUniformMatrix4fv(m_matrixLocation, 1, GL_FALSE, matrix.data());

		const GLubyte indices[] =
		{
			0, 1, 2, 0, 2, 3, //front
			4, 5, 6, 4, 6, 7, // back
			8, 9, 10, 8, 10, 11, //left
			12, 13, 14, 12, 14, 15, //right
			16, 17, 18, 16, 18, 19, //top
			20, 21, 22, 20, 22, 23, //bottom
		};
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

		m_graphic.swapBuffers();
	}

};