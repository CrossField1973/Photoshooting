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

private:
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11Buffer* modelViewProj = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	cBufferMatrix cb;
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;
	ID3D11Debug* debug = nullptr;
	DirectX::XMFLOAT3 position = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scaling = {1.0f, 1.0f, 1.0f};
	UINT numIndices;
};

