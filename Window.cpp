#include "Window.h"

Window::Window()
{
	xChange = 0.0f;
	yChange = 0.0f;

	width = 800;
	height = 600;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	xChange = 0.0f;
	yChange = 0.0f;

	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
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

	//Create the window
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

	//Handle key + mouse input
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	glfwSetWindowUserPointer(mainWindow, this);			//this refers to the user of this window
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);		//When a key is pressed on mainWindow, then callback to the handlekeys function
	glfwSetCursorPosCallback(mainWindow, handleMouse);		//When it notices anything happening with the cursors position on the screen, on the main window it will notify the handleMouse function
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));			//get the user pointer of the mainWindow which in turn allows us to access bool keys by going through the window
	
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);		//When the user presses the escape key, the window will close
	}

	if (key >= 0 && key < 1024)
	{
		//When the user presses the key, set it to true...else set it to false
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			printf("Pressed: %d\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			printf("Released: %d\n", key);
		}
	}
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (theWindow->mouseFirstMoved)		//if it is the very first movement
	{
		theWindow->lastX = xPos;			//initialize last position to the current position
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;		//it's no longer the first movement

	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

	/*printf("x:%.6f, y:%.6f\n", theWindow->xChange, theWindow->yChange);*/
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}