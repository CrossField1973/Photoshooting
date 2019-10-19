#include "Sphere.h"

extern int iWidth;
extern int iHeight;

using namespace DirectX;

Sphere::Sphere(Graphics& graphics, int latDiv, int longDiv)
{
	ZeroMemory(&vertexBuffer, sizeof(vertexBuffer));
	ZeroMemory(&indexBuffer, sizeof(indexBuffer));
	ZeroMemory(&inputLayout, sizeof(inputLayout));
	ZeroMemory(&modelViewProj, sizeof(modelViewProj));
	ZeroMemory(&device, sizeof(device));
	ZeroMemory(&context, sizeof(context));

	device = graphics.getDevice();
	graphics.getDevice()->AddRef();
	context = graphics.getContext();
	graphics.getContext()->AddRef();

	const auto base = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	std::vector<Vertex> vertices;

	float lattitudeAngle = XM_PI / latDiv;
	float longitudeAngle = 2.0f * XM_PI / longDiv;

	for (int iLat = 1; iLat < latDiv; iLat++)
	{
		auto latBase = XMVector3Transform(base, XMMatrixRotationX(lattitudeAngle * iLat));
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			auto v = XMVector3Transform(latBase, XMMatrixRotationZ(longitudeAngle * iLong));
			XMStoreFloat3(&vertices.back().pos, v);
			XMStoreFloat3(&vertices.back().normal, XMVector3Normalize(v));
		}
	}
	UINT iNorthPole = (UINT)vertices.size();
	vertices.emplace_back();
	XMStoreFloat3(&vertices.back().pos, base);
	XMStoreFloat3(&vertices.back().normal, XMVector3Normalize(base));
	
	UINT iSouthPole = (UINT)vertices.size();
	vertices.emplace_back();
	XMStoreFloat3(&vertices.back().pos, XMVectorNegate(base));
	XMStoreFloat3(&vertices.back().normal, XMVector3Normalize(XMVectorNegate(base)));

	//
	auto calcIdx = [latDiv, longDiv](UINT iLat, UINT iLong)
	{return iLat * longDiv + iLong; };

	std::vector<UINT> indices;

	for (UINT iLat = 0; iLat < latDiv - 2; iLat++)
	{
		for (UINT iLong = 0; iLong < longDiv - 1; iLong++)
		{
			indices.push_back(calcIdx(iLat, iLong));
			indices.push_back(calcIdx(iLat + 1, iLong));
			indices.push_back(calcIdx(iLat, iLong + 1));
			indices.push_back(calcIdx(iLat, iLong + 1));
			indices.push_back(calcIdx(iLat + 1, iLong));
			indices.push_back(calcIdx(iLat + 1, iLong + 1));
		}
		indices.push_back(calcIdx(iLat, longDiv - 1));
		indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		indices.push_back(calcIdx(iLat, 0));
		
		indices.push_back(calcIdx(iLat, 0));
		indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		indices.push_back(calcIdx(iLat + 1, 0));
	}

	//Alpha und Omega
	for (UINT iLong = 0; iLong < longDiv - 1; iLong++)
	{
		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, iLong));
		indices.push_back(calcIdx(0, iLong + 1));
		indices.push_back(calcIdx(latDiv - 2, iLong + 1));
		indices.push_back(calcIdx(latDiv - 2, iLong));
		indices.push_back(iSouthPole);
	}

	indices.push_back(iNorthPole);
	indices.push_back(calcIdx(0, longDiv - 1));
	indices.push_back(calcIdx(0, 0));
	indices.push_back(calcIdx(latDiv - 2, 0));
	indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
	indices.push_back(iSouthPole);
	//

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));
	indexBufferData.pSysMem = &indices[0];


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertices[0]) * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(indices[0]) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

	ID3DBlob* blob = nullptr;
	D3DReadFileToBlob(L"VertexShader.cso", &blob);

	static D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static UINT numElements = ARRAYSIZE(layout);

	device->CreateInputLayout(layout, numElements, blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
	blob->Release();

	numIndices = indices.size();

	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixPerspectiveLH(0.4f * 3.14f, (float)iHeight / (float)iWidth, 0.5f, 10.0f))
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

}

Sphere::~Sphere()
{
	vertexBuffer->Release();
	indexBuffer->Release();
	modelViewProj->Release();
	inputLayout->Release();
	device->Release();
	context->Release();
}

void Sphere::reset()
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

void Sphere::draw()
{
	context->IASetInputLayout(inputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->VSSetConstantBuffers(0, 1, &modelViewProj);
	
	context->DrawIndexed(numIndices, 0, 0);
}

void Sphere::move(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixPerspectiveLH(0.4f * 3.14f, (float)iHeight / (float)iWidth, 0.5f, 10.0f))
	};
	context->UpdateSubresource(modelViewProj, 0, 0, &cb, 0, 0);
}

void Sphere::rotate(float x, float y, float z)
{
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;

	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixPerspectiveLH(0.4f * 3.14f, (float)iHeight / (float)iWidth, 0.5f, 10.0f))
	};
	context->UpdateSubresource(modelViewProj, 0, 0, &cb, 0, 0);
}

void Sphere::scale(float x, float y, float z)
{
	scaling.x += x;
	scaling.y += y;
	scaling.z += z;

	cb =
	{
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z)),
		XMMatrixTranspose(XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(position.x, position.y, position.z) * XMMatrixPerspectiveLH(0.4f * 3.14f, (float)iHeight / (float)iWidth, 0.5f, 10.0f))
	};
	context->UpdateSubresource(modelViewProj, 0, 0, &cb, 0, 0);
}
