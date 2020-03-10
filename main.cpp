#include<stdio.h>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

//Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

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

	//Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	// loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();

		// Clear window
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);			//Clears the entire screen. RGB format
		glClear(GL_COLOR_BUFFER_BIT);		//Clears the color buffer

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}