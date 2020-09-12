#pragma once

#include <Windows.h>
#include "WindowListener.h"
#include <cassert>

class Window
{
private:
	HWND m_hwnd;
	WindowListener* m_listener;

public:
	Window(HINSTANCE hInst, int width, int height, bool resizable, const char* title) : m_listener(NULL)
	{
		auto WIN_CLASS = "WinClass";
		WNDCLASS wClass = { 0 };
		wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wClass.hInstance = hInst;
		wClass.lpfnWndProc = (WNDPROC)internalWindProc;
		wClass.lpszClassName = WIN_CLASS;

		auto okay = RegisterClass(&wClass);
		assert(okay);

		int dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		if (resizable) dwStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;

		RECT rect; rect.left = 0; rect.top = 0; rect.right = width; rect.bottom = height;
		auto adjustWindowRectOkay = AdjustWindowRect(&rect, dwStyle, FALSE);
		assert(adjustWindowRectOkay);

		const int windowWidth = rect.right - rect.left;
		const int windowHeight = rect.bottom - rect.top;

		const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		m_hwnd = CreateWindow(
			WIN_CLASS,
			title,
			dwStyle,
			(screenWidth - windowWidth) / 2,
			(screenHeight - windowHeight) / 2,
			windowWidth,
			windowHeight,
			NULL,
			NULL,
			hInst,
			NULL
		);
		assert(m_hwnd);
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
	}

	~Window()
	{
		DestroyWindow(m_hwnd);
	}

	HWND surface()
	{
		return m_hwnd;
	}

	void show(int fps, WindowListener* listener, int nCmdShow)
	{
		m_listener = listener;

		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		auto last = GetTickCount();
		const unsigned long frameTime = 1000 / fps;

		ShowWindow(m_hwnd, nCmdShow);
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				auto current = GetTickCount();
				auto duration = current - last;
				if (duration < frameTime)
					Sleep(frameTime - duration);
				last = GetTickCount();
				if (!m_listener->tick())
					break;
			}
		}
	}
	
private:
	static LRESULT CALLBACK internalWindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		auto window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		return window->windProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK windProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_SIZE:
		{
			if (m_listener) m_listener->onResized(LOWORD(lParam), HIWORD(lParam));
			return 0;
		}
		case WM_KEYDOWN:
		{
			if (m_listener) m_listener->onKeyDown(wParam);
			return 0;
		}
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

};
