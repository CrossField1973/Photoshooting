#include "Graphics.h"
#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* pCmdLine, int nCmdShow)
{
	InitMainWindow( hInstance, nCmdShow, true);
	static Graphics graphics(hHwnd, iWidth, iHeight, true);
	graphics.initScene();
	messageLoop(graphics);

	return 0;
}
