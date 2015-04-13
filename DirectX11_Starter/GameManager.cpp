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

#include <Windows.h>
#include <d3dcompiler.h>
#include "GameManager.h"
#include <xmmintrin.h>
#include <iostream>
#include "Primitives.h"

using namespace DirectX;

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
}

GameManager::~GameManager()
{
	// Release all of the D3D stuff that's still hanging out
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(inputLayout);

	delete cube;
	delete triMat;
	delete primitiveBatch;
	delete basicEffect;
	delete camera;
	delete lineRenderer;
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool GameManager::Init()
{	
	if( !DXGame::Init() )
		return false;

	// make a camera/input handler
	camera = new Camera(AspectRatio());
	input = new InputManager(hAppInst, hMainWnd, windowWidth, windowHeight);
	input->init();

	// create materials
	triMat = new Material(device, deviceContext, L"../images/epicTriforce.jpg");

	// Set up buffers and such
	LoadShadersAndInputLayout();

	// for drawing lines
	lineRenderer = new LineRenderer(device, pixelShader, vertexShader, camera);

	// initialize the stuff for the spline
	splineIndex = 0;
	dir = 1;
	ctrlPts.clear();
	splinePts.clear();
	
	// set some default values for the ctrlPts
	/* circle
	ctrlPts.push_back(XMFLOAT3(-1, -1, 0));
	ctrlPts.push_back(XMFLOAT3(-1, 1, 0));
	ctrlPts.push_back(XMFLOAT3(1, 1, 0));
	ctrlPts.push_back(XMFLOAT3(1, -1, 0));
	ctrlPts.push_back(XMFLOAT3(-1, -1, 0));
	ctrlPts.push_back(XMFLOAT3(-1, 1, 0));
	ctrlPts.push_back(XMFLOAT3(1, 1, 0));
	/*/// sin wave
	ctrlPts.push_back(XMFLOAT3(-1.5f, -1, 0));
	ctrlPts.push_back(XMFLOAT3(-1, 0, 0));
	ctrlPts.push_back(XMFLOAT3(-0.5f, 1, 0));
	ctrlPts.push_back(XMFLOAT3(0.5f, -1, 0));
	ctrlPts.push_back(XMFLOAT3(1, 0, 0));
	ctrlPts.push_back(XMFLOAT3(1.5f, 1, 0));
	//*/

	// make the spline
	splinePts = spline.sseMakeSpline(ctrlPts, 100);
	
	// add lines to draw
	for(int i = 1; i < splinePts.size(); i++)
	{
		lineRenderer->addLine(splinePts[i-1], splinePts[i], XMFLOAT4(1, 1, 1, 1));
	}

	elapsedTime = 0;

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
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 28,	D3D11_INPUT_PER_VERTEX_DATA, 0}
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
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow	= XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 white	= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Primitives primitives(device);
	//Create a cube
	cube = primitives.makeCube(pixelShader, vertexShader);
	cube->mat = triMat;
	cube->camera = camera;
	cube->scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	cube->translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
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
	elapsedTime += dt;
	input->update();
	camera->Update(dt);

	if(splineIndex > 1)
	{
		splineIndex = 1;
		dir *= -1;
	}
	else if(splineIndex < 0)
	{
		splineIndex = 0;
		dir *= -1;
	}
	splineIndex += dt/2 * dir;

	cube->translation = spline.sseGetPointOnSpline(ctrlPts, splineIndex);
	
	cube->Update(deviceContext);


	lineRenderer->Update(deviceContext);
}

// Clear the screen, redraw everything, present
void GameManager::DrawScene()
{
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

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

	//Draw the cube
	cube->Draw(deviceContext);

	// Draw debug lines
	lineRenderer->Draw(deviceContext);

	// Present the buffer
	HR(swapChain->Present(0, 0));
}

#pragma endregion