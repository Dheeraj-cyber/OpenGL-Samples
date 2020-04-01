#include "Skybox.h"

Skybox::Skybox()
{

}

Skybox::Skybox(std::vector<std::string> faceLocations)			//Order = right, left, up, down, back, front
{
	//Shader setup
	skyShader = new Shader();
	skyShader->CreateFromFiles("Shaders/skybox.vert", "Shaders/skybox.frag");

	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();

	//Texture setup
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, bitDepth;

	for (size_t i = 0; i < 6; i++)		//used to loop through our images
	{
		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);			//unsigned char* is used to represent an image array ( an array of bytes)
		if (!texData)
		{		//if the file location does not exist
			printf("Failed to find: %s\n", faceLocations[i].c_str());
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);			//free the image as soon as we finish attaching
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//GL_TEXTURE_WRAP_S specifies how to handle the image when it's around the border. S refers to the x-axis
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//GL_LINEAR means when you zoom in to the image, if you zoom in or zoom out of the image, it will try and blend the pixels together
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Mesh Setup
	unsigned int skyboxIndices[] = {
		// front 
		0, 1, 2,
		2, 1, 3,
		//right
		2, 3, 5,
		5, 3, 7,
		//back
		5, 7, 4,
		4, 7, 6,
		//left
		4, 6, 0,
		0, 6, 1,
		//top
		4, 0, 5,
		5, 0, 2,
		//bottom
		1, 6, 3,
		3, 6, 7
	};

	float skyboxVertices[] = {
		//corners of the back
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		//corners of the front
		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	skyMesh = new Mesh();
	skyMesh->CreateMesh(skyboxVertices, skyboxIndices, 64, 36);		//64 data points in our vertices. 36 data points in our indices
}

void Skybox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));			//create a 3x3 matrix and then convert it to a 4x4 matrix

	glDepthMask(GL_FALSE);			//disables the depth mask, so we will be able to see through our skybox

	skyShader->UseShader();			//Setup the shader being used

	//Attach the values to the shader
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glActiveTexture(GL_TEXTURE0);		//Setup and attach texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);		

	//Make sure that the texture is all fine when our texture is attached to it
	skyShader->Validate();			//our shaders should be validated before they are used

	skyMesh->RenderMesh();			//Render the skybox

	glDepthMask(GL_TRUE);			//switch the depth mask back on
}
Skybox::~Skybox()
{
}


