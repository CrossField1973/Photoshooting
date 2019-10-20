#include "Graphics.h"
#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	InitMainWindow( hInstance, nCmdShow, true);
	Graphics graphics(hHwnd, iWidth, iHeight, true);
	Cube cube(graphics);
	cube.scale(-0.5f, -0.5f, -0.5f);
	cube.move(0.0f, 0.0f, 2.5f);
	Sphere sphere(graphics, 16, 32);
	sphere.scale(-0.9f, -0.9f, -0.9f);
	graphics.moveLight(0.0f, 0.0f, 2.5f);
	sphere.move(0.0f, 0.0f, 2.5f);

	messageLoop(graphics, sphere, cube);

	return 0;
}
