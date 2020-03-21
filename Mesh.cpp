#include "Mesh.h"

Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO);			//creates a vertex array on the graphics card. 1 refers to the amount of arrays that we want to create
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Connect the buffer data to the vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);		//STATIC_DRAW usually means that the values are static, they cannot be altered

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*5, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*5, (void*)(sizeof(vertices[0])*3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Note: You should unbind the IBO/EBO AFTER YOU UNBIND THE VAO! 

	glBindVertexArray(0);		//Unbind the vertex array

}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);		//0 is the first point of the triangle and 3 refers to the amount of vertices we want to draw
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);          //12 because it is in 3D, 3 sides*3 sides*3 sides*3 sides
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);			//Deletes the buffer from your graphics card memory, for garbage collection
		IBO = 0;
	}
	
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);			//Deletes the buffer from your graphics card memory, for garbage collection
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);			//Deletes the Vertex array from your graphics card memory, for garbage collection
		VAO = 0;
	}

	indexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();
}