#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 RedColor;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

		vec3 direction;
		float innerCutoff;
		float outerCutoff;
};

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform vec3 viewPos;
uniform Light light;
uniform Material material;
uniform sampler2D shadowMap;
uniform sampler2D thicknessMap;


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
		vec4 LightSpacePos;
} fs_in;

float calculateShadow(vec4 lightSpacePos, float bias)
{
	// perspective divide (/w)
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	// to NDC [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float shadow = projCoords.z - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}

float attenuate(vec3 toLight)
{
	float d = length(toLight);
	return 1.0 / (1.0 + d * d); // cubic attenuation
}

void main()
{
	vec3 albedo = vec3(0.5);
	RedColor = vec4(albedo * vec3(1.0, 0.0, 0.0), 1.0);
	FragColor = vec4(albedo, 1.0);
}

