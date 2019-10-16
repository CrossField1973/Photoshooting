#include "Graphics.h"
#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* pCmdLine, int nCmdShow)
{
	InitMainWindow( hInstance, nCmdShow, true);
	static Graphics graphics(hHwnd, iWidth, iHeight, true);
	ID3D11Buffer* cubeVertexBuffer1 = nullptr;
	ID3D11Buffer* cubeVertexBuffer2 = nullptr;

	graphics.initCube('1');
	//graphics.initCube(-0.1f, -0.1f, -2.0f, 1.0f, '2'); // (float)16 / 9
	messageLoop(graphics);

	return 0;
}
