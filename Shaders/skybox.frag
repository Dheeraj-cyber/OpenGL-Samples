#version 330
 
in vec3 TexCoords;
 
out vec4 colour;
 
uniform samplerCube skyBox;
 
void main()
{
	colour = texture(skyBox, TexCoords);			//when we're drawing our cube, we are getting the point on the skybox and texture co-ordinates
}