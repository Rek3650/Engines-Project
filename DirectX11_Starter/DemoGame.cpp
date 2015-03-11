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
#include "DemoGame.h"

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
	DemoGame game(hInstance);
	
	if( !game.Init() )
		return 0;
	
	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

DemoGame::DemoGame(HINSTANCE hInstance) : DXGame(hInstance)
{
	// Set up our custom caption and window size
	windowCaption = L"Demo DX11 Game";
	windowWidth = 800;
	windowHeight = 600;

	// location of the texture file
	epicTriforce = L"../images/epicTriforce.jpg";
}

DemoGame::~DemoGame()
{
	// Release all of the D3D stuff that's still hanging out
	ReleaseMacro(vertexBuffer);
	ReleaseMacro(indexBuffer);
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(vsConstantBuffer);
	ReleaseMacro(inputLayout);

	delete triangle;
	delete square;
	delete cube;
	delete pentagon;
	delete triMat;
	delete primitiveBatch;
	delete basicEffect;
	delete toQuat;
	delete fromQuat;
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool DemoGame::Init()
{	
	if( !DXGame::Init() )
		return false;

	// create materials
	ID3D11SamplerState* sampState;
	ID3D11ShaderResourceView* SRV;
	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MaxAnisotropy	= 1;
	sampDesc.MipLODBias		= 0;
	sampDesc.MaxLOD			= FLT_MAX;
	sampDesc.MinLOD			= -FLT_MAX;
	HR(device->CreateSamplerState(
		&sampDesc, 
		&sampState));
	HR(CreateWICTextureFromFile(
		device, 
		deviceContext, 
		epicTriforce, 
		0, 
		&SRV));
	triMat = new Material(SRV, sampState);

	// Set up buffers and such
	CreateGeometryBuffers();
	LoadShadersAndInputLayout();

	// variables for making the objects move
	triangleXdir = 1;
	triangleYdir = 1;
	pentagonScaleDir = 1;

	// for drawing lines
	primitiveBatch = new PrimitiveBatch<VertexPositionColor>(deviceContext);
	basicEffect = new BasicEffect(device);

	// initialize the stuff for the spline
	splineIndex = 0;
	dir = 1;
	ctrlPts.clear();
	splinePts.clear();
	// set some default values for the ctrlPts
	/*ctrlPts.push_back(XMFLOAT3(-0.9f, -0.9f, 0));
	ctrlPts.push_back(XMFLOAT3(-0.9f, 0.9f, 0));
	ctrlPts.push_back(XMFLOAT3(0.9f, 0.9f, 0));
	ctrlPts.push_back(XMFLOAT3(0.9f, -0.9f, 0));
	ctrlPts.push_back(XMFLOAT3(-0.9f, -0.9f, 0));
	ctrlPts.push_back(XMFLOAT3(-0.9f, 0.9f, 0));
	ctrlPts.push_back(XMFLOAT3(0.9f, 0.9f, 0));*/
	ctrlPts.push_back(XMFLOAT3(-1.5f, -1, 0));
	ctrlPts.push_back(XMFLOAT3(-1, 0, 0));
	ctrlPts.push_back(XMFLOAT3(-0.5f, 1, 0));
	ctrlPts.push_back(XMFLOAT3(0.5f, -1, 0));
	ctrlPts.push_back(XMFLOAT3(1, 0, 0));
	ctrlPts.push_back(XMFLOAT3(1.5f, 1, 0));

	fromQuat = new XMVECTOR(cube->rotation);
	toQuat = new XMVECTOR(XMQuaternionRotationRollPitchYaw(0, 0, 2000));
	 

	return true;
}

// Creates the vertex and index buffers for a single triangle
void DemoGame::CreateGeometryBuffers()
{
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow	= XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// make a triangle
	Vertex triangleVerts[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), red, XMFLOAT2(0.5, 0) },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), blue, XMFLOAT2(1, 1) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), green, XMFLOAT2(0, 1) },
	};
	UINT triangleInds[] = { 0, 1, 2 };
	triangle = new GameEntity(triangleVerts, 3, triangleInds, 3, device, triMat);
	triangle->scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	triangle->translation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// make a square
	Vertex squareVerts[] = 
	{
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), red, XMFLOAT2(0, 0) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), blue, XMFLOAT2(1, 0) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), green, XMFLOAT2(1, 1) },
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), yellow, XMFLOAT2(0, 1) },
	};
	UINT squareInds[] = 
	{	
		0, 1, 2, 
		3, 0, 2
	};
	square = new GameEntity(squareVerts, 4, squareInds, 6, device, triMat);
	square->scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	square->translation = XMFLOAT3(1.0f, 0.0f, 0.0f);

	Vertex cubeVerts[] =
	{
		{ XMFLOAT3(-0.5, 0.5, 0.5), blue, XMFLOAT2(0, 0) },
		{ XMFLOAT3(0.5, 0.5, 0.5), blue, XMFLOAT2(1, 0) },
		{ XMFLOAT3(0.5, -0.5, 0.5), blue, XMFLOAT2(1, 1) },
		{ XMFLOAT3(-0.5, -0.5, 0.5), blue, XMFLOAT2(0, 1) },
		{ XMFLOAT3(-0.5, 0.5, -0.5), blue, XMFLOAT2(0, 0) },
		{ XMFLOAT3(0.5, 0.5, -0.5), blue, XMFLOAT2(1, 0) },
		{ XMFLOAT3(0.5, -0.5, -0.5), blue, XMFLOAT2(1, 1) },
		{ XMFLOAT3(-0.5, -0.5, -0.5), blue, XMFLOAT2(0, 1) }
	};
	UINT cubeInds[] =
	{
		1, 2, 3,
		0, 1, 3,
		5, 6, 2,
		1, 5, 2,
		4, 7, 6,
		5, 4, 6,
		0, 3, 7,
		4, 0, 7,
		5, 1, 0,
		4, 5, 0,
		2, 6, 7,
		3, 2, 7
	};
	cube = new GameEntity(cubeVerts, 8, cubeInds, 36, device, triMat);
	cube->scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	cube->translation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// make a pentagon
	Vertex pentagonVerts[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), red, XMFLOAT2(0.5f, 0.0f) },
		{ XMFLOAT3(+1.0f, +0.1f, +0.0f), blue, XMFLOAT2(0.0f, 0.1f) },
		{ XMFLOAT3(+0.5f, -1.0f, +0.0f), green, XMFLOAT2(0.25f, 1.0f) },
		{ XMFLOAT3(-0.5f, -1.0f, +0.0f), yellow, XMFLOAT2(0.75f, 1.0f) },
		{ XMFLOAT3(-1.0f, +0.1f, +0.0f), white, XMFLOAT2(1.0f, 0.1f) },
	};
	UINT pentagonInds[] = 
	{ 
		0, 1, 2,
		3, 0, 2,
		4, 0, 3
	};
	pentagon = new GameEntity(pentagonVerts, 5, pentagonInds, 9, device, triMat);
	pentagon->scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	pentagon->translation = XMFLOAT3(-1.0f, 0.0f, 0.0f);
}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void DemoGame::LoadShadersAndInputLayout() 
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
	D3DReadFileToBlob(L"VertexShader.cso", &vsBlob);

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
	D3DReadFileToBlob(L"PixelShader.cso", &psBlob);

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));

	// Clean up
	ReleaseMacro(psBlob);
}

#pragma endregion

#pragma region Window Resizing

// Handles resizing the window and updating our projection matrix to match
void DemoGame::OnResize()
{
	// Handle base-level DX resize stuff
	DXGame::OnResize();

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		AspectRatio(),
		0.1f,
		100.0f);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));
}
#pragma endregion

#pragma region Game Loop
void DemoGame::UpdateScene(float dt)
{
	/*
	// make the triangle move around and stay on screen
	if(triangle->translation.x > 2 || triangle->translation.x < -2) triangleXdir *= -1;
	if(triangle->translation.y > 2 || triangle->translation.y < -2) triangleYdir *= -1;
	triangle->translation = XMFLOAT3(triangle->translation.x+((dt/2)*triangleXdir), triangle->translation.y+(dt*triangleYdir), triangle->translation.z);
	triangle->Update(deviceContext);
	
	// make the square rotate
	square->rotation += dt;
	square->Update(deviceContext);
	
	// make the pentagon pulsate
	if(pentagon->scale.x > 2 || pentagon->scale.x < 0.15) pentagonScaleDir *= -1;
	pentagon->scale = XMFLOAT3(pentagon->scale.x+(dt*pentagonScaleDir), pentagon->scale.y+(dt*pentagonScaleDir), pentagon->scale.z+(dt*pentagonScaleDir));
	pentagon->Update(deviceContext);
	*/
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
	square->translation = spline.getPointOnSpline(ctrlPts, splineIndex);
	square->Update(deviceContext);

	cube->Update(deviceContext);
	Slerp(fromQuat, toQuat, 10, &cube->rotation);

	splinePts.clear();
	splinePts = spline.sseMakeSpline(ctrlPts, 100);
}

void DemoGame::Slerp(XMVECTOR* nQuatFrom, XMVECTOR* nQuatTo, float time, XMVECTOR* nResQuat)
{
	XMFLOAT4* quatFrom = new XMFLOAT4();
	XMFLOAT4* quatTo = new XMFLOAT4();
	XMFLOAT4* resQuat = new XMFLOAT4();

	XMStoreFloat4(quatFrom, *nQuatFrom);
	XMStoreFloat4(quatTo, *nQuatTo);
	XMStoreFloat4(resQuat, *nResQuat);

	float to1[4];
	double omega, cosom, sinom, scale0, scale1;
	// calc cosine
	cosom = quatFrom->x * quatTo->x + quatFrom->y * quatTo->y + quatFrom->z * quatTo->z
		+ quatFrom->w * quatTo->w;
	// adjust signs (if necessary)
	if (cosom <0.0)
	{
		cosom = -cosom; to1[0] = -quatTo->x;
		to1[1] = -quatTo->y;
		to1[2] = -quatTo->z;
		to1[3] = -quatTo->w;
	}
	else  
	{
		to1[0] = quatTo->x;
		to1[1] = quatTo->y;
		to1[2] = quatTo->z;
		to1[3] = quatTo->w;
	}
	// calculate coefficients
	if ((1.0 - cosom) > .001)//DELTA) 
	{
		// standard case (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - time) * omega) / sinom;
		scale1 = sin(time * omega) / sinom;
	}
	else 
	{
		// "from" and "to" quaternions are very close 
		//  ... so we can do a linear interpolation
		scale0 = 1.0 - time;
		scale1 = time;
	}
	// calculate final values
	resQuat->x = scale0 * quatFrom->x + scale1 * to1[0];
	resQuat->y = scale0 * quatFrom->y + scale1 * to1[1];
	resQuat->z = scale0 * quatFrom->z + scale1 * to1[2];
	resQuat->w = scale0 * quatFrom->w + scale1 * to1[3];

	nResQuat = &XMLoadFloat4(resQuat);
	delete(quatFrom);
	delete(quatTo);
	delete(resQuat);
}

// Clear the screen, redraw everything, present
void DemoGame::DrawScene()
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

	// draw the triangle
	//triangle->Draw(deviceContext, pixelShader, vertexShader);
	
	// draw the square
	//square->Draw(deviceContext, pixelShader, vertexShader);

	// draw the pentagon
	//pentagon->Draw(deviceContext, pixelShader, vertexShader);

	//draw the cube
	cube->Draw(deviceContext, pixelShader, vertexShader);
	
	DrawDebugLines();

	// Present the buffer
	HR(swapChain->Present(0, 0));
}

#pragma endregion

#pragma region Draw Debug Lines
// i put the line drawing in this function to try and keep the code in DrawScene cleaner
// this seems to cause errors though so only use for testing
void DemoGame::DrawDebugLines()
{
	basicEffect->Apply(deviceContext);
	
	primitiveBatch->Begin();
	////////////////////////////////////

	primitiveBatch->DrawLine(VertexPositionColor(square->translation, XMFLOAT4()), 
			VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4()));

	for(unsigned int i = 1; i < ctrlPts.size(); i++)
	{
		primitiveBatch->DrawLine(VertexPositionColor(ctrlPts[i-1], XMFLOAT4()), 
			VertexPositionColor(ctrlPts[i], XMFLOAT4()));
	}

	for(unsigned int i = 1; i < splinePts.size(); i++)
	{
		primitiveBatch->DrawLine(VertexPositionColor(splinePts[i-1], XMFLOAT4()), 
			VertexPositionColor(splinePts[i], XMFLOAT4()));
	}

	////////////////////////////////////
	primitiveBatch->End();
}
#pragma endregion

#pragma region Mouse Input

// These methods don't do much currently, but can be used for mouse-related input

void DemoGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hMainWnd);
}

void DemoGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void DemoGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;
}
#pragma endregion