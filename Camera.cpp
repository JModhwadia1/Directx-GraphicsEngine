#include "Camera.h"
//
//Camera::Camera()
//{
//
//
//}



Camera::Camera(CAMERATYPE type, XMFLOAT3 eye, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	// Initialize the view matrix
	//eye= (0.0f, 0.0f, -3.0f);
	//at = (0.0f, 0.0f, 0.0f);
	//up = (0.0f, 1.0f, 0.0f);

	//_eye = eye;
	//_at = at;
	//_up = up;

	//XMVECTOR vecEye = XMLoadFloat3(&eye);
	//XMVECTOR vecAt = XMLoadFloat3(&at);
	//XMVECTOR vecUp = XMLoadFloat3(&up);	

	
	_cameraType = type;
	_eye = eye;
	_at = at;
	_up = up;

	XMVECTOR vecEye = XMVectorSet(this->_eye.x, this->_eye.y, this->_eye.z, 0.0f);
	XMVECTOR vecAt = XMVectorSet(this->_at.x, this->_at.y, this->_at.z, 0.0f);
	XMVECTOR vecUp = XMVectorSet(this->_up.x, this->_up.y, this->_up.z, 0.0f);

	
	_windowHeight = windowHeight;
	_windowWidth = windowWidth;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
	
	/*XMStoreFloat3(&_eye, vecEye);
	XMStoreFloat3(&_at, vecAt);
	XMStoreFloat3(&_up, vecUp);*/

	// Initialize the view matrix
//	lookToVec = vecAt - vecEye;

	XMMATRIX LookAt = XMMatrixLookAtLH(vecEye, vecAt, vecUp);
	XMMATRIX LookTo = XMMatrixLookToLH(vecEye, vecAt, vecUp);
	if (type == cAt)
	{
		XMStoreFloat4x4(&_view, /*XMMatrixLookToLH(vecEye, vecAt, vecUp*/LookAt);
	}
	else if (type == cTo)
	{
		XMStoreFloat4x4(&_view, /*XMMatrixLookToLH(vecEye, vecAt, vecUp*/LookTo);
	}

	//XMMATRIX LookAt = XMMatrixLookAtLH(vecEye, vecAt, vecUp);

	
	//initialize the view matrix


	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / windowHeight, nearDepth, farDepth));
}

Camera::~Camera()
{

}

void Camera::Update()
{
	// Forward Movement
	if (GetAsyncKeyState(VK_UP))
	{
		_eye.z += CAMERA_SPEED / 60.0f;
	}
	// Backward Movement
	if (GetAsyncKeyState(VK_DOWN))
	{
		_eye.z -= CAMERA_SPEED / 60.0f;
	}
	// Right Movement
	if (GetAsyncKeyState(VK_RIGHT))
	{
		_eye.x +=  CAMERA_SPEED / 60.0f;
	}
	// Left Movement
	if (GetAsyncKeyState(VK_LEFT))
	{
		_eye.x -= CAMERA_SPEED / 60.0f;
	}
	// Up Movement
	if (GetAsyncKeyState(VK_SHIFT))
	{
		_eye.y += CAMERA_SPEED / 60.0f;
	}
	if (GetAsyncKeyState(VK_CONTROL))
	{
		_eye.y -= CAMERA_SPEED / 60.0f;
	}

	//XMMATRIX LookTo = XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up));

	//XMMATRIX LookAt = XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up));

	////initialize the view matrix
	//XMStoreFloat4x4(&_view, LookAt);

	//// Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / _windowHeight, _nearDepth, _farDepth));



	XMVECTOR vecEye = XMVectorSet(_eye.x, _eye.y, _eye.z, 0.0f);
	XMVECTOR vecAt = XMVectorSet(_at.x, _at.y, _at.z, 0.0f);
	XMVECTOR vecUp = XMVectorSet(_up.x, _up.y, _up.z, 0.0f);


	XMMATRIX LookAt = XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up));
	XMMATRIX LookTo = XMMatrixLookToLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up));

	if (_cameraType == cAt)
	{
		XMStoreFloat4x4(&_view, /*XMMatrixLookToLH(vecEye, vecAt, vecUp*/LookAt);
	}
	else if (_cameraType == cTo)
	{
		XMStoreFloat4x4(&_view, /*XMMatrixLookToLH(vecEye, vecAt, vecUp*/LookTo);
	}
	//initialize the view matrix


	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}
