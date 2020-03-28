#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
	FBO = 0;
	shadowMap = 0;
}

bool ShadowMap::Init(GLuint width, GLuint height)
{
	shadowWidth = width;			shadowHeight = height;

	glGenFramebuffers(1, &FBO);			//Generates the frame buffer. A frame buffer is basically the screen. We will be creating another frame buffer that won't be visible on the screen and then, we are going to draw the texture to that, to reflect it on the actual screen, i.e. the default frame buffer
										//Here 1 - no. of frame buffers. &FBO - reference of frame buffer object
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);		//0 - mipmap level, GL_DEPTH_COMPONENT - says how deep the image is based on the normalized coordinates
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	//GL_TEXTURE_WRAP_S specifies how to handle the image when it's around the border. S refers to the x-axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float bColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColour);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);		//GL_LINEAR means when you zoom in to the image, if you zoom in or zoom out of the image, it will try and blend the pixels together
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);		//binds the FBO ID with the frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);		//Here, 0 specifies the mipmap level

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);			//check for errors

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{		//If the frame buffer failed
		printf("Framebuffer Error: %i\n", status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);			//unbind the framebuffer

	return true;
}

void ShadowMap::write()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);			//draws to the alternative frame buffer, i.e. the buffer which cannot be seen

}

void ShadowMap::Read(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}

ShadowMap::~ShadowMap()
{
	if (FBO)
	{
		//if we have a FBO that is destroyed
		glDeleteFramebuffers(1, &FBO);
	}

	if (shadowMap)
	{	//if there is a shadow map
		glDeleteTextures(1, &shadowMap);
	}
}
