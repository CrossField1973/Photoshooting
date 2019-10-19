#include "Graphics.h"
#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* pCmdLine, int nCmdShow)
{
	InitMainWindow( hInstance, nCmdShow, true);
	static Graphics graphics(hHwnd, iWidth, iHeight, true);

	graphics.initCube('1');
	Sphere sphere(graphics, 16, 32);
	messageLoop(graphics, sphere);

	return 0;
}
