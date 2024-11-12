#version 330 core
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

out vec4 FragColor;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

void main()
{
    //FragColor = vec4(0.0, 0.5, 0.5, 1.0);
    FragColor = texture(material.texture_diffuse1, fs_in.TexCoord);
}