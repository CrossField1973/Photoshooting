#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgidebug.h>
#pragma comment(lib, "d3d11.lib")

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
	void setCube(char number);
	void setCBuffer(float rotation, char number);
	void moveLight(float xTransform);
	void debugReportLiveObject();
	void cleanUp();

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
	ID3D11Buffer* cubeVertexBuffer2 = nullptr;

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
};