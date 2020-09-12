#pragma once
#include <WindowListener.h>
#include <Graphic.h>
#include <Utils.h>
#include <string>
#include <cassert>

class App : public WindowListener
{
private:
	Graphic& m_graphic;
	int m_width, m_height;
	GLint m_matrixLocation;
	float m_angle = 0.0;

public:
	App(Graphic& graphic, int width, int height) : m_graphic(graphic), m_width(width), m_height(height)
	{
		const std::string vsSource = "\
		attribute vec2 a_position;\
		attribute vec3 a_color;\
		varying vec3 v_color;\
		uniform mat4 u_matrix;\
		void main()\
		{\
			gl_Position = u_matrix * vec4(a_position, 0.0, 1.0);\
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

		static float positions[] =
		{
			0.0f,1.0f,
			-1.0f,-1.0f,
			1.0f,-1.0f
		};
		auto positionLocation = glGetAttribLocation(program, "a_position");
		assert(positionLocation >= 0);
		glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, positions);
		glEnableVertexAttribArray(positionLocation);

		static GLubyte colors[] =
		{
			255,0,0,
			0,255,0,
			0,0,255
		};
		auto colorLocation = glGetAttribLocation(program, "a_color");
		assert(colorLocation >= 0);
		glVertexAttribPointer(colorLocation, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
		glEnableVertexAttribArray(colorLocation);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		m_matrixLocation = glGetUniformLocation(program, "u_matrix");
		assert(m_matrixLocation >= 0);
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

private:
	bool update()
	{
		if (m_angle <= 359)
			m_angle += 5;
		else
			m_angle = 0;
		return true;
		
		
	
	}
	void reder()
	{
		#define PI 3.14159f
		printf("*angle= %f\n", m_angle);
		float rotation[16] =
		{
			cos(m_angle*PI/180),sin(m_angle * PI / 180),0,0,
			-sin(m_angle * PI / 180),cos(m_angle * PI / 180),0,0,
			0,0,1,0,
			0,0,0,1
		};

		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, m_width, m_height);

		glUniformMatrix4fv(m_matrixLocation, 1, GL_FALSE, rotation);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		m_graphic.swapBuffers();
	}
};