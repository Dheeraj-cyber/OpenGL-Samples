#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

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

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct OmniShadowMap
{	//This structure stores information about our shadows
	samplerCube shadowMap;
	float farPlane;			//this specifies how far our omnishadow map reaches in any given plane
};

struct Material 
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

//this covers all the key directions
vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	
);

float CalcDirectionalShadowFactor(DirectionalLight light)
{
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5;
	
	//float closest = texture(directionalShadowMap, projCoords.xy).r;
	float current = projCoords.z;			//just says how far the shadow is from the light
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.direction);
	
	float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.005);
	
	float shadow = 0.0;
	
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);			//textureSize(directionalShadowMap, 0) - tells us how big our texture is. 0 - mipmap level. By doing this, we'll be defining one unit of our textureSize
	//1.0 specifies how big one unit of the texture is.
	for(int x = -1; x <= 1 ; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2( x, y) * texelSize).r; 
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
			
	shadow /= 9.0;
	
	//float shadow = current - bias > closest ? 1.0 : 0.0;		//is the shadow is the currentDepth	which is furthur away from the closestDepth, then we know that there is something in the way
	
	if(projCoords.z > 1)
	{
		shadow = 0.0;
	}
	
	return shadow;
}

float CalcOmniShadowFactor(PointLight light, int shadowIndex)
{
	vec3 fragToLight = FragPos - light.position;			//we can get the depth from this vector
	float current = length(fragToLight);
	
	float shadow = 0.0;
	float bias = 0.05;
	int samples = 20;
	
	float viewDistance = length(eyePosition - FragPos);
	float diskRadius = (1.0 + (viewDistance/omniShadowMaps[shadowIndex].farPlane)) / 25.0; 
	
	for(int i = 0; i < samples; i++)
	{
		float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closest *= omniShadowMaps[shadowIndex].farPlane;
		if(current - bias > closest )
		{
			shadow += 1.0;
		}
	}
	
	
	shadow /= float(samples);
	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
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
	
	return (ambientColour + (1.0 - shadowFactor) * (diffuseColour + specularColour));
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
		vec3 direction = FragPos - pLight.position;			/*Gets the vector from the point light to our fragment*/
		float distance = length(direction);
		direction = normalize(direction);
		
		float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex);
		
		vec4 colour = CalcLightByDirection(pLight.base, direction, shadowFactor);
		float attenuation = pLight.exponent * distance * distance + 
							pLight.linear * distance + 
							pLight.constant;				/*ax^2 + bx + c*/
							
		return (colour / attenuation);
}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)			//calculates the light for a single spotlight
{
	vec3 rayDirection = normalize(FragPos - sLight.base.position);			//vector between where we are and the fragment we are currently using the calculation for
	float slFactor = dot(rayDirection, sLight.direction);			//this decides whether to light the fragment or not. rayDirection is from where we are to the point being illuminated. sLight.direction is the direction that our light is facing.
																	//the dot product gets the angle between the two and if the angle is smaller, then wwe know that the point lies within the sort of cone of our spotlight
																	
	if (slFactor > sLight.edge)
	{
		vec4 colour = CalcPointLight(sLight.base, shadowIndex);
		
		return colour * (1.0f - (1.0f - slFactor)*(1.0f/(1.0f - sLight.edge)));				//Gets the ratio between the two scales 
	} else {
		return vec4(0, 0, 0, 0);				//No light
	}
}

vec4 CalcPointLight()
{
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++)
	{
		totalColour += CalcPointLight(pointLights[i], i);
	}
	
	return totalColour;
}

vec4 CalcSpotLight()
{
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < spotLightCount; i++)
	{
		totalColour += CalcSpotLight(spotLights[i], i + pointLightCount);
	}
	
	return totalColour;
}
void main()
{
	vec4 finalColour = CalcDirectionalLight();
	finalColour += CalcPointLight();
	finalColour += CalcSpotLight();
	
	colour = texture(theTexture, TexCoord) * finalColour;
}


