#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
<<<<<<< HEAD
#include <d3dcompiler.h>
=======
#include <dxgidebug.h>
>>>>>>> 7ea4a51a859acda2bcb04159f053fbe06d3ff73e
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

class Graphics
{
public:
	Graphics(HWND hwnd, int width, int height, bool windowed);
	void initScene();
	void initTriangle(float size, float screenRatio);
	void initCube(char bufferNumber);
	void updateTriangle();
	void drawTriangle();
	void drawCube();
	void startDraw();
	void endDraw();
	void setCube();
	void setCBuffer(float rotation, char number);
	void moveLight(float xTransform);
<<<<<<< HEAD
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getContext();
=======
	void debugReportLiveObject();
	void cleanUp();
>>>>>>> 7ea4a51a859acda2bcb04159f053fbe06d3ff73e

private:
	ID3D11Debug* debug;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;
	ID3DBlob* blob = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	ID3D11Buffer* cubeVertexBuffer1 = nullptr;

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };//0.2,0.0,0.0,1.0
};

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
};