#include<stdio.h>
#include<string.h>
#include <cmath>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;    //radians = pi/180

GLuint VAO, VBO, IBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.0f;	//the offset will start at zero and move it left and right
float triMaxOffset = 0.7f;	//when the triOffset value hits 0.7, we make it move to the left until it hits -0.7
float triIncrement = 0.0005f;

float curAngle = 0.0f;

float sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.0f;
float minSize = 0.0f;

//Vertex shader
static const char* vShader = "							\n\
#version 330											\n\
														\n\
layout (location = 0) in vec3 pos;						\n\
														\n\
out vec4 vCol;											\n\
														\n\
uniform mat4 model;										\n\
														\n\
void main()												\n\
{														\n\
	gl_Position = model*vec4(pos, 1.0);					\n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);			\n\
}";
//vec3 refers to a vector with 3 values with x,y,z positions
//glPosition is a value to the shader itself. It's an output value
//out vec4 vCol is used to indicate whatever vCol is set to, it will be passed on to another shader. In this case, the fragment shader will be picked up
//vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f); - means that we want to make the color to whatever the position is. The positions are given in GLfloat vertices[] in the createTriangle() method
//clamp is used because we don't want to have any negative values in there because that will be just black


//Fragment shader
static const char* fShader = "							\n\
#version 330											\n\
														\n\
in vec4 vCol;											\n\
														\n\
out vec4 colour;										\n\
														\n\
void main()												\n\
{														\n\
	colour = vCol;										\n\
}";
//in vec4 vCol is used to catch the value that will be given by out vec4 vCol in the vertex shader

void CreateTriangle()
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

	glGenVertexArrays(1, &VAO);			//creates a vertex array on the graphics card. 1 refers to the amount of arrays that we want to create
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Connect the buffer data to the vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		//STATIC_DRAW usually means that the values are static, they cannot be altered

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Note: You should unbind the IBO/EBO AFTER YOU UNBIND THE VAO! 

	glBindVertexArray(0);		//Unbind the vertex array

}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);		//Creates the shader

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);  //grabs the code and puts it into theShader
	glCompileShader(theShader);			//Compiles the shader

	//Error checking
	GLint result = 0;			//This holds the result of the above two functions that we are going to perform
	GLchar eLog[1024] = { 0 };	//This is a place to log the error

	//glLinkProgram(shader);			//creates executables on the graphics card
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);	//GL_LINK_STATUS says whether the program has been linked or not
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: %s\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

//Apply shades to program
void CompileShaders() {
	shader = glCreateProgram();

	//Check for errors while creating the shader
	if (!shader) {
		printf("Error creating shader program\n");
		return;
	}

	//Creates the vertex shader and the fragment shader
	AddShader(shader, vShader, GL_VERTEX_SHADER);		//when you are adding the shader, we know what part of the shader it should be. shader refers to the program that we want to add the shader to. GL_VERTEX_SHADER refers to the type of the shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;			//This holds the result of the above two functions that we are going to perform
	GLchar eLog[1024] = { 0 };	//This is a place to log the error

	glLinkProgram(shader);			//link the program that creates executables on the graphics card
	glGetProgramiv(shader, GL_LINK_STATUS, &result);	//GL_LINK_STATUS says whether the program has been linked or not

	//Check for errors 
	if (!result)
	{ 
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: %s\n", eLog);
		return;
	}

	glValidateProgram(shader);			//Validates the program
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);	//GL_LINK_STATUS says whether the program has been linked or not
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: %s\n", eLog);
		return;
	}

	//get the actual ID or location of the uniform variable
	uniformModel = glGetUniformLocation(shader, "model");		//shader refers to the shader program itself, and xMove is the name of the variable in the shader
	//uniform model will be the location of the model matrix
}


int main() 
{
	//initialize GLFW
	if (!glfwInit())
	{
		printf("GLFW initialization falied!");
		glfwTerminate();
		return 1;
	}

	//Setup GLFW window properties 
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Core profile - No backward compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);   //profile is basically saying how openGL is treating the code it's passed. The CORE_PROFILE means that it wont be backwards compatible, you won't be able to use old/depreciated features
	//Allow foreward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	//Get buffer size information
	//Buffer is the area in the middle of the window, that will be the size of the opengl viewport
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);   //Get the size of the mainWindow and assign it to bufferWidth and bufferHeight

	//Set context for GLEW to use
	//This lets GLEW know that this window you created here is the one with the OpenGL content should be tied to, so when everything gets drawn, it should get drawn to this window
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension features
	glewExperimental = GL_TRUE;		//Allows us to use experimental features

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("Error: %s",glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);			//Enables depth testing to determine which triangles are deeper into the image effectively, and therefore which ones to be drawn on top of the others

	//Create Viewport and Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	// loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();
		
		if (direction) {
			triOffset += triIncrement;		//increment the triOffset value if you are heading towards the right
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset) {
			direction = !direction;
		}

		curAngle += 0.01f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

		if (direction)
		{
			curSize += 0.0001f;
		}
		else {
			curSize -= 0.0001f;
		}

		if (curSize >= maxSize || curSize <= minSize) {
			sizeDirection = !sizeDirection;
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			//Clears the entire screen. RGB format
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clears the color buffer bit and the depth buffer bit 

		glUseProgram(shader);

		glm::mat4 model(1.0f);	//creates a 4x4 identity matrix
		
		model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));    //glm::vec3(0.0f, 0.0f, 1.0f) is used to spin the model aroud the z-axis, which is the axis pointing forward and backward from us.
		//model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));	//Apply translation to the identity matrix. transaltion is used to move a set of points 
		
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));		//scale in the x axis by 2, y axis by 2 and the z axis by 1
		
		
		//glUniform1f(uniformXMove, triOffset);		Here, since we have attached the shader, we want to set the uniform value to the value of triOffset. uniformXMove is the location in the shader
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));	//GL_FALSE is used when we don't want to transpose the matrix. value_ptr is used because the model is not directly in a raw format
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);		//0 is the first point of the triangle and 3 refers to the amount of vertices we want to draw
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);          //12 because it is in 3D, 3 sides*3 sides*3 sides*3 sides
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}