#include "MainWindow.h"
#include "Graphics.h"
#include <fstream>
#include <string>
#include <math.h>

int iWidth = 1280;
int iHeight = 720;
LPCTSTR sWndClassName = L"normalWindow";
HWND hHwnd = NULL;
MSG mMsg;

bool InitMainWindow(HINSTANCE hInstance, int nCmdShow, bool windowed)
{
	//Read (Global) Variables from 'preferences.ini'
	std::ifstream prefFile ("preferences.ini", std::ios::in);
	std::string line;
	
	if (prefFile.is_open())
	{
		while (std::getline(prefFile, line))
		{
			if (line.find("cWidth") != std::string::npos)
			{
				iWidth = stoi(line.substr(line.find(" ") + 1, line.length()));
			}

			else if (line.find("cHeight") != std::string::npos)
			{
				iHeight = stoi(line.substr(line.find(" ") + 1, line.length()));
			}
		}
		prefFile.close();
	}

	else
	{
		std::ofstream prefFile;
		prefFile.open("preferences.ini");
		prefFile << "cWidth: " << iWidth << std::endl;
		prefFile << "cHeight: " << iHeight << std::endl;
		prefFile.close();
	}


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

void messageLoop(Graphics& graphics, Sphere& sphere, Cube& cube)
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
			static float delta = DirectX::XM_PI / 360.0f;
			static float degree = 0.0f;

			graphics.startDraw();

			graphics.moveLight(0.0f, 0.0f, 1.8f * std::sin(degree) + 2.5f);
			sphere.move(0.0f, 0.0f, 1.8f * std::sin(degree) + 2.5f);

			cube.move(-1.0f, 0.0f, 0.0f);
			cube.draw();

			cube.move(2.0f, 0.0f, 0.0f);
			cube.draw();

			cube.move(-1.0f, 1.0f, 0.0f);
			cube.draw();

			cube.move(0.0f, -2.0f, 0.0f);
			cube.draw();

			cube.move(0.0f, 1.0f, 0.0f);
			cube.rotate(delta, 0.0f, 0.0f);

			sphere.draw();

			graphics.endDraw();

			degree += 0.005;
		}
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}