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
    vec4 diffuse_sample = texture(material.texture_diffuse1, fs_in.TexCoord);
    vec4 specular_sample = texture(material.texture_specular1, fs_in.TexCoord);
    // ambient
    vec3 ambient = diffuse_sample.xyz * light.ambient;

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * diffuse_sample.xyz);

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    vec3 specular = light.specular * (spec * specular_sample.xyz);  

    vec3 phongResult = ambient + diff + specular;
    FragColor = vec4(phongResult, 1.0);
}