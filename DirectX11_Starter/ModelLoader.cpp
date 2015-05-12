#include "ModelLoader.h"

ModelLoader::ModelLoader()
{

}

ModelLoader::~ModelLoader()
{

}

Mesh* ModelLoader::LoadModel(char* filePath, ID3D11Device* device)
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

	int vertLength = atoi(posNode->first_node()->first_attribute("count")->value());
	int indLength = atoi(norNode->first_node()->first_attribute("count")->value())*2;

	
	Vertex* vertArray;
	vertArray = new Vertex[vertLength/3];
	//{ XMFLOAT3(-0.5, 0.5, 0.5), color, XMFLOAT2(0, 0) }

	UINT* indArray;
	indArray = new unsigned int[indLength];

	char* token = NULL;

	char* tmpChar = NULL;
	tmpChar = strtok_s(posNode->first_node()->value(), " ", &token);

	double tmp1, tmp2, tmp3 = 0;
	int index = 0;

	for(int i = 0; i < vertLength; i += 3)
	{
		tmp1 = atof(tmpChar);
		tmpChar = strtok_s(NULL, " ", &token);
		tmp2 = atof(tmpChar);
		tmpChar = strtok_s(NULL, " ", &token);
		tmp3 = atof(tmpChar);
		tmpChar = strtok_s(NULL, " ", &token);
		vertArray[index].Position = DirectX::XMFLOAT3(tmp1, tmp2, tmp3);
		vertArray[index].Color = DirectX::XMFLOAT4(0, 0, 0, 1);
		vertArray[index].uv =  XMFLOAT2(0, 0);
		index++;
	}

	token = NULL;
	tmpChar = strtok_s(polyNode->first_node("p")->value(), " ", &token);

	for(int i = 0; i < indLength; i++) 
	{
		indArray[i] = atoi(tmpChar);
		tmpChar = strtok_s(NULL, " ", &token);
	}
	/*
	std::cout << "Verticies: " << std::endl;
	for(int i = 0; i < vertLength/3; i++)
	{
		std::cout << "x: " << vertArray[i].Position.x << " y: " << vertArray[i].Position.y << " z: " << vertArray[i].Position.z << std::endl;
		std::cout << "color: " << vertArray[i].Color.x << ", " << vertArray[i].Color.y << ", " << vertArray[i].Color.z << ", " << vertArray[i].Color.w << std::endl;
		std::cout << "uv: " << vertArray[i].uv.x << ", " << vertArray[i].uv.y << std::endl;
	}
		std::cout << "num vert: " << vertLength/3 << std::endl;
	*/

	return new Mesh(vertArray, vertLength/3, indArray, indLength, device);

//END==========================
}