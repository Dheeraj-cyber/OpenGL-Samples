#pragma once

#include <stdio.h>

#include <GL/glew.h>

class ShadowMap
{
public:
	ShadowMap();

	virtual bool Init(GLuint width, GLuint height);		//initializer. Here, we are setting up our texture
	
	virtual void write();			// render to the shadowMap

	virtual void Read(GLenum textureUnit);

	GLuint GetShadowWidth() { return shadowWidth;  }
	GLuint GetShadowHeight() { return shadowHeight;  }

	~ShadowMap();

protected:
	GLuint FBO, shadowMap;
	GLuint shadowWidth, shadowHeight;			// to make sure that the viewport is of the same dimensions as the shadow map texture 
};

