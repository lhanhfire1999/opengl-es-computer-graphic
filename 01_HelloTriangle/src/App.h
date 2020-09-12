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

public:
	App(Graphic& graphic, int width, int height) : m_graphic(graphic), m_width(width), m_height(height)
	{
		const std::string vsSource = "\
		attribute vec2 a_position;\
		void main()\
		{\
			gl_Position = vec4(a_position, 0.0, 1.0);\
		}";
		auto vs = Utils::compileShader(vsSource, GL_VERTEX_SHADER);
		assert(vs > 0);

		const std::string fsSource = "\
		precision mediump float;\
		void main()\
		{\
			gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
		return true;
		
		
	
	}
	void reder()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, m_width, m_height);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		m_graphic.swapBuffers();
	}
};