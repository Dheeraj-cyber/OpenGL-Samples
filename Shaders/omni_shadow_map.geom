#version 330

layout (triangles) in;				//we can even have lines, triangles, points, etc
layout (triangle_strip, max_vertices = 18) out;				//max_vertices defines the amount of vertices that we want from the geometry shader

uniform mat4 lightMatrices[6];

out vec4 FragPos;

void main()
{
	for(int face = 0; face < 6; face++)		//go through each side of the cube and render each one
	{
		gl_Layer = face;				//which face we are rendering to our cubes
		for(int i = 0; i < 3; i++)					//go through each of the vertices of the triangle that we pass
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = lightMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();			//We have finished drawing our triangle string so that we can go to another one
	}
}