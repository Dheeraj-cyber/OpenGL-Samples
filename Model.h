#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"

class Model
{
public:
	Model();
	
	void LoadModel(const std::string& fileName);			//takes the filename of our model
	void RenderModel();
	void ClearModel();

	~Model();

private:

	void LoadNode(aiNode *node, const aiScene *scene);			//node holds all the data. scene holds one specific value
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);

	std::vector<Mesh*> meshList;			//Holds all the meshes
	std::vector<Texture*> textureList;		//Holds all the textures
	std::vector<unsigned int> meshToTex;
};

