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

// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// For DirectX Math
using namespace DirectX;

// Demo class which extends the base DXGame class
class DemoGame : public DXGame
{
public:
	DemoGame(HINSTANCE hInstance);
	~DemoGame();

	// Overrides for base level methods
	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	// For handing mouse input
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	// for holding all of the debug line draws
	void DrawDebugLines();

private:
	// Initialization for our "game" demo
	void CreateGeometryBuffers();
	void LoadShadersAndInputLayout();
	XMVECTOR Slerp(XMVECTOR* quatFrom, XMVECTOR* quatTo, float time, XMVECTOR* resQuat);
	XMVECTOR SlerpSSE(XMVECTOR* quatFrom, XMVECTOR* quatTo, float time, XMVECTOR* resQuat);

private:
	// Buffers to hold actual geometry
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Our basic shaders for this example
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;

	// A few more odds and ends we'll need
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vsConstantBuffer;
	VertexShaderConstantBuffer vsConstantBufferData;

	// The matrices to go from 3D model space
	// to screen space
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	XMVECTOR* fromQuat;
	XMVECTOR* toQuat;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	// test Game Entities
	std::vector<GameEntity*> cubes;
	GameEntity* cube;

	// texture stuff
	const wchar_t* epicTriforce;
	Material* triMat;

	// for debug lines
	BasicEffect* basicEffect;
	PrimitiveBatch<VertexPositionColor>* primitiveBatch;

	// for creating a spline
	std::vector<XMFLOAT3> ctrlPts;
	std::vector<XMFLOAT3> splinePts;
	Spline spline;
	float splineIndex;
	int dir;

	float elapsedTime;
};