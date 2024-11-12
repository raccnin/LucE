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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out vec4 FragColor;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

void main()
{
    // ambient
    vec3 ambient = material.diffuse * light.ambient;

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  

    vec3 phongResult = ambient + diff + specular;
    float gamma = 2.2;
    FragColor.rgb = pow(phongResult, vec3(1.0/gamma));
    FragColor.a = 1.0;
    
}