#pragma once
#include "Graphics.h"
#include <vector>

class Sphere
{
public:
	Sphere(Graphics& graphics, int latDiv, int longDiv);
	~Sphere();
	void reset();
	void draw();
	void move(float x, float y, float z);
	void rotate(float x, float y, float z);
	void scale(float x, float y, float z);

	struct cBufferMatrix
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX modelViewProj;
	};

private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* modelViewProj;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	cBufferMatrix cb;
	DirectX::XMFLOAT3 position = {0.0f, 0.0f, 2.0f};
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scaling = {0.7f, 0.7f, 0.7f};
	UINT numIndices;
};

