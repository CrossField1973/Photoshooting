#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

class Graphics
{
public:
	Graphics(HWND hwnd, int width, int height, bool windowed);
	~Graphics();
	void startDraw();
	void endDraw();
	void moveLight(float x, float y, float z);
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getContext();

private:
	ID3D11Debug* debug = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };//0.2,0.0,0.0,1.0
	float lightPos[3] = { 0.0f, 0.0f, 0.0f };
};

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
};

struct cBufferMatrix
{
	DirectX::XMMATRIX model;
	DirectX::XMMATRIX modelViewProj;
};