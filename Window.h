#pragma once

#include<stdio.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

class Window
{
public:
	Window();
	
	Window(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLfloat getBufferWidth()		//Used for projection calculation
	{
		return bufferWidth;
	}

	GLfloat getBufferHeight() 
	{
		return bufferHeight;
	}

	bool getShouldClose()		//if the window should close
	{
		return glfwWindowShouldClose(mainWindow);
	}

	bool* getKeys()   //grab the set of keys once from the window class, pass the pointer to that array, move the camera and then have the camera check that array itself
	{
		return keys;
	}

	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers()
	{
		glfwSwapBuffers(mainWindow);
	}

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024];	//When you press a key on the keyboard, GLFW will pick it up and make the assign the boolean at that index number in the keys array to true and when you lift off the key, it will set it back to false

	GLfloat lastX;	//every time the mouse is moved, we want to see what the last coordinates were in comparison to the current coordinates and use that to calculate how much has changed since the last movement
	GLfloat lastY;	
	GLfloat xChange;			//calculates the change
	GLfloat yChange;
	bool mouseFirstMoved;		//when we first put the cursor on the screen, it could be anywhere on the screen and it could measure that movement.

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);		// GLFW will be doing a callback and we need a static function to do a callback to a regular member, a normal member function won't work in this case
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

