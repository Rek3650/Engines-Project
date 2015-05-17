#include "LineRenderer.h"


LineRenderer::LineRenderer(ID3D11Device* _device, ID3D11PixelShader* _pixelShader, ID3D11VertexShader* _vertexShader, Camera* _camera)
{
	device = _device;
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	camera = _camera;
	lines.clear();
}


LineRenderer::~LineRenderer(void)
{
	for(int i = 0; i < lines.size(); i++)
	{
		delete lines[i];
	}
	lines.clear();
}

void LineRenderer::addLine(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT4 color)
{
	Vertex verts[] =
	{
		{ p1, color, XMFLOAT2(0, 0) },
		{ p2, color, XMFLOAT2(0, 0) },
	};
	UINT inds[] = { 0, 1 };

	lines.push_back(new GameEntity(verts, 2, inds, 2, device, pixelShader, vertexShader, NULL, NULL, camera));
}

void LineRenderer::Update(ID3D11DeviceContext* deviceContext)
{
	for(int i = 0; i < lines.size(); i++)
	{
		lines[i]->Update(deviceContext);
	}
}

void LineRenderer::Draw(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	for(int i = 0; i < lines.size(); i++)
	{
		lines[i]->Draw(deviceContext);
	}

	// clear the list so we don't draw old lines
	for(int i = 0; i < lines.size(); i++)
	{
		delete lines[i];
	}
	lines.clear();
}