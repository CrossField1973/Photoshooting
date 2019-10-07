#include "MainWindow.h"
#include "Graphics.h"

int iWidth = 1066;
int iHeight = 600;
LPCTSTR sWndClassName = L"normalWindow";
HWND hHwnd = NULL;
MSG mMsg;

bool InitMainWindow(HINSTANCE hInstance, int nCmdShow, bool windowed)
{
	//Windows Klasse erstellen
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//Windows Klasse einrichten
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = sWndClassName;

	//Registriere Klasse
	RegisterClassEx(&wc);

	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));
	rect.right = iWidth;
	rect.bottom = iHeight;
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, NULL, NULL);

	hHwnd = CreateWindowEx( NULL, 
							sWndClassName, 
							L"Photoshooting", 
							WS_OVERLAPPEDWINDOW, 
							CW_USEDEFAULT, 
							CW_USEDEFAULT, 
							rect.right - rect.left, 
							rect.bottom - rect.top, 
							NULL, 
							NULL, 
							hInstance, 
							NULL);

	if (!hHwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//Fenster zeigen und einmal updaten
	ShowWindow(hHwnd, nCmdShow);
	UpdateWindow(hHwnd);

	return true;
}

void messageLoop(Graphics& graphics)
{
	ZeroMemory(&mMsg, sizeof(MSG));
	while (true)
	{
		//Nach Nachrichten schauen
		if (PeekMessage(&mMsg, NULL, 0, 0, PM_REMOVE))
		{
			if (mMsg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&mMsg);
			DispatchMessage(&mMsg);
		}

		//Run Game Code
		else
		{
			graphics.drawScene();
			graphics.updateScene();
		}
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}