#include "ModelLoader.h"

ModelLoader::ModelLoader()
{

}

ModelLoader::~ModelLoader()
{

}

void ModelLoader::LoadModel(char* filePath, GeometryNode* node)
{
//START=========================
std::string tmpLine;
std::string colladaFile = "";
std::ifstream myfile("../Resources/Model.dae");
if(myfile.is_open())
{
	while(getline(myfile,tmpLine))
	{
		colladaFile += tmpLine;
	}
	myfile.close();
}
else std::cout << "unable to open file\n";

char *cstr = new char[colladaFile.length()+1];
strcpy(cstr, colladaFile.c_str());
xml_document<> doc;
doc.parse<0>(cstr);

//END==========================
}