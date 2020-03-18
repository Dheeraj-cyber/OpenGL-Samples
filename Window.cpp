#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
}

int Window::Initialise()
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

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	//Get buffer size information
	//Buffer is the area in the middle of the window, that will be the size of the opengl viewport

	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);   //Get the size of the mainWindow and assign it to bufferWidth and bufferHeight

	//Set context for GLEW to use
	//This lets GLEW know that this window you created here is the one with the OpenGL content should be tied to, so when everything gets drawn, it should get drawn to this window
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension features
	glewExperimental = GL_TRUE;		//Allows us to use experimental features

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);			//Enables depth testing to determine which triangles are deeper into the image effectively, and therefore which ones to be drawn on top of the others

	//Create Viewport and Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}