#version 330 core

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

		vec3 direction;
		float cutoff;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

uniform vec3 viewPos;
uniform Light light;

out vec4 FragColor;

// constants
const float PI = 3.14159;

// material
vec3 albedo = vec3(0.5);
float metallic = 0.0;
float roughness = 0.5;
float ao = 0.1;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main(){
	vec3 N = normalize(fs_in.Normal);
	vec3 V = normalize(viewPos - fs_in.FragPos);
	
	vec3 Lo = vec3(0.0); // outgoing light

	// light radiance
	vec3 L = normalize(light.position - fs_in.FragPos);
	vec3 H = normalize(V + L);

	float distance = length(light.position - fs_in.FragPos);
	float attenuation = 1/(distance * distance);
	vec3 radiance = light.diffuse * attenuation;
	
	// cook-torrance specular term
	// 1. calculate ratio of specular:diffuse (Fresnel)
	// 2. calculate Normal Distribution
	// 3. calculate Geometry obstruction
	// 4. put together in fraction
	vec3 F0 = vec3(0.04); // assumption for all dielectrics (non-metals)
	F0 = mix(F0, albedo, metallic); // mixes F0 with albedo, weighted by the metallic value
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	// weight specular proportion F by NDF and G
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001); // floor to prevent 0-division

	// calculate contribution to resulting reflectance equation
	// energy preservation happens here
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	
	kD *= 1.0 - metallic; // nullifying if material is metal

	// calculate final reflectance equation
	float NdotL =  max(dot(N, L), 0.0);
	Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	// add (currently incorrect) ambient value
	vec3 ambient = vec3(0.03) * albedo * ao;
	Lo += ambient;

	FragColor = vec4(Lo, 1.0); 
}

// determines ratio of reflected/refracted light
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// approximates percentage of reflects aligned with the viewer
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

// approximates amount of light lost by micro geometry obstruction
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
