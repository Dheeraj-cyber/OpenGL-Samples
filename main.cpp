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

GLuint VAO, VBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.0f;	//the offset will start at zero and move it left and right
float triMaxOffset = 0.7f;	//when the triOffset value hits 0.7, we make it move to the left until it hits -0.7
float triIncrement = 0.0005f;



//Vertex shader
static const char* vShader = "							\n\
#version 330											\n\
														\n\
layout (location = 0) in vec3 pos;						\n\
														\n\
uniform mat4 model;									\n\
														\n\
void main()												\n\
{														\n\
	gl_Position = model*vec4(0.4*pos.x, 0.4*pos.y, pos.z, 1.0);	\n\
}";
//vec3 refers to a vector with 3 values with x,y,z positions
//glPosition is a value to the shader itself. It's an output value



//Fragment shader
static const char* fShader = "							\n\
#version 330											\n\
														\n\
out vec4 colour;										\n\
														\n\
void main()												\n\
{														\n\
	colour = vec4(1.0, 0.0, 0.0, 1.0);	\n\
}";

void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0f,-1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);			//creates a vertex array on the graphics card. 1 refers to the amount of arrays that we want to create
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Connect the buffer data to the vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		//STATIC_DRAW usually means that the values are static, they cannot be altered

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

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

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			//Clears the entire screen. RGB format
		glClear(GL_COLOR_BUFFER_BIT);		//Clears the color buffer

		glUseProgram(shader);

		glm::mat4 model(1.0f);	//creates a 4x4 identity matrix
		model = glm::translate(model, glm::vec3(triOffset, triOffset, 0.0f));	//Apply translation to the identity matrix. transaltion is used to move a set of points 

		//glUniform1f(uniformXMove, triOffset);		Here, since we have attached the shader, we want to set the uniform value to the value of triOffset. uniformXMove is the location in the shader
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));	//GL_FALSE is used when we don't want to transpose the matrix. value_ptr is used because the model is not directly in a raw format
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);		//0 is the first point of the triangle and 3 refers to the amount of vertices we want to draw
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}