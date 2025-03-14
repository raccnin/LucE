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

const float GAMMA = 2.2;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
		vec4 LightSpacePos;
} fs_in;



void main()
{
	vec3 ambient = 0.01 * material.albedo;
	vec3 scatterContribution = texture(scatterTexture, gl_FragCoord.xy / windowSize).rgb;	


	vec3 HDRColour = ambient + scatterContribution;
	// reinahrd tone mapping
	vec3 outColour = HDRColour / (HDRColour + vec3(1.0)); 
	// gamma correction
	outColour = pow(outColour, vec3(1.0 / GAMMA));
	FragColor = vec4(outColour, 1.0);
}
