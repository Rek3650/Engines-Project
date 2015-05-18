#pragma once

#include <DirectXMath.h>
#include "Camera.h"
#include "InputManager.h"
#include "GameEntity.h"
#include "Bullet.h"
#include <vector>

using namespace DirectX;

class Player
{
public:
	Player(Camera* _camera = NULL, InputManager* _input = NULL, GameEntity* _graphics = NULL);
	~Player(void);

	void Update(float dt);

	XMFLOAT3 getPosition();
	XMFLOAT4 getRotation();
	void setPosition(XMFLOAT3 newPos);

	OBB* getCollider();

	bool onGround;
	void addBullet(Bullet* b);

private:
	int health;
	float gravity;
	float jumpSpeed;
	float jumpHeight;
	float curJumpHeight;
	bool jumping;

	int ammo;
	std::vector<Bullet*> bullets;

	Camera* camera;
	InputManager* input;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	float speed;
	float currentRot;
	GameEntity* graphics;
};

