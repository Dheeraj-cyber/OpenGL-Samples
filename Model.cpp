#include "Model.h"

Model::Model()
{

}

void Model::LoadModel(const std::string& fileName)
{
	Assimp::Importer importer;
	//imports our scene
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);		//aiProcess_Triangulate - forces assimp to split up complex shapes into triangles so that we can actually work with them, 
																																										//aiProcess_FlipUVs - flips all the texture UV values along the y-axis
																																										// aiProcess_GenSmoothNormals - generates the normals for our model
																																										// aiProcess_JoinIdenticalVertices - if there are two identical vertices at the same point, it will just treat them as one vertex
	if (!scene)
	{
		printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());
		return;
	}

	LoadNode(scene->mRootNode, scene);

	LoadMaterials(scene);
}

void Model::LoadNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		//Loads all the meshes
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);			//node->mMeshes[i] - holds the mesh number(or the ID of the meshes in that node)
																	//but the sctual mesh is stored in the scene
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLfloat> vertices;			//we are storing our vertices as an array
	std::vector<unsigned int> indices;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });		//mVertices returns an ai vector 3d which actually contains x, y and z
		if (mesh->mTextureCoords[0])		//if texture coordinates exist in the first texture
		{		//the texture coordinates are u and v
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
		}
		else
		{
			//if there are no texture coordinates
			vertices.insert(vertices.end(), { 0.0f, 0.0f });	
		}
		vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });			//'-'ve values indicate that the normals are reversed
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)		//each face will contain the indices that point to the appropriate mesh vertices
	{
		aiFace face = mesh->mFaces[i];			//the faces are stored at the node itself
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);			//go through each face and add them to our indices list	
		}
	}
	
	Mesh* newMesh = new Mesh();
	newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());   //&vertices[0] - get the first location of the first value of vertices, &indices[0] - get the first location of the first value of indices
	meshList.push_back(newMesh);		//add it to our meshlist
	meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* scene)
{
	textureList.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		textureList[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE))		//GetTextureCount - gets the texture count for a specific type of texture
		{
			aiString path;			//DIFFUSE specifies the way the texture will be coloured if the light is hitting it
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int idx = std::string(path.data).rfind("\\");				//erase everything upto the last backslash in the directory
				std::string fileName = std::string(path.data).substr(idx + 1);			//this makes sure that we have the raw fileName. It gets the string starting from the position from where the backslash is and it returns the fileName
			
				std::string texPath = std::string("Textures/") + fileName;

				textureList[i] = new Texture(texPath.c_str());

				if (!textureList[i]->LoadTexture())
				{
					//if the load texture fails
					printf("Failed to load texture at: %s\n", texPath);
					delete textureList[i];			//calls the destructor of Texture
					textureList[i] = nullptr;
				}
			}
		}

		if (!textureList[i])
		{
			textureList[i] = new Texture("Textures/plain.png");				//if the texture is missing, it will just load the plain texture
			textureList[i]->LoadTextureA();
		}
	}
}

void Model::ClearModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		if (meshList[i])			//if there is a list at a position
		{
			delete meshList[i];
			meshList[i] = nullptr;
		}
	}
	
	//run through all the textures and set them to null, if they exist
	for (size_t i = 0; i < textureList.size(); i++)
	{
		if (textureList[i])			//if there is a list at a position
		{
			delete textureList[i];
			textureList[i] = nullptr;
		}
	}
}

void Model::RenderModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		unsigned int materialIndex = meshToTex[i];

		if (materialIndex < textureList.size() && textureList[materialIndex])
		{
			textureList[materialIndex]->UseTexture();
		}

		meshList[i]->RenderMesh();
	}
}

Model::~Model()
{

}
