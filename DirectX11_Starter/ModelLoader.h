#pragma once

#include "rapidxml.hpp"
#include <fstream>
#include "GeometryNode.h"
#include <iostream>

using namespace rapidxml;

class ModelLoader{
public:
	ModelLoader();
	~ModelLoader();

	void LoadModel(char* filePath, GeometryNode* node);
private:

};