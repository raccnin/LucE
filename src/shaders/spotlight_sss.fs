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

float attenuate(vec3 toLight)
{
	float d = length(toLight);
	return 1.0 / (1.0 + 1.0 * d + 1.0 * d * d); // cubic attenuation
}

void main()
{
	vec3 albedo = vec3(0.5, 0.5, 0.8);
	float thickness = texture(thicknessMap, fs_in.TexCoord).r;

	vec3 L = light.position - fs_in.FragPos;
	float attenuation = attenuate(L);
	L = normalize(L);
	vec3 N = normalize(fs_in.Normal);
	vec3 V = normalize(viewPos - fs_in.FragPos);
	float distortion = 5.0f;
	float LPower = 20.0f;
	float LTScale = 5.0;
	float LTAmbient = 0.1;

	vec3 spotDirection = normalize(light.direction);
	float theta = dot(L, -spotDirection); // cosine of angle

	// translucency approximation
	vec3 LTLight = L + N * distortion;
	float LTDot = pow(clamp(dot(V, -LTLight), 0.0, 1.0), LPower) * LTScale;
	float LT = attenuation * (LTDot + LTAmbient) * thickness; 
	vec3 diffuse = albedo * light.diffuse * LT;


	if (theta > light.outerCutoff)
	{
		/*
		// ambient
		vec3 ambient = light.ambient * albedo;

		// diffuse
		float diff = max(dot(N, L), 0.0);
		diffuse = light.diffuse * (diff * albedo);
		*/

		// specular
		vec3 H = normalize(L + V);
		float spec = pow(max(dot(N, H), 0.0), 64.0);
		vec3 specular = light.specular * (spec * albedo);

		// smooth edges
		float epsilon = light.innerCutoff - light.outerCutoff;
		float intensity = (theta - light.outerCutoff) / epsilon;
		specular *= intensity;

		float bias = max(0.0005 * (1.0 - dot(N, L)), 0.00005);
		float shadow = calculateShadow(fs_in.LightSpacePos, bias);
		specular = (1.0 - shadow) * specular;

		FragColor = vec4(specular + diffuse, 1.0);
	}
	else
	{
			FragColor = vec4(diffuse, 1.0);
	}
}

