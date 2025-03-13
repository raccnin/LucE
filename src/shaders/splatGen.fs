#version 330

struct Material
{
	vec3 albedo;

	float scattering;
	float absorption;
	float meanCosine;
	float rri;
};

struct SpotLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
	float innerCutoff;
	float outerCutoff;
};

out vec4 FragColor;

uniform sampler2D splatPositions;
uniform sampler2D splatNormals;
uniform sampler2D viewPositions;
uniform int maxDistance;
uniform Material material;

const float PI = 3.141;

in VS_OUT {
	vec3 FragPos;
	vec3 splatCenter;
} fs_in;

// multi-scatter = 1/PI * (Ft(rri, L) * R_d(r) * Ft(rri, V))
// single-scatter much more complex!

void main()
{
	FragColor = vec4(material.albedo, 1.0);
}
