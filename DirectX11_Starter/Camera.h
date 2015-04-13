#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(float aspectRatio=1);

	DirectX::XMFLOAT3 GetPosition(void);
	DirectX::XMFLOAT3 GetTarget(void);
	DirectX::XMFLOAT3 GetUp(void);
	DirectX::XMFLOAT4X4 GetView(void);
	DirectX::XMFLOAT4X4 GetProjection(void);
	void SetPosition(float x, float y, float z);
	void SetTarget(float x, float y, float z);
	void SetUp(float x, float y, float z);

	void Update(float dt);
	void UpdateView(void);
	void UpdateProjection(float aspectRatio);
	bool IsDirty(void);

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 target;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	bool dirty;

	float fieldOfView;
	float viewRange;
};

