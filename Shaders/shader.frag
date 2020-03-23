#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;

struct Light {
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct Material 
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform sampler2D theTexture;
uniform Material material;

uniform vec3 eyePosition;

vec4 CalcLightByDirection(Light light, vec3 direction)
{
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;
	
	/*Here, A.B = |A||B|cos(angle) = 1*1*cos(angle) ===>  A.B = cos(angle)  */
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);   /*normalize means converting it to a unit vector*/
	vec4 diffuseColour = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);
	
	vec4 specularColour = vec4(0, 0, 0, 0);				/*the 0's specify that we have no specular colour*/
	
	if(diffuseFactor > 0.0f)			/*Checks if our diffuse factor is greater than 0*/ 
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);			/*We want to get a 0 vertex between eyePosition and FragPos*/
		vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));  	/*Find where the light ray is reflected around the normal. Here the first arguement is what we want to reflect and the second arguement is what we are reflecting it around*/
		
		float specularFactor = dot(fragToEye, reflectedVertex);		/*dot product of two unit  vectors will give us the cosine of the angle between them.*/ 
		if(specularFactor > 0.0f)			/*if we have specular lighting to apply*/
		{
			specularFactor = pow(specularFactor, material.shininess);			/*returns specularFactor^material.shininess*/
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);			/*material.specularIntensity specifies how much specular light to apply. specularFactor specifies how powerful that light is or the shininess */ 
		}
	}
	
	return (ambientColour + diffuseColour + specularColour);
}

vec4 CalcDirectionalLight()
{
	return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLight()
{
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++)
	{
		vec3 direction = FragPos - pointLights[i].position;			/*Gets the vector from the point light to our fragment*/
		float distance = length(direction);
		direction = normalize(direction);
		
		vec4 colour = CalcLightByDirection(pointLights[i].base, direction);
		float attenuation = pointLights[i].exponent * distance * distance + 
							pointLights[i].linear * distance + 
							pointLights[i].constant;				/*ax^2 + bx + c*/
							
		totalColour += (colour / attenuation);
	}
	
	return totalColour;
}

void main()
{
	vec4 finalColour = CalcDirectionalLight();
	finalColour += CalcPointLight();
	
	colour = texture(theTexture, TexCoord) * finalColour;
}


