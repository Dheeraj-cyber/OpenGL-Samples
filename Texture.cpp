#include "Texture.h"

Texture::Texture()
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}

Texture::Texture(const char* fileLoc)
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLoc;
}
bool Texture::LoadTexture()
{
	unsigned char *texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);			//unsigned char* is used to represent an image array ( an array of bytes)
	if (!texData)
	{		//if the file location does not exist
		printf("Failed to find: %s\n", fileLocation);
		return false;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);			//GL_TEXTURE_2D is just a flat surface


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//GL_TEXTURE_WRAP_S specifies how to handle the image when it's around the border. S refers to the x-axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//GL_LINEAR means when you zoom in to the image, if you zoom in or zoom out of the image, it will try and blend the pixels together
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);		//Used for mipmapping

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);

	return true;
}

bool Texture::LoadTextureA()
{
	unsigned char *texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);			//unsigned char* is used to represent an image array ( an array of bytes)
	if (!texData)
	{		//if the file location does not exist
		printf("Failed to find: %s\n", fileLocation);
		return false;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);			//GL_TEXTURE_2D is just a flat surface
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//GL_TEXTURE_WRAP_S specifies how to handle the image when it's around the border. S refers to the x-axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//GL_LINEAR means when you zoom in to the image, if you zoom in or zoom out of the image, it will try and blend the pixels together
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);		//Used for mipmapping

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);

	return true;
}	

void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE);		//GL_TEXTURE is the texture unit, when our texture is being run in the shader, it will be a sampler. The sampler is what will have access to the data for our texture. It access it through the texture unit.
	glBindTexture(GL_TEXTURE_2D, textureID);

}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &textureID);		//Delete the texture from memory
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}

Texture::~Texture()
{	//Free up memory after destroying the texture object
	ClearTexture();
}