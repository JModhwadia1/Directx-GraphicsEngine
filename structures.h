#pragma once
#include <d3d11_1.h>
#include <directxmath.h>
using namespace DirectX;


struct MeshData
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};


struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT2 TexC;

    bool operator<(const SimpleVertex other) const
    {
        return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
    };

};


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	//Ambient Light
	XMFLOAT4 AmbLight;
	XMFLOAT4 AmbMat;

	//Diffuse Light
	XMFLOAT4 DiffLight;
	XMFLOAT4 DiffMat;
	XMFLOAT3 DirToLight;
	FLOAT pad;

	//Specular
	XMFLOAT4 SpecMat;
	XMFLOAT4 SpecLight;
	FLOAT SpecPower;		//Power to raise specular falloff by
	XMFLOAT3 EyeWorldPos;		// cameras eye position in the world
	//bool SpecularEnabled;

	XMFLOAT4 fogColor;
	FLOAT    fogStart;
	FLOAT    fogEnd;
	FLOAT	 pad2;
	FLOAT	 pad3;

};

enum CAMERATYPE
{
	cAt,
	cTo
};

