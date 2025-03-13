#version 330

out vec4 FragColor;

uniform sampler2D splatPositions;
uniform sampler2D splatNormals;
uniform sampler2D viewPositions;

in VS_OUT {
	vec3 FragPos;
} fs_in;

void main()
{
	FragColor = vec4(0.2);
}
