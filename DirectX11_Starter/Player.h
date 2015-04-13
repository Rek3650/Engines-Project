#pragma once

#include <DirectXMath.h>
#include "Camera.h"
#include "InputManager.h"

using namespace DirectX;

class Player
{
public:
	Player(Camera* _camera, InputManager* _input);
	~Player(void);

	void Update(float dt);

	XMFLOAT3 getPosition();

private:
	Camera* camera;
	InputManager* input;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	float speed;
};

