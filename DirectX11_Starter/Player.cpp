#include "Player.h"
#include <iostream>

Player::Player(Camera* _camera, InputManager* _input, GameEntity* _graphics)
{
	health = 5;
	gravity = 10;
	jumpSpeed = 10;
	jumpHeight = 6;
	curJumpHeight = 0;
	jumping = false;
	onGround = false;
	ammo = 0;

	camera = _camera;
	input = _input;
	graphics = _graphics;
	if(graphics != NULL) position = graphics->geometry->GetPosition();
	direction = XMFLOAT3(0, 0, 1);
	speed = 5;
	currentRot = 0;
}


Player::~Player(void)
{
}

void Player::Update(float dt)
{
	if(input != NULL && camera != NULL && graphics != NULL)
	{
		float rotation = 0;

		// move
		if(input->getKey(DIK_W))
		{
			position.x += speed*direction.x*dt;
			position.y += speed*direction.y*dt;
			position.z += speed*direction.z*dt;
		}
		if(input->getKey(DIK_S))
		{
			position.x -= speed*direction.x*dt;
			position.y -= speed*direction.y*dt;
			position.z -= speed*direction.z*dt;
		}
		if(input->getKey(DIK_A))
		{
			XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 0);
			XMMATRIX rot = XMMatrixRotationY(90);
			dir = XMVector3Transform(dir, rot);
			XMFLOAT3 right;
			XMStoreFloat3(&right, dir);

			position.x -= speed*right.x*dt;
			position.y -= speed*right.y*dt;
			position.z -= speed*right.z*dt;
		}
		if(input->getKey(DIK_D))
		{
			XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 0);
			XMMATRIX rot = XMMatrixRotationY(90);
			dir = XMVector3Transform(dir, rot);
			XMFLOAT3 right;
			XMStoreFloat3(&right, dir);

			position.x += speed*right.x*dt;
			position.y += speed*right.y*dt;
			position.z += speed*right.z*dt;
		}

		// jump
		if(health > 0)
		{
			if(input->onKeyDown(DIK_SPACE) && onGround)
			{
				if(!jumping)
				{
					jumping = true;
					onGround = false;
					curJumpHeight = 0;
				}
			}
			if(jumping)
			{
				curJumpHeight += jumpSpeed*dt;
				position.y += jumpSpeed*dt;
				if(curJumpHeight >= jumpHeight)
				{
					jumping = false;
				}
			}
			else if(!onGround)
			{
				position.y -= gravity*dt;
			}
		}

		rotation += input->getMousePos().x/50;
		currentRot += input->getMousePos().x/50;

		XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 0);
		XMMATRIX rot = XMMatrixRotationY(rotation);
		dir = XMVector3Transform(dir, rot);
		XMStoreFloat3(&direction, dir);

		// update the graphics for the player
		graphics->Translation(position);
		rot = XMMatrixRotationY(currentRot);
		XMFLOAT4 rotQuat;
		XMStoreFloat4(&rotQuat, XMQuaternionRotationMatrix(rot));
		graphics->Rotation(rotQuat);

		// update the camera
		camera->SetPosition(position.x-(direction.x*5), position.y+1, position.z-(direction.z*5));
		camera->SetTarget(position.x, position.y, position.z);

		// shoot a bullet
		if(input->onMouseDown(0) && ammo > 0 && health > 0)
		{
			bullets[ammo-1]->setPos(position);
			bullets[ammo-1]->Fire(direction);
			bullets.pop_back();
			ammo--;
		}
	}
}

void Player::addBullet(Bullet* b)
{
	ammo++;
	bullets.push_back(b);
}

XMFLOAT3 Player::getPosition()
{
	return position;
}

void Player::setPosition(XMFLOAT3 newPos)
{
	position = newPos;
}

XMFLOAT4 Player::getRotation()
{
	return graphics->geometry->GetRotation();
}