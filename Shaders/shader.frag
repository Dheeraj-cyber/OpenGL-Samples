#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

struct DirectionalLight 
{
	vec3 colour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

struct Material 
{
	float specularIntensity;
	float shininess;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform vec3 eyePosition;

void main()
{
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;
	
	/*Here, A.B = |A||B|cos(angle) = 1*1*cos(angle) ===>  A.B = cos(angle)  */
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);   /*normalize means converting it to a unit vector*/
	vec4 diffuseColour = vec4(directionalLight.colour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;
	
	vec4 specularColour = vec4(0, 0, 0, 0);				//the 0's specify that we have no specular colour
	
	if(diffuseFactor > 0.0f)			//Checks if our diffuse factor is greater than 0 
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);			//We want to get a 0vertex between eyePosition and FragPos
		vec3 reflectedVertex = normalize(reflect(directionalLight.direction, normalize(Normal)));  	//Find where the light ray is reflected around the normal. Here the first arguement is what we want to reflect and the second arguement is what we are reflecting it around
		
		float specularFactor = dot(fragToEye, reflectedVertex);		//dot product of two unit  vectors will give us the cosine of the angle between them. 
		if(specularFactor > 0.0f)			//if we have specular lighting to apply
		{
			specularFactor = pow(specularFactor, material.shininess);			//returns specularFactor^material.shininess
			specularColour = vec4(directionalLight.colour * material.specularIntensity * specularFactor, 1.0f);			//material.specularIntensity specifies how much specular light to apply. specularFactor specifies how powerful that light is or the shininess 
		}
	}

	colour = texture(theTexture, TexCoord) * (ambientColour + diffuseColour + specularColour);
}


