#version 330 core

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 LightSpacePos;
} fs_in;

out vec4 FragColor;

uniform Light light;

void main()
{
	FragColor = vec4(light.specular, 1.0);
}
