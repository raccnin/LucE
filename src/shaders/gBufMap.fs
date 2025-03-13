#version 330

layout (location = 0) out vec3 worldPos;
layout (location = 1) out vec3 normal;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
		vec4 LightSpacePos;
} fs_in;


void main()
{
	worldPos = fs_in.FragPos;
	normal = normalize(fs_in.Normal);
}
