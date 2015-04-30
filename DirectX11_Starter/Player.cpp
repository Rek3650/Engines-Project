#include "Player.h"
#include <iostream>

Player::Player(Camera* _camera, InputManager* _input, GameEntity* _graphics)
{
	camera = _camera;
	input = _input;
	position = camera->GetPosition();
	direction = XMFLOAT3(0, 0, 1);
	speed = 0.005f;
	currentRot = 0;
	graphics = _graphics;
}


Player::~Player(void)
{
}

void Player::Update(float dt)
{
	float rotation = 0;

	if(input->getKey(DIK_W))
	{
		position.x += speed*direction.x;
		position.y += speed*direction.y;
		position.z += speed*direction.z;
	}
	if(input->getKey(DIK_S))
	{
		position.x -= speed*direction.x;
		position.y -= speed*direction.y;
		position.z -= speed*direction.z;
	}
	if(input->getKey(DIK_A))
	{
		rotation -= speed;
		currentRot -= speed;
	}
	if(input->getKey(DIK_D))
	{
		rotation += speed;
		currentRot += speed;
	}
	if(input->getKey(DIK_Q))
	{
		position.y += speed;
	}
	if(input->getKey(DIK_E))
	{
		position.y -= speed;
	}

	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 0);
	XMMATRIX rot = XMMatrixRotationY(rotation);
	dir = XMVector3Transform(dir, rot);
	XMStoreFloat3(&direction, dir);

	/*if(input->getMouse(0))
	{
		std::cout << input->getMousePos().x << ", " << input->getMousePos().y << std::endl;
	}*/

	// update the graphics for the player
	graphics->Translation(position);
	rot = XMMatrixRotationY(currentRot);
	XMFLOAT4 rotQuat;
	XMStoreFloat4(&rotQuat, XMQuaternionRotationMatrix(rot));
	graphics->Rotation(rotQuat);

	// update the camera
	camera->SetPosition(position.x-(direction.x*5), position.y+1, position.z-(direction.z*5));
	camera->SetTarget(position.x, position.y, position.z);
}

XMFLOAT3 Player::getPosition()
{
	return position;
}

XMFLOAT4 Player::getRotation()
{
	return graphics->geometry->GetRotation();
}