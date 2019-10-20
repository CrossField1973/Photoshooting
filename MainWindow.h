#pragma once
#include <Windows.h>
#include "Graphics.h"
#include "Sphere.h"
#include "Cube.h"

//Variablen
extern int iWidth;
extern int iHeight;
extern HWND hHwnd;
extern MSG mMsg;

//Funktionen
bool InitMainWindow(HINSTANCE hInstance, int ShowWnd, bool windowed);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void messageLoop(Graphics& graphics, Sphere& sphere, Cube& cube);
