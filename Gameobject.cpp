#include "Gameobject.h"

Gameobject::Gameobject(std::string filePath, ID3D11Device* device,XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	char* meshfilePath = (char*)filePath.c_str();
	Mesh = OBJLoader::Load(meshfilePath, device);

	
	Position = position;
	Rotation = rotation;
	Scale = scale;
}

void Gameobject::SetPosition(XMFLOAT3 position)
{
	Position = position;
}

void Gameobject::SetRotation(XMFLOAT3 rotationSpeed)
{
	Rotation = rotationSpeed;
	//Rotation.x += rotationSpeed.x;
	//Rotation.y += rotationSpeed.y;
	//Roat.z += rotationSpeed.z;
}

void Gameobject::SetScale(XMFLOAT3 size)
{
	Scale = size;
}

XMFLOAT4X4 Gameobject::GetWorldMatrix()
{
	//Build world matrix:
	//W = S * R * T
	// X, Y, Z rotations
	float xR = XMConvertToRadians(Rotation.x);
	float yR = XMConvertToRadians(Rotation.y);
	float zR = XMConvertToRadians(Rotation.z);

	//translation, scale and rotation matrices
	XMMATRIX translationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(xR, yR, zR);

	XMFLOAT4X4 returnMatrix;
	XMStoreFloat4x4(&returnMatrix, scaleMatrix * rotationMatrix * translationMatrix);

	return returnMatrix;
}
