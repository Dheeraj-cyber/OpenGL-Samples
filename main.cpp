#define STB_IMAGE_IMPLEMENTATION

#include<stdio.h>
#include<string.h>
#include <cmath>
#include<vector>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"
#include "SpotLight.h"
#include "Model.h"



const float toRadians = 3.14159265f / 180.0f;    //radians = pi/180

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;			//has high shininess and very high specular intensity
Material dullMaterial;			//has low shininess and low specular intensity

Model xwing;
Model blackhawk;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;		//change in time from the last time we chaecked
GLfloat lastTime = 0.0f;

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

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);		//Here the values that we get in case of the triangle will be 1, 2, 0
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);		//Here instead of in1, we are using in2
		glm::vec3 normal = glm::cross(v1, v2);		//Cross product finds the line which is poking towards us or away from us
		normal = glm::normalize(normal);			//Returns a unit vector, since the vectors should be of the same length
	
		in0 += normalOffset;	in1 += normalOffset;	in2 += normalOffset;
		vertices[in0] += normal.x;		vertices[in0 + 1] += normal.y;		vertices[in0 + 2] += normal.z;	
		vertices[in1] += normal.x;		vertices[in1 + 1] += normal.y;		vertices[in1 + 2] += normal.z;	
		vertices[in2] += normal.x;		vertices[in2 + 1] += normal.y;		vertices[in2 + 2] += normal.z;	
	}

	//go through all the vertices and jump straight to the end of each one and also normalize the values
	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;		//Every time when we go through the loop, it is effectively counting up each of these 0 1 2 3 and then grabbing the relevant one from the list of vertices and going to the offset section of it  
		
		//Go through each of the rows of the vertices array, get the start of the normal offsets grabbing each of those normals that we've just calculated up here by going through this loop few times going through each one.
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);		
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;	vertices[nOffset + 1] = vec.y;	vertices[nOffset + 2] = vec.z;
	}

}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,				//Draws the 0th point first, 3rd next and then the 1st
		1, 3, 2, 
		2, 3, 0,
		0, 1, 2 
	};

	GLfloat vertices[] = {
	//	x		y	  z				u		v	 normals  nx	ny	 nz           u and v refer to the texture coordinates
		-1.0f,-1.0f, -0.6f,		   0.0f,   0.0f,		0.0f, 0.0f, 0.0f,		//black , bottom left
		0.0f, -1.0f, 1.0f,  	   0.5f,   0.0f,		0.0f, 0.0f, 0.0f,		//goes into the background
		1.0f, -1.0f, -0.6f,	       1.0f,   0.0f,		0.0f, 0.0f, 0.0f,		//red, bottom right
		0.0f, 1.0f, 0.0f,		   0.5f,   1.0f,		0.0f, 0.0f, 0.0f		//green, top
	};

	unsigned int floorIndices[] = {
		0, 2, 1,   //top-left, bottom-left, top-right
		1, 2, 3	//go backwards along that line ==> previous 0, "2, 1" ==> "1,2" , 3 ; 3 specifies bottom right
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	  0.0f, 0.0f,		0.0f, -1.0f, 0.0f,	//x,y,z = back left corner; u, v = top left of the image. Here the normals 0.0f, -1.0f, 0.0f will be pointing to the top left of the image
		10.0f, 0.0f, -10.0f,	  10.0f, 0.0f,		0.0f, -1.0f, 0.0f,	//x, y, z = back right; u, v = top right of the image
		-10.0f, 0.0f, 10.0f,	  0.0f, 10.0f,		0.0f, -1.0f, 0.0f,	//front left
		10.0f, 0.0f, 10.0f,		  10.0f, 10.0f,		0.0f, -1.0f, 0.0f	//front right
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);		//amount of indices - 12, no. of vertices - 32, size of each vertex - 8, normal offset - 5. Note: When the indices are altered, the vertices will be altered as well

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);			//For vectors for just adding it to the end of the list
	
	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);			//For vectors for just adding it to the end of the list

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);			//we have four points that we need to define ==> 4*8 = 32. We have 6 indices
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);			//start at the middle

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 256);			//the value for shine goes in powers of 2 = 2, 4, 8, 16, 32, 64.............
	dullMaterial = Material(0.3f, 4);

	xwing = Model();
	xwing.LoadModel("Models/x-wing.obj");

	blackhawk = Model();
	blackhawk.LoadModel("Models/uh60.obj");

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								 0.3f, 0.6f, 
								 0.0f, 0.0f, -1.0f);					//change the fourth parameter 0.2f, to increase or decrease the intensity of diffuse light
	
	unsigned int pointLightCount = 0;

	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
								0.0f, 0.1f,
								0.0f, 0.0f, 0.0f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;
	
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
								0.0f, 0.1f,
								-4.0f, 2.0f, 0.0f,
								0.3f, 0.1f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
								0.0f, 2.0f,
								0.0f, 0.0f, 0.0f,
								0.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f,
								20.0f);				//20.0f indicates the angle of our spotlight. In this case, 20 degrees.
	spotLightCount++;
	
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
								0.0f, 1.0f,
								0.0f, -1.5f, 0.0f,
								-100.0f, -1.0f, 0.0f,
								1.0f, 0.0f, 0.0f,
								20.0f);				//20.0f indicates the angle of our spotlight. In this case, 20 degrees.
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat) mainWindow.getBufferWidth()/ mainWindow.getBufferHeight(), 0.1f, 100.0f);		//Divide the width by the height to get the aspect ratio


	// loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();		//If you are using SDL instead of GLFW, you can use SDL GetPerformanceCounter();
		deltaTime = now - lastTime;		//how long it took the last loop to go around. In SDL ..... (now - lastTime)*1000/SDL_GetPerformanceFrequency()
		lastTime = now;

		// Get + Handle user input events
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			//Clears the entire screen. RGB format
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clears the color buffer bit and the depth buffer bit 

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();			//Holds the uniform value for the position of our camera
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		
		glm::mat4 model(1.0f);	//creates a 4x4 identity matrix

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));	//Apply translation to the identity matrix. transaltion is used to move a set of points 
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));    //glm::vec3(0.0f, 0.0f, 1.0f) is used to spin the model aroud the z-axis, which is the axis pointing forward and backward from us.
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));		//scale in the x axis by 2, y axis by 2 and the z axis by 1
		//glUniform1f(uniformXMove, triOffset);		Here, since we have attached the shader, we want to set the uniform value to the value of triOffset. uniformXMove is the location in the shader
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));	//GL_FALSE is used when we don't want to transpose the matrix. value_ptr is used because the model is not directly in a raw format
		brickTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0f);	//Creates a identity matrix
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();
		
		model = glm::mat4(1.0f);	//Creates a identity matrix
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		
		model = glm::mat4(1.0f);	//Creates a identity matrix
		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		xwing.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		blackhawk.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}