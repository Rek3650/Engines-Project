#include "Camera.h"

using namespace DirectX;

Camera::Camera(float aspectRatio)
{
	position = XMFLOAT3(0,0,-12);
	target = XMFLOAT3(0,0,0);
	up = XMFLOAT3(0,1,0);
	fieldOfView = .25f * 3.1415926535f;
	viewRange = 500.0f;

	UpdateView();
	UpdateProjection(aspectRatio);
}

XMFLOAT3 Camera::GetPosition(void)
{
	return position;
}

XMFLOAT3 Camera::GetTarget(void)
{
	return target;
}

XMFLOAT3 Camera::GetUp(void)
{
	return up;
}

XMFLOAT4X4 Camera::GetView(void)
{
	return view;
}

XMFLOAT4X4 Camera::GetProjection(void)
{
	return projection;
}

void Camera::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	dirty = true;
}

void Camera::SetTarget(float x, float y, float z)
{
	target = XMFLOAT3(x, y, z);
	dirty = true;
}

void Camera::SetUp(float x, float y, float z)
{
	up = XMFLOAT3(x, y, z);
	dirty = true;
}

void Camera::Update(float dt)
{
	if (IsDirty())
		UpdateView();
}

void Camera::UpdateView(void)
{
	XMVECTOR p	= XMVectorSet(position.x, position.y, position.z, 0);
	XMVECTOR t	= XMVectorSet(target.x, target.y, target.z, 0);
	XMVECTOR u	= XMVectorSet(up.x, up.y, up.z, 0);
	XMMATRIX V	= XMMatrixLookAtLH(p, t, u);
	XMStoreFloat4x4(&view, XMMatrixTranspose(V));

	dirty = false;
}

void Camera::UpdateProjection(float aspectRatio)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		fieldOfView,
		aspectRatio,
		0.1f,
		viewRange);
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P));
}

bool Camera::IsDirty(void)
{
	return dirty;
}