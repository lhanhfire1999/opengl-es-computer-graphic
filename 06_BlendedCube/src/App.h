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
	bool m_blendEnabled;
	GLuint m_textures[6];

	bool m_isgoingfar;
	bool m_moving;

	float m_opacity;
	float m_opacityLocation;


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
#define A -1.0f, -1.0f,  1.0f
#define B  1.0f, -1.0f,  1.0f
#define C  1.0f,  1.0f,  1.0f
#define D -1.0f,  1.0f,  1.0f

#define E  1.0f, -1.0f, -1.0f
#define F -1.0f, -1.0f, -1.0f
#define G -1.0f,  1.0f, -1.0f
#define H  1.0f,  1.0f, -1.0f

		static float positions[] =
		{
			A, B, C, D, // front
			E, F, G, H, // back
			F, A, D, G, // left
			B, E, H, C, // right
			D, C, H, G, // top
			F, E, B, A, // bottom
		};
		auto positionLocation = glGetAttribLocation(program, "a_position");
		assert(positionLocation >= 0);
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, positions);
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
		glGenTextures(6, m_textures);
		int i = 0;
		loadTexture(m_textures[i++], "ngoctrinh.tga");
		loadTexture(m_textures[i++], "haho.tga");
		loadTexture(m_textures[i++], "hatang.tga");
		loadTexture(m_textures[i++], "maiphuongthuy.tga");
		loadTexture(m_textures[i++], "buiphuongnga.tga");
		loadTexture(m_textures[i++], "midu.tga");

		auto samplerLocation = glGetUniformLocation(program, "u_sampler");
		assert(samplerLocation >= 0);
		glUniform1i(samplerLocation, 0);

		glActiveTexture(GL_TEXTURE0);
		//


		//
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//
		m_blendEnabled = true;
		glEnable(GL_BLEND);
		glBlendColor(0.0f, 0.0f, 0.0f, 0.5f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		//
		m_distance = 0.0f;
		m_exit = false;

		m_isgoingfar = true;
		m_moving = false;

		m_opacity = 0.5f;



		m_opacityLocation = glGetUniformLocation(program, "u_opacity");
		assert(samplerLocation >= 0);
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
		case VK_ESCAPE:
			m_exit = true;
			break;
		case 'B':
			m_blendEnabled = !m_blendEnabled;
			if (m_blendEnabled)
			{
				glEnable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			}
			break;
		case 'N':
			m_opacity -= 0.05f;
			if (m_opacity < 0)
			{
				m_opacity = 0;
			}
			break;

		case'M':
			m_opacity += 0.05f;
			if (m_opacity > 1)
			{
				m_opacity = 1;
			}
			break;

		case VK_TAB:
			m_moving = !m_moving;
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
				m_rotationMatrix = Matrix::rotation(rotationStep, 0.0f, 1.0f, 0.0f) * m_rotationMatrix;

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

		auto h = 1.0f;
		auto w = h * m_width / m_height;

		auto matrix =
			Matrix::frustum(-w / 2, w / 2, -h / 2, h / 2, 1.0f, 50.0f)
			* Matrix::translate(0.0f, 0.0f, -4.0f + m_distance)
			* m_rotationMatrix;

		glUniformMatrix4fv(m_matrixLocation, 1, GL_FALSE, matrix.data());
		glUniform1f(m_opacityLocation, m_opacity);

		GLubyte indices[] =
		{
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};

		GLubyte* pointer = indices;
		for (int i = 0; i < 6; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, pointer);\
				pointer += 6;
		}

		m_graphic.swapBuffers();
	}

};
