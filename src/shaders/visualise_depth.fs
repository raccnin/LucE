#version 330 core

in vec2 TexCoord;
uniform sampler2D frameTexture;

out vec4 FragColor;

const float near_plane = 0.1;
const float far_plane = 1000.0;

float LinearizeDepth(float depth) // for perspective spotlight
{
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
	float depthValue = texture(frameTexture, TexCoord).r;
	depthValue = LinearizeDepth(depthValue) / far_plane;
	FragColor = vec4(vec3(depthValue), 1.0);
}
