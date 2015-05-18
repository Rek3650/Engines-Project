// ----------------------------------------------------------------------------
//  A few notes on project settings
//
//  - The project is set to use the UNICODE character set
//    - This was changed in Project Properties > Config Properties > General > Character Set
//    - This basically adds a "#define UNICODE" to the project
//
//  - The include directories were automagically correct, since the DirectX 
//    headers and libs are part of the windows SDK
//    - For instance, $(WindowsSDK_IncludePath) is set as a project include 
//      path by default.  That's where the DirectX headers are located.
//
//  - Two libraries had to be manually added to the Linker Input Dependencies
//    - d3d11.lib
//    - d3dcompiler.lib
//    - This was changed in Project Properties > Config Properties > Linker > Input > Additional Dependencies
//
//  - The Working Directory was changed to match the actual .exe's 
//    output directory, since we need to load the shader files at run time
//    - This was changed in Project Properties > Config Properties > Debugging > Working Directory
//
// ----------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <d3dcompiler.h>
#include "GameManager.h"
#include <xmmintrin.h>
#include <iostream>
#include "Primitives.h"
//#include "easylogging++.h"
#include "ModelLoader.h"

using namespace DirectX;
using namespace std;

//INITIALIZE_EASYLOGGINGPP

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Make the game, initialize and run
	GameManager game(hInstance);
	
	if( !game.Init() )
		return 0;
	
	return game.Run();
}

#pragma endregion
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#pragma region Constructor / Destructor

GameManager::GameManager(HINSTANCE hInstance) : DXGame(hInstance)
{
	// Set up our custom caption and window size
	windowCaption = L"Awesome Engine";
	windowWidth = 800;
	windowHeight = 600;

	//initializes logging with default settings. must only be run once
	//LOG(INFO) << "My first info log using default logger";
}

GameManager::~GameManager()
{
	// Release all of the D3D stuff that's still hanging out
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(inputLayout);
	
	delete cube;
	delete cube1;
	delete triMat;
	delete camera;
	delete lineRenderer;
	delete player;
	delete network;

	for(int i = 0; i < 9; i++)
	{
		delete platforms[i];
	}

	for(int i = 0; i < 10; i++)
	{
		delete bullets[i];
	}
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool GameManager::Init()
{	
	lockMouse = true;
	if( !DXGame::Init() )
		return false;

	// make a camera/input handler
	camera = new Camera(AspectRatio());
	input = new InputManager(hAppInst, hMainWnd, windowWidth, windowHeight);
	input->init();

	// create materials
	triMat = new Material(device, deviceContext, L"../images/epicTriforce.jpg");
// for some retarded reason this texture won't load in Debug so only load it in release
// this may cause objects that use this texture to look weird in debug
#if defined(DEBUG) | defined(_DEBUG)
#else
	nullTexture = new Material(device, deviceContext, L"../images/black.jpg");
#endif
	bumpsNormalMap = new Material(device, deviceContext, L"../images/BubbleGrip-NormalMap.png");

	// Set up buffers and such
	LoadShadersAndInputLayout();

	// for drawing lines
	lineRenderer = new LineRenderer(device, pixelShader, vertexShader, camera);

	elapsedTime = 0;

	network = new NetworkManager(hMainWnd);

	playerIndex = network->GetPlayerIndex();

	if(playerIndex == 0)
	{
		player = new Player(camera, input, cube);
		for(int i = 0; i < 5; i++)
		{
			prevActive[i] = false;
			buls[i] = new Bullet(bullets[i]);
			player->addBullet(buls[i]);
		}
		network->sendObjects.push_back(cube);
		/*for(int i = 0; i < 5; i++)
		{
			network->sendObjects.push_back(bullets[i]);
		}*/
		network->receiveObjects.push_back(cube1);
	}
	else
	{
		player = new Player(camera, input, cube1);
		for(int i = 0; i < 5; i++)
		{
			prevActive[i] = false;
			buls[i] = new Bullet(bullets[i+5]);
			player->addBullet(buls[i]);
		}
		network->sendObjects.push_back(cube1);
		/*for(int i = 5; i < 10; i++)
		{
			network->sendObjects.push_back(bullets[i]);
		}*/
		network->receiveObjects.push_back(cube);
	}
	collision = new Collision();
	numCollisions = 0;

	return true;
}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void GameManager::LoadShadersAndInputLayout() 
{
	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 28,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Load Vertex Shader --------------------------------------
	ID3DBlob* vsBlob;
#if defined(DEBUG) | defined(_DEBUG)
	D3DReadFileToBlob(L"VertexShader.cso", &vsBlob);
#else
	D3DReadFileToBlob(L"../Release/VertexShader.cso", &vsBlob);
#endif

	// Create the shader on the device
	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));

	// Before cleaning up the data, create the input layout
	HR(device->CreateInputLayout(
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));

	// Clean up
	ReleaseMacro(vsBlob);

	// Load Pixel Shader ---------------------------------------
	ID3DBlob* psBlob;
#if defined(DEBUG) | defined(_DEBUG)
	D3DReadFileToBlob(L"BumpPixelShader.cso", &psBlob);
#else
	D3DReadFileToBlob(L"../Release/BumpPixelShader.cso", &psBlob);
#endif

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&bumpPixelShader));

	// Clean up
	ReleaseMacro(psBlob);

	// load another shader
#if defined(DEBUG) | defined(_DEBUG)
	D3DReadFileToBlob(L"PixelShader.cso", &psBlob);
#else
	D3DReadFileToBlob(L"../Release/PixelShader.cso", &psBlob);
#endif

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));

	// Clean up
	ReleaseMacro(psBlob);

	// called in here because this step has to happen after the shaders are created
	CreateGeometryBuffers();
}

// initialize the game objects
void GameManager::CreateGeometryBuffers()
{
	Primitives primitives(device);
	//Create a cube
	ModelLoader* loader = new ModelLoader();
	//cube = new GameEntity(loader->LoadModel("../Resources/Model.dae", device), device, pixelShader, vertexShader, triMat, camera);
	cube = primitives.makeCube(pixelShader, vertexShader , nullTexture, nullTexture,camera, XMFLOAT4(0.75, 0, 0, 1));
	cube->Scale(XMFLOAT3(0.5f, 1.0f, 0.5f));
	cube->Translation(XMFLOAT3(0.0f, 0.0f, -10.0f));

	cube1 = primitives.makeCube(pixelShader, vertexShader , nullTexture, nullTexture,camera, XMFLOAT4(0, 0, 0.75, 1));
	//cube1 = new GameEntity(loader->LoadModel("../Resources/Model.dae", device), device, pixelShader, vertexShader, nullTexture, nullTexture, camera);
	//cube1->Scale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	cube1->Scale(XMFLOAT3(0.5f, 1.0f, 0.5f));
	cube1->Translation(XMFLOAT3(0.0f, 0.0f, 10.0f));

	// make the collider fit the object a bit more
	//cube1->getCollider()->SetScale(XMFLOAT3(3, 3, 3));
	//cube1->getCollider()->SetPos(XMFLOAT3(0, 0.01, 0.5));

	// create the platform objects
	for(int i = 0; i < 9; i++)
	{
		platforms[i] = primitives.makeCube(bumpPixelShader, vertexShader , nullTexture, bumpsNormalMap,camera, XMFLOAT4(0.75, 0.75, 0.75, 1));
		platforms[i]->Scale(XMFLOAT3(0.25f, 5.0f, 5.0f));
		platforms[i]->Rotation(XMFLOAT4(0.7071067811865476, 0.7071067811865475, 0, 0));
	}

	// set the layout of the platforms
	platforms[0]->Translation(XMFLOAT3(0, 0, 0));
	platforms[1]->Translation(XMFLOAT3(0, 0, 10));
	platforms[2]->Translation(XMFLOAT3(0, 0, -10));
	platforms[3]->Translation(XMFLOAT3(10, 0, 0));
	platforms[4]->Translation(XMFLOAT3(-10, 0, 0));
	platforms[5]->Translation(XMFLOAT3(10, 0, 10));
	platforms[6]->Translation(XMFLOAT3(10, 0, -10));
	platforms[7]->Translation(XMFLOAT3(-10, 0, 10));
	platforms[8]->Translation(XMFLOAT3(-10, 0, -10));

	// create the bullet objects
	for(int i = 0; i < 10; i++)
	{
		if(i < 5)
		{
			bullets[i] = primitives.makeCube(bumpPixelShader, vertexShader , nullTexture, nullTexture,camera, XMFLOAT4(0.75, 0, 0, 1));
		}
		else 
		{
			bullets[i] = primitives.makeCube(bumpPixelShader, vertexShader , nullTexture, nullTexture,camera, XMFLOAT4(0, 0, 0.75, 1));
		}

		bullets[i]->Scale(XMFLOAT3(0.25f, 0.25f, 0.25f));
		bullets[i]->Rotation(XMFLOAT4(0.7325378163287419, 0.4619397662556433, -0.19134171618254492, 0.4619397662556433));
		bullets[i]->Translation(XMFLOAT3(0, 0, 0));
	}
}

#pragma endregion

#pragma region Window Resizing

// Handles resizing the window and updating our projection matrix to match
void GameManager::OnResize()
{
	// Handle base-level DX resize stuff
	DXGame::OnResize();

	// Update our projection matrix since the window size changed
	if(camera != NULL) camera->UpdateProjection(AspectRatio());

	// update the window size for the mouse input
	if(input != NULL) input->onResize(windowWidth, windowHeight);
}
#pragma endregion

#pragma region Game Loop
void GameManager::UpdateScene(float dt)
{
	if(input->onKeyDown(DIK_ESCAPE))
	{
		lockMouse = !lockMouse;
	}
	if(lockMouse)
	{
		// trap the cursor in the center of the window
		RECT windowRect;
		GetWindowRect(hMainWnd, &windowRect);
		RECT r = {windowRect.left+(windowWidth/2), windowRect.top+(windowHeight/2), windowRect.left+(windowWidth/2), windowRect.top+(windowHeight/2)};
		ClipCursor(&r);
		// hide cursor
		ShowCursor(false);
	}
	else
	{
		// unlock the cursor
		RECT r = {10, 10, 0, 0};
		ClipCursor(&r);
		// show the cursor
		ShowCursor(true);
	}

	elapsedTime += dt;
	input->update();
	camera->Update(dt);
	
	player->Update(dt);

	for(int i = 0; i < 9; i++)
	{
		platforms[i]->Update(deviceContext);
	}

	for(int i = 0; i < 5; i++)
	{
		prevActive[i] = buls[i]->active;
		buls[i]->Update(dt);
		if(prevActive[i] && !buls[i]->active)
		{
			player->addBullet(buls[i]);
		}
	}

	// update the networked objects
	network->Update();
	for(int i = 0; i < network->sendObjects.size(); i++)
	{
		network->sendObjects[i]->Update(deviceContext);
	}
	for(int i = 0; i < network->receiveObjects.size(); i++)
	{
		network->receiveObjects[i]->Update(deviceContext);
	}

	// check for collisions between the moving objects and platforms
	XMFLOAT4 collisionColor(0, 1, 0, 1);
	for(int i = 0; i < 9; i++)
	{
		if(collision->SAT(cube->getCollider(), platforms[i]->getCollider()))
		{
			collisionColor = XMFLOAT4(1, 0, 0, 1);
			player->onGround = true;
			player->setPosition(XMFLOAT3(player->getPosition().x, 0.625, player->getPosition().z));
		}
	}

	if(input->getKey(DIK_B))
	{
		// draw the axis of the player obb
		/*
		XMFLOAT3 centerPos = cube->getCollider()->CenterPos();
		XMFLOAT3 colXAxis = cube->getCollider()->XAxis();
		XMFLOAT3 colYAxis = cube->getCollider()->YAxis();
		XMFLOAT3 colZAxis = cube->getCollider()->ZAxis();
		lineRenderer->addLine(centerPos, XMFLOAT3(centerPos.x+colXAxis.x, centerPos.y+colXAxis.y, centerPos.z+colXAxis.z), XMFLOAT4(1, 1, 0, 1));
		lineRenderer->addLine(centerPos, XMFLOAT3(centerPos.x+colYAxis.x, centerPos.y+colYAxis.y, centerPos.z+colYAxis.z), XMFLOAT4(1, 1, 0, 1));
		lineRenderer->addLine(centerPos, XMFLOAT3(centerPos.x+colZAxis.x, centerPos.y+colZAxis.y, centerPos.z+colZAxis.z), XMFLOAT4(1, 1, 0, 1));
		*/

		// draw the wireframe of the player obb
		XMFLOAT3* obbVerts = cube->getCollider()->GetVerts();
		lineRenderer->addLine(obbVerts[0], obbVerts[1], collisionColor);
		lineRenderer->addLine(obbVerts[2], obbVerts[3], collisionColor);
		lineRenderer->addLine(obbVerts[4], obbVerts[5], collisionColor);
		lineRenderer->addLine(obbVerts[6], obbVerts[7], collisionColor);

		lineRenderer->addLine(obbVerts[0], obbVerts[2], collisionColor);
		lineRenderer->addLine(obbVerts[1], obbVerts[3], collisionColor);
		lineRenderer->addLine(obbVerts[4], obbVerts[6], collisionColor);
		lineRenderer->addLine(obbVerts[5], obbVerts[7], collisionColor);

		lineRenderer->addLine(obbVerts[0], obbVerts[4], collisionColor);
		lineRenderer->addLine(obbVerts[1], obbVerts[5], collisionColor);
		lineRenderer->addLine(obbVerts[2], obbVerts[6], collisionColor);
		lineRenderer->addLine(obbVerts[3], obbVerts[7], collisionColor);
	}

	lineRenderer->Update(deviceContext);
}

// Clear the screen, redraw everything, present
void GameManager::DrawScene()
{
	const float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	// Clear the buffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Set up the input assembler
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for(int i = 0; i < 9; i++)
	{
		platforms[i]->Draw(deviceContext);
	}

	// draw the networked objects
	for(int i = 0; i < network->sendObjects.size(); i++)
	{
		network->sendObjects[i]->Draw(deviceContext);
	}
	for(int i = 0; i < network->receiveObjects.size(); i++)
	{
		network->receiveObjects[i]->Draw(deviceContext);
	}

	// Draw debug lines
	lineRenderer->Draw(deviceContext);

	// Present the buffer
	HR(swapChain->Present(0, 0));
}

#pragma endregion