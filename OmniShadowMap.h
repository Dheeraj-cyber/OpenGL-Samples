#pragma once
#include "ShadowMap.h"
class OmniShadowMap :
	public ShadowMap
{
public:
	OmniShadowMap();

	bool Init(GLuint width, GLuint height);		//initializer. Here, we are setting up our texture

	void write();			// render to the shadowMap

	void Read(GLenum textureUnit);

	~OmniShadowMap();

private:

};

