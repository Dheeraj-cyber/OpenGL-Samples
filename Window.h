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

	void swapBuffers()
	{
		return glfwSwapBuffers(mainWindow);
	}

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;
};

