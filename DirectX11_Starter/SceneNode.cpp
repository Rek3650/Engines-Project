#include "SceneNode.h"
#include <iostream>
SceneNode::SceneNode()
{
	translation = DirectX::XMFLOAT3(0,0,0);
	rotation = DirectX::XMFLOAT4(0,0,0,1);
	scale = DirectX::XMFLOAT3(0,0,0);
}

SceneNode::~SceneNode()
{
	Destroy();
}

void SceneNode::Release()
{
	delete this;
}

void SceneNode::SetTranslation(DirectX::XMFLOAT3 nPos)
{
	for(std::list<SceneNode*>::iterator i =  m_lstChildren.begin();
		i != m_lstChildren.end(); i++)
	{
		float xDiff = (*i)->GetPosition().x - GetPosition().x;
		float yDiff = (*i)->GetPosition().y - GetPosition().y;
		float zDiff = (*i)->GetPosition().z - GetPosition().z;
		(*i)->SetTranslation(DirectX::XMFLOAT3(nPos.x + xDiff,nPos.y +yDiff,nPos.z+zDiff));
	}
	translation = nPos;
}

void SceneNode::SetRotation(DirectX::XMFLOAT4 nRot)
{
	rotation = nRot;
	
	for(std::list<SceneNode*>::iterator i =  m_lstChildren.begin();
		i != m_lstChildren.end(); i++)
	{
		(*i)->SetRotation(nRot);
	}
}

void SceneNode::SetScale(DirectX::XMFLOAT3 nScale)
{
	scale = nScale;
	
	for(std::list<SceneNode*>::iterator i =  m_lstChildren.begin();
		i != m_lstChildren.end(); i++)
	{
		(*i)->SetScale(nScale);
	}
}

DirectX::XMFLOAT3 SceneNode::GetPosition()
{
	return translation;
}

DirectX::XMFLOAT4 SceneNode::GetRotation()
{
	return rotation;
}

DirectX::XMFLOAT3 SceneNode::GetScale()
{
	return scale;
}

//Where we update the objects logic
void SceneNode::Update(ID3D11DeviceContext* deviceContext)
{
	//Loop through and update children
	for(std::list<SceneNode*>::iterator i =  m_lstChildren.begin();
		i != m_lstChildren.end(); i++)
	{
		(*i)->Update(deviceContext);
	}
}

void SceneNode::Draw(ID3D11DeviceContext* deviceContext)
{
	//Loop through and update children
	for(std::list<SceneNode*>::iterator i =  m_lstChildren.begin();
		i != m_lstChildren.end(); i++)
	{
		(*i)->Draw(deviceContext);
	}
}

//Destroy all children
void SceneNode::Destroy()
{
	for(std::list<SceneNode*>::iterator i = m_lstChildren.begin();
		i != m_lstChildren.end(); i++)
	{
		(*i)->Release();
	}

	m_lstChildren.clear();
}

void SceneNode::AddChildren(SceneNode* nNode)
{
	m_lstChildren.push_back(nNode);
}