#pragma once

#include<GL/glew.h>

class Material
{
public:
	Material();
	Material(GLfloat sIntensity, GLfloat shine);
	
	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);			//Specifies the lighting for our texture
	
	~Material();

private:
	GLfloat specularIntensity;		//Specifies how much light should be there on the object
	GLfloat shininess;			//Shininess specifies how smooth the object will look.  High value - light is very bright
};

