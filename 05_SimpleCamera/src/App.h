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

	int m_numVertices;
	float* m_vertices;
	static const int STRIDE = sizeof(float) * 5;

	int m_matrixLocaiton;
	Matrix m_matrix;

	float m_yRotation;
	float m_xTranslation, m_zTranslation;
	float m_walkBiasAngle, m_yTranslation;

	bool m_exit;

private:
	void loadVertices()
	{
		FILE* f = fopen("world.txt", "rt");
		assert(f);
		int numTriangles;
		fscanf(f, "%d", &numTriangles);
		m_numVertices = numTriangles * 3;
		m_vertices = new float[STRIDE * m_numVertices];
		float* pointer = m_vertices;
		for (int i = 0; i < m_numVertices; i++)
		{
			float x, y, z, u, v;
			fscanf(f, "%f %f %f %f %f", &x, &y, &z, &u, &v);
			pointer[0] = x;
			pointer[1] = y;
			pointer[2] = z;
			pointer[3] = u;
			pointer[4] = v;
			pointer += 5;
		}
		fclose(f);
	}

	void unloadVertices()
	{
		delete[] m_vertices;
	}

public:
	App(Graphic& graphic, int width, int height) : m_graphic(graphic), m_width(width), m_height(height), m_matrix(Matrix::identity())
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

		loadVertices();

		auto positionLocation = glGetAttribLocation(program, "a_position");
		assert(positionLocation >= 0);
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, STRIDE, m_vertices);
		glEnableVertexAttribArray(positionLocation);

		auto texCoordLocation = glGetAttribLocation(program, "a_texCoord");
		assert(texCoordLocation >= 0);
		glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, STRIDE, m_vertices + 3);
		glEnableVertexAttribArray(texCoordLocation);

		m_matrixLocaiton = glGetUniformLocation(program, "u_matrix");
		assert(m_matrixLocaiton >= 0);

		GLuint texture;
		glGenTextures(1, &texture);
		loadTexture(texture, "image.tga");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		auto samplerLocation = glGetUniformLocation(program, "u_sampler");
		assert(samplerLocation >= 0);
		glUniform1i(samplerLocation, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		m_exit = false;

		m_yRotation = 0.0f;
		m_xTranslation = m_yTranslation = m_zTranslation = 0.0f;
		m_walkBiasAngle = 0.0f;
	}

	~App()
	{
		unloadVertices();
	}
private:
	void loadTexture(GLuint texture, const char* file)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		auto tga = Tga(file);
		assert(tga.okay());
		auto format = tga.hasAlpha() ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, tga.width(), tga.height(), 0, format, GL_UNSIGNED_BYTE, tga.data());
	}

public:
	bool tick()
	{
		render();
		return update();
	}

	void onResized(int newWidth, int newHeight)
	{
		m_width = newWidth;
		m_height = newHeight;
	}

	void onKeyDown(int keycode)
	{
		const float yRotationStep = 5.0f;
		const float walkStep = 1.0f;
		switch (keycode)
		{
		case 'S':
		case VK_DOWN:
			m_xTranslation += sinDeg(m_yRotation) * walkStep;
			m_zTranslation += cosDeg(m_yRotation) * walkStep;
			m_walkBiasAngle -= 10.0f;
			if (m_walkBiasAngle < 0.0f) m_walkBiasAngle += 360.0f;
			m_yTranslation = sinDeg(m_walkBiasAngle) / 20.0f + 0.25f;
			break;
		case 'W':
		case VK_UP:
			m_xTranslation -= sinDeg(m_yRotation) * walkStep;
			m_zTranslation -= cosDeg(m_yRotation) * walkStep;
			m_walkBiasAngle += 10.0f;
			if (m_walkBiasAngle > +360.0f) m_walkBiasAngle -= 360.0f;
			m_yTranslation = sinDeg(m_walkBiasAngle) / 20.0f + 0.25f;
			break;
		case 'A':
		case VK_LEFT:
			m_yRotation += yRotationStep;
			break;
		case 'D':
		case VK_RIGHT:
			m_yRotation -= yRotationStep;
			break;
		case VK_SPACE:
			m_yRotation = 0.0f;
			m_xTranslation = m_yTranslation = m_zTranslation = 0.0f;
			m_walkBiasAngle = 0.0f;
			break;
		case VK_ESCAPE:
			m_exit = true;
			break;
		}
	}

private:
	bool update()
	{
		return !m_exit;
	}

	void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_width, m_height);
		auto matrix =
			Matrix::perspective(45.0f, (float)m_width / (float)m_height, 0.1f, 100.0f)
			* Matrix::rotation(-m_yRotation, 0.0f, 1.0f, 0.0f)
			* Matrix::translate(-m_xTranslation, -m_yTranslation, -m_zTranslation);
		glUniformMatrix4fv(m_matrixLocaiton, 1, GL_FALSE, matrix.data());
		glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
		m_graphic.swapBuffers();
	}

};

