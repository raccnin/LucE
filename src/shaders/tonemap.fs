#version 330 core

in vec2 TexCoord;
uniform sampler2D frameTexture;

out vec4 FragColor;

vec3 tonemap(vec3 HDR_sample, float exposure);

void main()
{
	vec3 sample = texture(frameTexture, TexCoord).rgb;
	sample = tonemap(sample, 1.0);
	float gamma = 2.2;
	sample = pow(sample,vec3(1/gamma));
	FragColor = vec4(sample, 1.0);
}

// reinahrd tonemapping
vec3 tonemap(vec3 HDR_sample, float exposure)
{
	return vec3(1.0) - exp(-HDR_sample * exposure);
}
