#pragma once

#include <DirectXMath.h>
#include "Camera.h"
#include "InputManager.h"
#include "GameEntity.h"
#include "OBB.h"

using namespace DirectX;

class Player
{
public:
	Player(Camera* _camera, InputManager* _input, GameEntity* _graphics);
	~Player(void);

	void Update(float dt);

	XMFLOAT3 getPosition();
	XMFLOAT4 getRotation();

	OBB* getCollider();

private:
	Camera* camera;
	InputManager* input;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	float speed;
	float currentRot;
	GameEntity* graphics;
};

