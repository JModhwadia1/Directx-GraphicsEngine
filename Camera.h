#pragma once

#include <DirectXMath.h>
#include <d3d11_1.h>
#include "structures.h"
using namespace DirectX;
class Camera
{
private:

	//private attributes to store the camera position and view
	//volume
	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	CAMERATYPE _cameraType;

	//attributes to hold the view and projection matrices which
	//will be passed to the shader
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

	const float CAMERA_SPEED = 0.1f;


public:

	Camera();
	//Constructor and destuctor for the camera
	Camera( CAMERATYPE type, XMFLOAT3 eye, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	~Camera();

	//update function to make the current view and projection matrices
	void Update();

	//functions to return the eye, at and up attributes
	XMFLOAT3 GetEye() { return _eye; }
	XMFLOAT3 GetAt()  { return _at; }
	XMFLOAT3 GetUp()  { return _up; }

	//functions to set the eye, at and up attributes

	//functions to get view, projection and combined ViewProjection matrices
	XMFLOAT4X4 GetView() { return _view; }
	XMFLOAT4X4 GetProjection() { return _projection; }

	XMVECTOR lookToVec;

	

	//A function to reshape the camera volume if the window is resized
	/*void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);*/
};

