#pragma once

#include<GL/glew.h>
class Mesh
{
	//Mesh is the name usually used for model information so all the points and the actual sort of outside of the model itself
	
//Functions
public:
	Mesh();
	
	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);		//Creates our mesh
	void RenderMesh();		//Draws the mesh to the screen
	void ClearMesh();		//Deletes the mesh from the graphic's card
	
	~Mesh();
	//Variables
private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

 