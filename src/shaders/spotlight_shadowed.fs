#version 330 core
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

		vec3 direction;
		float innerCutoff;
		float outerCutoff;
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
	vec3 albedo = vec3(0.5, 1.0, 0.5);

	vec3 L = normalize(light.position - fs_in.FragPos);
	vec3 spotDirection = normalize(light.direction);
	float theta = dot(L, -spotDirection); // cosine of angle

	if (theta > light.outerCutoff)
	{
		// ambient
		vec3 ambient = light.ambient * albedo;

		// diffuse
		vec3 N = normalize(fs_in.Normal);
		float diff = max(dot(N, L), 0.0);
		vec3 diffuse = light.diffuse * (diff * albedo);

		// specular
		vec3 V = normalize(viewPos - fs_in.FragPos);
		vec3 H = normalize(L + V);
		float spec = pow(max(dot(N, H), 0.0), 32.0);
		vec3 specular = light.specular * (spec * albedo);

		// smooth edges
		float epsilon = light.innerCutoff - light.outerCutoff;
		float intensity = (theta - light.outerCutoff) / epsilon;

		diffuse *= intensity;
		specular *= intensity;


		vec3 phongResult = ambient + diffuse + specular;

		FragColor = vec4(phongResult, 1.0);
	}
	else
	{
			FragColor = vec4(albedo * light.ambient, 1.0);
	}
}

