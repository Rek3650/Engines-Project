#include "ModelLoader.h"

ModelLoader::ModelLoader()
{

}

ModelLoader::~ModelLoader()
{

}

void ModelLoader::LoadModel(char* filePath)
{
//START=========================
std::string tmpLine;
std::string colladaFile = "";
std::ifstream myfile(filePath);
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
strcpy_s(cstr, colladaFile.length()+1, colladaFile.c_str());
xml_document<> doc;
doc.parse<0>(cstr);

xml_node<>* geomNode = doc.first_node()->first_node("library_geometries")->first_node("geometry");
char *objName = geomNode->first_attribute()->value();
xml_node<>* posNode = geomNode->first_node()->first_node("source");
xml_node<>* norNode = posNode->next_sibling();
xml_node<>* polyNode = geomNode->first_node()->first_node("polylist");

int length = atoi(posNode->first_node()->first_attribute("count")->value());

int* vertArray;
vertArray = new int[length];
char* token;
char* posArray = strtok_s(posNode->first_node()->value(),"", &token);

for(int i = 0; i < length; i++)
{
	std::cout << posArray[i] << std::endl;
}

return;

//END==========================
}