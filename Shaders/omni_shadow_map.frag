#version 330

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float distance = length(FragPos.xyz - lightPos);			//gets the distance between the fragment and the light position
	distance = distance/farPlane;
	gl_FragDepth = distance;				//overriding the distance with the depth buffer
	
}