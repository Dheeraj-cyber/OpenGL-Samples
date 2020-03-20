#include "Shader.h"

Shader::Shader()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;

}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);	//this will pass in the vertex location, i.e. Shaders/shader.vert
	std::string  fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read %s! File doesn't exist.", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderID = glCreateProgram();

	//Check for errors while creating the shader
	if (!shaderID) {
		printf("Error creating shader program\n");
		return;
	}

	//Creates the vertex shader and the fragment shader
	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);		//when you are adding the shader, we know what part of the shader it should be. shader refers to the program that we want to add the shader to. GL_VERTEX_SHADER refers to the type of the shader
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;			//This holds the result of the above two functions that we are going to perform
	GLchar eLog[1024] = { 0 };	//This is a place to log the error

	glLinkProgram(shaderID);			//link the program that creates executables on the graphics card
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);	//GL_LINK_STATUS says whether the program has been linked or not

	//Check for errors 
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: %s\n", eLog);
		return;
	}

	glValidateProgram(shaderID);			//Validates the program
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);	//GL_LINK_STATUS says whether the program has been linked or not
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: %s\n", eLog);
		return;
	}

	//uniform model will be the location of the model matrix
	uniformProjection = glGetUniformLocation(shaderID, "projection");   //get the projection variable and place the id of the location in uniformProjection
	//get the actual ID or location of the uniform variable
	uniformModel = glGetUniformLocation(shaderID, "model");		//shader refers to the shader program itself, and xMove is the name of the variable in the shader
	uniformView = glGetUniformLocation(shaderID, "view");	//when the shader is created, we will get the uniform view and location and put it in uniformView
}

GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
	return uniformModel;
}

GLuint Shader::GetViewLocation()
{
	return uniformView;
}

void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);		//Removes the program from the graphics card
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

//Apply shades to program
void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
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

Shader::~Shader()
{
	ClearShader();
}