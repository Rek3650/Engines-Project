#pragma once

#include "rapidxml.hpp"
#include <fstream>
#include "GeometryNode.h"
#include <iostream>
#include "Mesh.h"
#include <DirectXMath.h>

using namespace rapidxml;

class ModelLoader{
public:
	ModelLoader();
	~ModelLoader();

	Mesh* LoadModel(char* filePath, ID3D11Device* device);
private:

};