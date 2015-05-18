#pragma once
#include <DirectXMath.h>
#include "GameEntity.h"

using namespace DirectX;

class Bullet
{
public:
	Bullet(GameEntity* _graphics);
	~Bullet(void);

	void Fire(XMFLOAT3 dir);
	void setPos(XMFLOAT3 newPos);
	void Update(float dt);

	bool active;

private:
	GameEntity* graphics;

	float lifeSpan;
	float timeAlive;
	XMFLOAT3 direction;
	float speed;
	float gravity;
	bool onGround;
};

