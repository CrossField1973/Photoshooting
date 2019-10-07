#pragma once
#include <wrl.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class Graphics
{
public:
	Graphics(HWND hwnd, int width, int height, bool windowed);
	~Graphics();
	void initScene();
	void updateScene();
	void drawScene();
private:
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;

	float clearColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
};

