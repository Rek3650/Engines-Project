#include "Bullet.h"


Bullet::Bullet(GameEntity* _graphics)
{
	graphics = _graphics;

	lifeSpan = 5;
	timeAlive = 0;
	direction = XMFLOAT3(0, 0, 0);
	speed = 20;
	gravity = 1;
	active = false;
	onGround = false;
}


Bullet::~Bullet(void)
{

}

void Bullet::Fire(XMFLOAT3 dir)
{
	direction = XMFLOAT3(dir.x, 0, dir.z);
	active = true;
}

void Bullet::setPos(XMFLOAT3 newPos)
{
	graphics->geometry->SetTranslation(newPos);
}

void Bullet::Update(float dt)
{
	if(!active)
	{
		graphics->geometry->SetTranslation(XMFLOAT3(0, 500, 0));
	}
	else
	{
		timeAlive += dt;
		if(timeAlive > lifeSpan)
		{
			timeAlive = 0;
			active = false;
		}

		XMFLOAT3 position = graphics->geometry->GetPosition();
		position = XMFLOAT3(position.x+(direction.x*speed*dt), position.y, position.z+(direction.z*speed*dt));
		graphics->geometry->SetTranslation(position);
	}
}
