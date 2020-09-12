#include <stdio.h>
#include <Windows.h>
#include <Window.h>
#include <Graphic.h>
#include <Utils.h>
#include "App.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	Utils::showConsole();

	const int WIDTH = 800, HEIGHT = 480;
	const bool RESIZABLE = true;
	Window window(hInstance, WIDTH, HEIGHT, RESIZABLE, "Colorful Cube");

	Graphic graphic(window.surface());
	App app(graphic, WIDTH, HEIGHT);

	window.show(30, &app, nShowCmd);
	return 0;
}
