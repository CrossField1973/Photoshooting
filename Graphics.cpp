#include "Graphics.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <cassert>
#include <vector>

#pragma comment(lib, "D3DCompiler.lib")

extern int iHeight;
extern int iWidth;

//The vertex Structure


Graphics::Graphics(HWND hwnd, int width, int height, bool windowed)
{
	DXGI_MODE_DESC swapChainBufferDesc;
	ZeroMemory(&swapChainBufferDesc, sizeof(DXGI_MODE_DESC));

	swapChainBufferDesc.Width = 0;
	swapChainBufferDesc.Height = 0;
	swapChainBufferDesc.RefreshRate.Numerator = 120;
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
}

Graphics::~Graphics()
{
	if (pSwapChain)
	{
		pSwapChain->Release();
	}

	if (pDevice)
	{
		pDevice->Release();
	}

	if (pRenderTargetView)
	{
		pRenderTargetView->Release();
	}

	if (pContext)
	{
		pContext->Release();
	}

	if (blob)
	{
		blob->Release();
	}

	if (vs)
	{
		vs->Release();
	}

	if (ps)
	{
		ps->Release();
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
	}

	if (cubeVertexBuffer1)
	{
		cubeVertexBuffer1->Release();
	}

	if (cubeVertexBuffer2)
	{
		cubeVertexBuffer2->Release();
	}
}

void Graphics::initScene()
{
}

void Graphics::initTriangle(float size, float screenRatio)
{
	ID3D11Buffer* triangleVertexBuffer;

	Vertex triangle[] =
	{
		{DirectX::XMFLOAT3(0.0f * size / screenRatio, 0.0f * size, 0.0f)},
		{DirectX::XMFLOAT3(0.0f * size / screenRatio, 1.0f * size, 0.0f)},
		{DirectX::XMFLOAT3(1.0f * size / screenRatio, 0.0f * size, 0.0f)},
	};

	

	D3D11_SUBRESOURCE_DATA triangleBufferData;
	ZeroMemory(&triangleBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	triangleBufferData.pSysMem = triangle;

	D3D11_BUFFER_DESC triangleBufferDesc;
	ZeroMemory(&triangleBufferDesc, sizeof(D3D11_BUFFER_DESC));

	triangleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	triangleBufferDesc.ByteWidth = sizeof(triangle);
	triangleBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	triangleBufferDesc.CPUAccessFlags = 0;
	triangleBufferDesc.MiscFlags = 0;

	pDevice->CreateBuffer(&triangleBufferDesc, &triangleBufferData, &triangleVertexBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &triangleVertexBuffer, &stride, &offset);

	D3DReadFileToBlob(L"PixelShader.cso", &blob);
	pDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ps);
	blob->Release();

	D3DReadFileToBlob(L"VertexShader.cso", &blob);
	pDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vs);

	pContext->VSSetShader(vs, nullptr, NULL);
	pContext->PSSetShader(ps, nullptr, NULL);

	ID3D11InputLayout* inputLayout;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);

	pDevice->CreateInputLayout(layout, numElements, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
	pContext->IASetInputLayout(inputLayout);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	inputLayout->Release();
	triangleVertexBuffer->Release();
}

void Graphics::initCube(char bufferNumber)
{
	using namespace DirectX;

	//ID3D11Buffer* cubeVertexBuffer;
	ID3D11Buffer* cubeIndexBuffer;

	using namespace DirectX;
	Vertex cube[] =
	{
		{XMFLOAT3(-1.0f, -1.0f, -1.0)}, 
		{XMFLOAT3(1.0f, -1.0f, -1.0)}, 
		{XMFLOAT3(-1.0f, 1.0f, -1.0)},  
		{XMFLOAT3(1.0f, 1.0f, -1.0)},  
		{XMFLOAT3(-1.0f, -1.0f, 1.0)},	 
		{XMFLOAT3(1.0f, -1.0, 1.0)},	 
		{XMFLOAT3(-1.0f, 1.0, 1.0)},    
		{XMFLOAT3(1.0f, 1.0f, 1.0)},
		{XMFLOAT3(-1.0f, -1.0f, -1.0)},
		{XMFLOAT3(-1.0f, 1.0f, -1.0)},
		{XMFLOAT3(-1.0f, -1.0f, 1.0)},
		{XMFLOAT3(-1.0f, 1.0f, 1.0)},
		{XMFLOAT3(1.0f, -1.0, -1.0)},
		{XMFLOAT3(1.0f, 1.0, -1.0)},
		{XMFLOAT3(1.0f, -1.0, 1.0)},
		{XMFLOAT3(1.0f, 1.0f, 1.0)},
		{XMFLOAT3(-1.0f, -1.0f, -1.0)},
		{XMFLOAT3(1.0f, -1.0f, -1.0)},
		{XMFLOAT3(-1.0f, -1.0f, 1.0)},
		{XMFLOAT3(1.0f, -1.0f, 1.0)},
		{XMFLOAT3(-1.0f, 1.0, -1.0)},
		{XMFLOAT3(1.0f, 1.0, -1.0)},
		{XMFLOAT3(-1.0f, 1.0, 1.0)},
		{XMFLOAT3(1.0f, 1.0, 1.0)}
	};

	UINT indices[] =
	{
		0,2,1, 2,3,1,
		4,5,7, 4,7,6,
		8,10,9, 10,11,9,
		12,13,15, 12,15,14,
		16,17,18, 18,17,19,
		20,23,21, 20,22,23
	};

	//Set Normals
	for (size_t i = 0; i < (sizeof(indices) / sizeof(UINT)); i += 3)
	{
		auto v0 = cube[indices[i]];
		auto v1 = cube[indices[i + 1]];
		auto v2 = cube[indices[i + 2]];
		const auto p0 = XMLoadFloat3(&v0.pos);
		const auto p1 = XMLoadFloat3(&v1.pos);
		const auto p2 = XMLoadFloat3(&v2.pos);

		const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

		XMStoreFloat3(&cube[indices[i]].normal, n);
		XMStoreFloat3(&cube[indices[i + 1]].normal, n);
		XMStoreFloat3(&cube[indices[i + 2]].normal, n);
	}

	D3D11_SUBRESOURCE_DATA cubeBufferData;
	ZeroMemory(&cubeBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	cubeBufferData.pSysMem = &cube;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = &indices;


	D3D11_BUFFER_DESC cubeBufferDesc;
	ZeroMemory(&cubeBufferDesc, sizeof(D3D11_BUFFER_DESC));

	cubeBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeBufferDesc.ByteWidth = sizeof(cube);
	cubeBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	cubeBufferDesc.CPUAccessFlags = 0;
	cubeBufferDesc.MiscFlags = 0;

	if (bufferNumber == '1')
	{
		pDevice->CreateBuffer(&cubeBufferDesc, &cubeBufferData, &cubeVertexBuffer1);
	}

	if (bufferNumber == '2')
	{
		pDevice->CreateBuffer(&cubeBufferDesc, &cubeBufferData, &cubeVertexBuffer2);
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &cubeIndexBuffer);


	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;
	//pContext->IASetVertexBuffers(0, 1, &cubeVertexBuffer, &stride, &offset);

	pContext->IASetIndexBuffer(cubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3DReadFileToBlob(L"PixelShader.cso", &blob);
	pDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ps);
	blob->Release();

	D3DReadFileToBlob(L"VertexShader.cso", &blob);
	pDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vs);

	pContext->VSSetShader(vs, nullptr, NULL);
	pContext->PSSetShader(ps, nullptr, NULL);

	ID3D11InputLayout* inputLayout;

	static D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static UINT numElements = ARRAYSIZE(layout);

	pDevice->CreateInputLayout(layout, numElements, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
	pContext->IASetInputLayout(inputLayout);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	inputLayout->Release();
	//cubeVertexBuffer->Release();
	cubeIndexBuffer->Release();
}

void Graphics::updateTriangle()
{

}

void Graphics::drawTriangle()
{
	pContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pContext->ClearRenderTargetView(pRenderTargetView, clearColor);
	pContext->Draw(3u, 0u);
	pSwapChain->Present(0, 0);
}

void Graphics::drawCube()
{
	pContext->DrawIndexed(36, 0, 0);
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

void Graphics::setCube(char number)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	if (number == '1')
	{
		pContext->IASetVertexBuffers(0, 1, &cubeVertexBuffer1, &stride, &offset);
	}
	else
	{
		pContext->IASetVertexBuffers(0, 1, &cubeVertexBuffer2, &stride, &offset);
	}
}

void Graphics::setCBuffer(float rotation, char number)
{
	using namespace DirectX;

	struct cBufferMatrix
	{
		XMMATRIX model;
		XMMATRIX modelViewProj;
	};

	cBufferMatrix cb;

	if (number == '1')
	{
		cb =
		{
			XMMatrixTranspose(XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationRollPitchYaw(0.0f, rotation / 2.0f, 0.0f) * XMMatrixTranslation(0.0f, 0.0f, 2.0f)),
			XMMatrixTranspose(XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationRollPitchYaw(0.0f, rotation / 2.0f, 0.0f) * XMMatrixTranslation(0.0f, 0.0f, 2.0f) * XMMatrixPerspectiveLH(0.4f * 3.14f, (float)iHeight / (float)iWidth, 0.5f, 10.0f))
		};
	}
	
	else if(number == '2')
	{
		cb =
		{
			XMMatrixTranspose(XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationRollPitchYaw(0.0f, rotation / 2.0f, 0.0f) * XMMatrixTranslation(1.0f, 0.0f, 2.0f)),
			XMMatrixTranspose(XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationRollPitchYaw(0.0f, rotation / 2.0f, 0.0f) * XMMatrixTranslation(1.0f, 0.0f, 2.0f) * XMMatrixPerspectiveLH(0.4f * 3.14f, (float)iHeight / (float)iWidth, 0.5f, 10.0f))
		};
	}

	ID3D11Buffer* constantBuffer;
	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.ByteWidth = sizeof(cb);
	constBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));
	srd.pSysMem = &cb;
	pDevice->CreateBuffer(&constBufferDesc, &srd, &constantBuffer);
	pContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	constantBuffer->Release();
}

void Graphics::moveLight(float xTransform)
{
	struct constantBufferLight
	{
		alignas(16) float element[3];
	};

	constantBufferLight cbLPos =
	{
		{
			0.0f, 0.0f, 1.2f
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
