#version 330 core

in vec2 TexCoord;
uniform sampler2D depthTexture;

out vec4 FragColor;

void main()
{
	float depthValue = texture(depthTexture, TexCoord).r;
	FragColor = vec4(vec3(depthValue), 1.0);
}
