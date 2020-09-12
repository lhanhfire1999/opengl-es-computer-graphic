#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cassert>
#include <stdio.h>

class Graphic
{
private:
	EGLDisplay m_display;
	EGLSurface m_surface;
	EGLContext m_context;

public:
	Graphic(void* nativeSurface)
	{
		m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		assert(m_display != EGL_NO_DISPLAY);

		EGLint majorVersion, minorVersion;
		auto okay = eglInitialize(m_display, &majorVersion, &minorVersion);
		assert(okay);
		printf("EGL_VERSION: %d.%d\n", majorVersion, minorVersion);

		EGLint attributes[] =
		{
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 16,
			EGL_STENCIL_SIZE, 8,
			EGL_NONE
		};
		EGLConfig config;
		EGLint numConfigs;
		okay = eglChooseConfig(m_display, attributes, &config, 1, &numConfigs);
		assert(okay);

		m_surface = eglCreateWindowSurface(m_display, config, (EGLNativeWindowType)nativeSurface, NULL);
		assert(m_surface != EGL_NO_SURFACE);

		EGLint contextAttributes[] =
		{
			EGL_CONTEXT_MAJOR_VERSION, 2,
			EGL_CONTEXT_MINOR_VERSION, 0,
			EGL_NONE
		};
		m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, contextAttributes);
		assert(m_context != EGL_NO_CONTEXT);

		makeCurrent();

		printGLString("GL_VERSION", GL_VERSION);
		printGLString("GL_VENDOR", GL_VENDOR);
		printGLString("GL_RENDERER", GL_RENDERER);
		printGLString("GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION);
		printGLString("GL_EXTENSIONS", GL_EXTENSIONS);
	}

	void makeCurrent()
	{
		auto okay = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
		assert(okay);
	}

	void swapBuffers()
	{
		auto okay = eglSwapBuffers(m_display, m_surface);
		assert(okay);
	}

private:
	void printGLString(const char* name, GLenum s)
	{
		const char* v = (const char*)glGetString(s);
		printf("%s: %s\n", name, v);
	}

};
