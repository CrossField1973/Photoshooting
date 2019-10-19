#include "MainWindow.h"
#include "Graphics.h"
#include <fstream>
#include <string>

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

void messageLoop(Graphics& graphics, Sphere& sphere)
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
			const float delta = 0.001f;

			graphics.startDraw();
			sphere.draw();
			sphere.rotate(delta, 0.0f, 0.0f);
			sphere.move(0.0f, 0.0f, -delta / 10);

			/*static float rot = 0.0f;
			graphics.moveLight(rot);
			graphics.startDraw();
			graphics.setCube();
			graphics.setCBuffer(rot, '2');
			graphics.drawCube();
			graphics.setCBuffer(rot, '3');
			graphics.drawCube();
			graphics.setCBuffer(rot, '4');
			graphics.drawCube();
			graphics.setCBuffer(rot, '5');
			graphics.drawCube();
			graphics.endDraw();
			rot += 0.01;*/
			graphics.endDraw();
		}
	}
	graphics.cleanUp();
	graphics.debugReportLiveObject();
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