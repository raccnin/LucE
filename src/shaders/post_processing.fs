#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D frameTexture;

void main()
{
    vec3 sample = texture(frameTexture, TexCoord).rgb;
    float gamma = 2.2;
    sample = pow(sample, vec3(1/gamma));
    FragColor = vec4(sample, 1.0);
} 