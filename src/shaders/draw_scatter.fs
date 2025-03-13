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

uniform sampler2D scatterTexture;
uniform vec2 windowSize;
uniform Material material;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
		vec4 LightSpacePos;
} fs_in;



void main()
{
	vec3 ambient = 0.1 * material.albedo;
	vec3 scatterContribution = texture(scatterTexture, gl_FragCoord.xy / windowSize).rgb;	

	FragColor = vec4(ambient + scatterContribution, 1.0);
}
