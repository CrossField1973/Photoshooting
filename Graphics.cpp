#include "Graphics.h"
#include <cassert>
#include <vector>

extern int iHeight;
extern int iWidth;
float FoV = DirectX::XM_PIDIV4;
float aspectRatio = (float)iHeight / (float)iWidth;

Graphics::Graphics(HWND hwnd, int width, int height, bool windowed)
{
	DXGI_MODE_DESC swapChainBufferDesc;
	ZeroMemory(&swapChainBufferDesc, sizeof(DXGI_MODE_DESC));

	swapChainBufferDesc.Width = 0;
	swapChainBufferDesc.Height = 0;
	swapChainBufferDesc.RefreshRate.Numerator = 60;
	swapChainBufferDesc.RefreshRate.Denominator = 1;
	swapChainBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = swapChainBufferDesc;
	swapChainDesc.SampleDesc.Count = 8;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = windowed;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//Create our SwapChain
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0,
		D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext);

	//Create our BackBuffer
	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& pBackBuffer);

	//Create our Render Target
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	pBackBuffer->Release();

	//Set our Render Target
	pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
	

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	//Set the Viewport
	pContext->RSSetViewports(1, &viewport);


	ID3D11Texture2D* depthStencilBuffer = nullptr;

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 8;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	pDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	pDevice->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	depthStencilBuffer->Release();

	pContext->OMSetRenderTargets(1, &pRenderTargetView, depthStencilView);

	//pDevice->QueryInterface(IID_PPV_ARGS(&debug));
}

Graphics::~Graphics()
{
	pSwapChain->Release();
	pSwapChain = nullptr;

	pDevice->Release();
	pDevice->Release();
	pDevice = nullptr;

	pRenderTargetView->Release();
	pRenderTargetView = nullptr;

	pContext->Release();
	pContext = nullptr;

	depthStencilView->Release();
	depthStencilView = nullptr;

	/*if (debug)
	{
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		debug->Release();
		debug = nullptr;
	}*/
}

void Graphics::startDraw()
{
	pContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pContext->ClearRenderTargetView(pRenderTargetView, clearColor);
}

void Graphics::endDraw()
{
	pSwapChain->Present(0, 0);
}

void Graphics::moveLight(float x, float y, float z)
{
	lightPos[0] = x;
	lightPos[1] = y;
	lightPos[2] = z;

	struct constantBufferLight
	{
		alignas(16) float element[3];
	};

	constantBufferLight cbLPos =
	{
		{
			lightPos[0], lightPos[1], lightPos[2]
		}
	};
	ID3D11Buffer* cbL;

	D3D11_SUBRESOURCE_DATA cbData;
	ZeroMemory(&cbData, sizeof(D3D11_SUBRESOURCE_DATA));
	cbData.pSysMem = &cbLPos;

	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));

	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.ByteWidth = sizeof(cbLPos);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	pDevice->CreateBuffer(&cbDesc, &cbData, &cbL);
	pContext->PSSetConstantBuffers(0, 1, &cbL);
	cbL->Release();
}

ID3D11Device* Graphics::getDevice()
{
	return pDevice;
}

ID3D11DeviceContext* Graphics::getContext()
{
	return pContext;
}
