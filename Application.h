#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <iostream>
#include <stdlib.h>
#include "DDSTextureLoader.h"
#include "OBJLoader.h";
#include "structures.h"
#include "Camera.h"
#include "Gameobject.h"
#include "XML/rapidxml_utils.hpp"

using namespace DirectX;




class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11VertexShader*     _pSkyboxVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11PixelShader*      _pSkyboxPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pPyramidVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pPyramidIndexBuffer;
	ID3D11Buffer*           _pSkyboxIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world, _world2, _world3, _world4, _pyramid, _carMesh, _skyboxMesh;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	XMFLOAT4X4				asteroidWorlds[10];
#
	ID3D11DepthStencilState* _depthStencilLessEqual;

	ID3D11BlendState* Transparency;
	//Texture Variables
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11ShaderResourceView* _pCarRV = nullptr;
	ID3D11ShaderResourceView* _pTextureSpecRV = nullptr;
	ID3D11ShaderResourceView* _pCubeMapRV;
	ID3D11SamplerState* _pSamplerLinear = nullptr;

	//Object variables
	MeshData carMeshData;
	MeshData skyboxMeshData;

	std::vector<Gameobject*> GameObjects;

	//std::vector<Camera*> myCameras;

	//Cameras
	Camera* currentCamera;
	Camera* myCamera;
	Camera* myCamera2;


	
private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	//Raseterizer States
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solid;

	//lighting variables
	XMFLOAT4 AmbientLight;
	XMFLOAT4 AmbientMaterial;

	//Diffuse variables
	XMFLOAT4 DiffuseMaterial;
	XMFLOAT4 DiffuseLight;
	XMFLOAT3 directionToLight;
	
	//Specular Varibles
	XMFLOAT4 SpecularMaterial;
	XMFLOAT4 SpecularLight;
	FLOAT SpecularPower;		
	XMFLOAT3 EyeWorldPos;
	
	//int SpecEnable = 1;
	
	//Fog Variables
	XMFLOAT4 FogColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	FLOAT FogStart = 0.0f;
	FLOAT FogEnd = 50.0f;

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);


	void Update();
	void Draw();
};

