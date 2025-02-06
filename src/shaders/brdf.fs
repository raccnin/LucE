#version 330 core

vec3 albedo = vec3(1.0);
float metallic = 0.0;
float roughness = 1.0;
float ao = ;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoord;
} fs_in;

uniform vec3 camPos;

void main()
{
	vec3 N = normalize(fs_in.Normal);
	vec3 V = normalize(camPos - fs_in.FragPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
}
