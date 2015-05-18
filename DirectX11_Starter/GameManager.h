#pragma once

#include <DirectXMath.h>
#include "DXGame.h"

#include "Mesh.h"
#include "GameEntity.h"
#include "Material.h"
#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "Spline.h"
#include <vector>
#include "Camera.h"
#include "InputManager.h"
#include "LineRenderer.h"
#include "Player.h"
#include "NetworkManager.h"
#include "Collision.h"
#include "Bullet.h"

// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// For DirectX Math
using namespace DirectX;

// Demo class which extends the base DXGame class
class GameManager : public DXGame
{
public:
	GameManager(HINSTANCE hInstance);
	~GameManager();

	// Overrides for base level methods
	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

private:
	// Initialization for our "game" demo
	void LoadShadersAndInputLayout();
	void CreateGeometryBuffers();

private:
	bool lockMouse;

	// our shaders
	ID3D11PixelShader* bumpPixelShader;
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;

	// A few more odds and ends we'll need
	ID3D11InputLayout* inputLayout;

	// stuff for the user
	Camera* camera;
	InputManager* input;
	Player* player;
	int playerIndex;

	// test Game Entities
	std::vector<GameEntity*> cubes;
	GameEntity* cube;
	GameEntity* cube1;

	GameEntity* platforms[17];
	GameEntity* bullets[10];
	Bullet*	buls[5];
	bool prevActive[5];

	// texture stuff
	Material* triMat;
	Material* bumpsNormalMap;
	Material* nullTexture;
	
	// Networking
	NetworkManager* network;

	// for debug lines
	LineRenderer* lineRenderer;

	// collision detection
	Collision* collision;
	int numCollisions;

	float elapsedTime;
};