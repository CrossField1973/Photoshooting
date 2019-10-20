#include "Cube.h"

extern int iWidth;
extern int iHeight;
extern float FoV;
extern float aspectRatio;

using namespace DirectX;

Cube::Cube(Graphics& graphics)
{
	ZeroMemory(&vertexBuffer, sizeof(vertexBuffer));
	ZeroMemory(&indexBuffer, sizeof(indexBuffer));
	ZeroMemory(&inputLayout, sizeof(inputLayout));
	ZeroMemory(&modelViewProj, sizeof(modelViewProj));
	ZeroMemory(&device, sizeof(device));
	ZeroMemory(&context, sizeof(context));
	ZeroMemory(&vs, sizeof(vs));
	ZeroMemory(&ps, sizeof(ps));
	ZeroMemory(&debug, sizeof(debug));

	device = graphics.getDevice();
	graphics.getDevice()->AddRef();
	context = graphics.getContext();
	graphics.getContext()->AddRef();

	Vertex vertices[] =
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
		auto v0 = vertices[indices[i]];
		auto v1 = vertices[indices[i + 1]];
		auto v2 = vertices[indices[i + 2]];
		const auto p0 = XMLoadFloat3(&v0.pos);
		const auto p1 = XMLoadFloat3(&v1.pos);
		const auto p2 = XMLoadFloat3(&v2.pos);

		const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

		XMStoreFloat3(&vertices[indices[i]].normal, n);
		XMStoreFloat3(&vertices[indices[i + 1]].normal, n);
		XMStoreFloat3(&vertices[indices[i + 2]].normal, n);
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));
	indexBufferData.pSysMem = &indices;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

	ID3DBlob* blob = nullptr;
	D3DReadFileToBlob(L"PixelShader.cso", &blob);
	device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ps);
	blob->Release();

	D3DReadFileToBlob(L"VertexShader.cso", &blob);
	device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vs);

	static D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static UINT numElements = ARRAYSIZE(layout);

	device->CreateInputLayout(layout, numElements, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
	blob->Release();

	numIndices = ARRAYSIZE(indices);

	XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixLookAtLH(Eye, At, Up)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)  * XMMatrixLookAtLH(Eye, At, Up) * XMMatrixPerspectiveFovLH(FoV, aspectRatio, 0.5f, 10.0f))
	};

	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufferDesc.CPUAccessFlags = 0;
	constBufferDesc.ByteWidth = sizeof(cb);
	constBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));
	srd.pSysMem = &cb;
	device->CreateBuffer(&constBufferDesc, &srd, &modelViewProj);

	device->QueryInterface(IID_PPV_ARGS(&debug));
}

Cube::~Cube()
{
	vertexBuffer->Release();
	indexBuffer->Release();
	modelViewProj->Release();
	inputLayout->Release();
	vs->Release();
	ps->Release();
	device->Release();
	context->Release();

	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	debug->Release();
}

void Cube::reset()
{
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 2.0f;
	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotation.z = 0.0f;
	scaling.x = 0.7f;
	scaling.y = 0.7f;
	scaling.z = 0.7f;
}

void Cube::draw()
{
	context->IASetInputLayout(inputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->VSSetConstantBuffers(0, 1, &modelViewProj);

	context->VSSetShader(vs, nullptr, 0);
	context->PSSetShader(ps, nullptr, 0);

	context->DrawIndexed(numIndices, 0, 0);
}

void Cube::move(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixPerspectiveLH(FoV, aspectRatio, 0.5f, 10.0f))
	};
	context->UpdateSubresource(modelViewProj, 0, 0, &cb, 0, 0);
}

void Cube::rotate(float x, float y, float z)
{
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;

	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixPerspectiveLH(FoV, aspectRatio, 0.5f, 10.0f))
	};
	context->UpdateSubresource(modelViewProj, 0, 0, &cb, 0, 0);
}

void Cube::scale(float x, float y, float z)
{
	scaling.x += x;
	scaling.y += y;
	scaling.z += z;

	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixPerspectiveLH(FoV, aspectRatio, 0.5f, 10.0f))
	};
	context->UpdateSubresource(modelViewProj, 0, 0, &cb, 0, 0);
}
