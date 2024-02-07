#pragma once

#include <d3d11_1.h>
#include "OBJLoader.h"
class Gameobject
{
private:
	MeshData Mesh;
	XMFLOAT4X4 objectWorldMatrix;
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scale;
	ID3D11ShaderResourceView* textureRV = nullptr;



public:
	Gameobject(std::string filePath, ID3D11Device* device, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale);

	~Gameobject();

	void SetPosition(XMFLOAT3 position);
	void SetRotation(XMFLOAT3 rotationSpeed);
	void SetScale(XMFLOAT3 size);
	XMFLOAT4X4 GetWorldMatrix();

	void SetShaderResource(ID3D11ShaderResourceView* in) { textureRV = in; }
	ID3D11ShaderResourceView** GetShaderResource() { return &textureRV; }

	void SetMeshData(MeshData in);
	MeshData* GetMeshData() { return &Mesh; }

};

