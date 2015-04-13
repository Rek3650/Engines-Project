#pragma once

#include <DirectXMath.h>
#include "GameEntity.h"
#include "Camera.h"
#include <vector>

using namespace DirectX;

class LineRenderer
{
public:
	LineRenderer(ID3D11Device* _device, ID3D11PixelShader* _pixelShader, ID3D11VertexShader* _vertexShader, Camera* _camera=NULL);
	~LineRenderer(void);

	void addLine(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT4 color=XMFLOAT4(0, 0, 0, 1));

	void Update(ID3D11DeviceContext* deviceContext);
	void Draw(ID3D11DeviceContext* deviceContext);

	Camera* camera;

private:
	ID3D11Device* device;
	ID3D11PixelShader* pixelShader; 
	ID3D11VertexShader* vertexShader;
	std::vector<GameEntity*> lines;
};

