#version 330

out vec4 FragColor;

uniform vec3 aColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

void main()
{
    FragColor = vec4(aColor, 1.0);
}