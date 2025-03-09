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
uniform sampler2D shadowMap;
uniform sampler2D thicknessMap;


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
		vec4 LightSpacePos;
} fs_in;

float calculateShadow(vec4 lightSpacePos, float bias)
{
	// perspective divide (/w)
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	// to NDC [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float shadow = projCoords.z - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	vec3 albedo = vec3(0.5, 0.5, 0.5);
	//vec3 albedo = texture(material.texture_diffuse1, fs_in.TexCoord).rgb;

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
		float spec = pow(max(dot(N, H), 0.0), 64.0);
		vec3 specular = light.specular * (spec * albedo);

		// smooth edges
		float epsilon = light.innerCutoff - light.outerCutoff;
		float intensity = (theta - light.outerCutoff) / epsilon;

		diffuse *= intensity;
		specular *= intensity;

		float bias = max(0.0005 * (1.0 - dot(N, L)), 0.00005);
		float shadow = calculateShadow(fs_in.LightSpacePos, bias);
		vec3 phongResult = ambient + ((1.0 - shadow) * (specular + diffuse));

		FragColor = vec4(phongResult, 1.0);
	}
	else
	{
			FragColor = vec4(albedo * light.ambient, 1.0);
	}
}

