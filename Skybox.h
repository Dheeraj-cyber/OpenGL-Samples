#pragma once

#include<vector>			//Pass a vector of images into our skybox
#include<string>			//for error messages

#include<GL/glew.h>

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"

class Skybox
{
public:
	Skybox();

	Skybox(std::vector<std::string> faceLocations);

	void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);			//Where to draw the camera from and the projection we are using when we are drawing the skybox

	~Skybox();

private:
	Mesh* skyMesh;
	Shader* skyShader;

	GLuint textureID;
	GLuint uniformProjection, uniformView;				//gets the uniform projection location of the view matrix
};

