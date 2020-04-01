#version 330

layout (location = 0) in vec3 pos;				//position of the point on our skybox

out vec3 TexCoords;				//the texture coordinates will be the same as the position

uniform mat4 projection;
uniform mat4 view;

void main()
{
	TexCoords = pos;
	gl_Position = projection * view * vec4(pos, 1.0);		//projection * view gives the position of the object relative to it's origin...Here the model is the skybox that is being used

}