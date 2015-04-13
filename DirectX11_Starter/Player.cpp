#include "Player.h"
#include <iostream>

Player::Player(Camera* _camera, InputManager* _input)
{
	camera = _camera;
	input = _input;
	position = camera->GetPosition();
	direction = XMFLOAT3(0, 0, 1);
	speed = 0.005f;
}


Player::~Player(void)
{
}

void Player::Update(float dt)
{
	if(input->getKey(DIK_W))
	{
		position.z += speed;
	}
	if(input->getKey(DIK_S))
	{
		position.z -= speed;
	}
	if(input->getKey(DIK_A))
	{
		position.x -= speed;
	}
	if(input->getKey(DIK_D))
	{
		position.x += speed;
	}
	if(input->getKey(DIK_Q))
	{
		position.y += speed;
	}
	if(input->getKey(DIK_E))
	{
		position.y -= speed;
	}

	if(input->getMouse(0))
	{
		std::cout << input->getMousePos().x << ", " << input->getMousePos().y << std::endl;
	}

	camera->SetPosition(position.x, position.y, position.z);
	camera->SetTarget(position.x, position.y, position.z+1);
}

XMFLOAT3 Player::getPosition()
{
	return position;
}