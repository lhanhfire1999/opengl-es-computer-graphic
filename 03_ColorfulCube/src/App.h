#pragma once
#include <WindowListener.h>
#include <Graphic.h>
#include <Utils.h>
#include <string>
#include <cassert>
#include <glmath.h>

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
		const std::string vsSource = "\
		attribute vec3 a_position;\
		attribute vec3 a_color;\
		varying vec3 v_color;\
		uniform mat4 u_matrix;\
		void main()\
		{\
			gl_Position = u_matrix * vec4(a_position, 1.0);\
			v_color = a_color;\
		}";
		auto vs = Utils::compileShader(vsSource, GL_VERTEX_SHADER);
		assert(vs > 0);

		const std::string fsSource = "\
		precision mediump float;\
		varying vec3 v_color;\
		void main()\
		{\
			gl_FragColor = vec4(v_color, 1.0);\
		}";
		auto fs = Utils::compileShader(fsSource, GL_FRAGMENT_SHADER);
		assert(fs > 0);

		auto program = Utils::linkProgram(vs, fs);
		assert(program > 0);
		glUseProgram(program);

		//
		static float positions[] =
		{
			-1.0f,  1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			 1.0f, -1.0f, 1.0f,
			 1.0f,  1.0f, 1.0f,

			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f
		};
		auto positionLocation = glGetAttribLocation(program, "a_position");
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, positions);
		glEnableVertexAttribArray(positionLocation);

		//
		static GLubyte colors[] = {
			0,   0, 0,
			0,   0, 255,
			0, 255, 0,
			0, 255, 255,

			255,   0,   0,
			255,   0, 255,
			255, 255,   0,
			255, 255, 255

		};
		auto colorLocation = glGetAttribLocation(program, "a_color");
		assert(colorLocation >= 0);
		glVertexAttribPointer(colorLocation, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
		glEnableVertexAttribArray(colorLocation);

		//
		m_matrixLocation = glGetUniformLocation(program, "u_matrix");
		assert(m_matrixLocation >= 0);

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
		reder();
		return update();

	}

	void onResized(int newWidth, int newHeight)
	{
		m_width = newWidth;
		m_height = newHeight;
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
	void reder()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_width, m_height);

		const float h = 1.0f;
		const float w = h * m_width/m_height;

		const Matrix matrix =
			Matrix::frustum(-w / 2, w / 2, -h / 2, h / 2, 1.0f, 50.0f)
			* Matrix::translate(0.0f, 0.0f, -4.0f + m_distance)
			* m_rotationMatrix;
		glUniformMatrix4fv(m_matrixLocation, 1, GL_FALSE, matrix.data());

		const GLbyte indices[] =
		{
			0 , 1, 2, 0, 2, 3,  // float
			4 , 6, 5, 4, 7, 6,  // back
			0 , 5, 1, 0, 4, 5,  // left
			3 , 2, 6, 3, 6, 7,  // right
			0 , 3, 4, 4, 3, 7,  // top
			1 , 6, 2, 1, 5, 6,  // bottom
		};
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

		m_graphic.swapBuffers();
	}
};