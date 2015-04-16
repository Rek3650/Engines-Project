#pragma once

#include<list>
#include <DirectXMath.h>
#include "Mesh.h"
class SceneNode
{
public:
	SceneNode();
	virtual ~SceneNode();
	void Release();
	virtual void Update(ID3D11DeviceContext* deviceContext);
	virtual void Draw(ID3D11DeviceContext* deviceContext);
	void Destroy();
	void AddChildren (SceneNode* nNode);
	
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT4 GetRotation();
	DirectX::XMFLOAT3 GetScale();

	void SetTranslation(DirectX::XMFLOAT3 nPos);
	void SetRotation(DirectX::XMFLOAT4 nRot);
	void SetScale(DirectX::XMFLOAT3 nScale);
protected:
	std::list<SceneNode*> m_lstChildren;

	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT3 translation;
};