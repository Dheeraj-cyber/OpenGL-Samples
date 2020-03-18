#include<stdio.h>
#include<string.h>
#include <cmath>
#include<vector>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"


const float toRadians = 3.14159265f / 180.0f;    //radians = pi/180

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;




//Vertex shader
static const char* vShader = "Shaders/shader.vert";
//vec3 refers to a vector with 3 values with x,y,z positions
//glPosition is a value to the shader itself. It's an output value
//out vec4 vCol is used to indicate whatever vCol is set to, it will be passed on to another shader. In this case, the fragment shader will be picked up
//vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f); - means that we want to make the color to whatever the position is. The positions are given in GLfloat vertices[] in the createTriangle() method
//clamp is used because we don't want to have any negative values in there because that will be just black


//Fragment shader
static const char* fShader = "Shaders/shader.frag";
//in vec4 vCol is used to catch the value that will be given by out vec4 vCol in the vertex shader

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,				//Draws the 0th point first, 3rd next and then the 1st
		1, 3, 2, 
		2, 3, 0,
		0, 1, 2 
	};

	GLfloat vertices[] = {
		-1.0f,-1.0f, 0.0f,      //black , bottom left
		0.0f, -1.0f, 1.0f,		//goes into the background
		1.0f, -1.0f, 0.0f,		//red, bottom right
		0.0f, 1.0f, 0.0f		//green, top
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);			//For vectors for just adding it to the end of the list
	
	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj2);			//For vectors for just adding it to the end of the list
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	GLuint uniformProjection = 0, uniformModel = 0;

	glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);		//Divide the width by the height to get the aspect ratio

	// loop until window closed
	while (!mainWindow.getShouldClose())
	{
		// Get + Handle user input events
		glfwPollEvents();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			//Clears the entire screen. RGB format
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clears the color buffer bit and the depth buffer bit 

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();

		glm::mat4 model(1.0f);	//creates a 4x4 identity matrix

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));	//Apply translation to the identity matrix. transaltion is used to move a set of points 
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));    //glm::vec3(0.0f, 0.0f, 1.0f) is used to spin the model aroud the z-axis, which is the axis pointing forward and backward from us.
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));		//scale in the x axis by 2, y axis by 2 and the z axis by 1
		//glUniform1f(uniformXMove, triOffset);		Here, since we have attached the shader, we want to set the uniform value to the value of triOffset. uniformXMove is the location in the shader
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));	//GL_FALSE is used when we don't want to transpose the matrix. value_ptr is used because the model is not directly in a raw format
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);	//Creates a identity matrix
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}