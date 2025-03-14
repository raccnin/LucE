#version 330

struct Material
{
	vec3 albedo;

	float scattering;
	float absorption;
	float meanCosine;
	float rri;
};

struct SpotLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
	float innerCutoff;
	float outerCutoff;
};

out vec4 FragColor;

uniform vec2 windowSize;
uniform vec3 viewPos;

uniform sampler2D viewPositions;
uniform sampler2D dipoleLookup;
uniform int maxDistance;

uniform Material material;
uniform SpotLight light;

const float PI = 3.141;

in VS_OUT {
	flat bool inVoid;
	vec3 FragPos;
	vec3 Normal;
	flat vec3 splatCenter;
	flat vec3 splatNormal;
} fs_in;

// light out = S(splatCenter, toLight, FragPos, toViewer)*L(splatCenter, toLight) * dot(splatNormal, toLight)
// S = single_scatter + multiscatter
// L = light attenuation

// rri is ratio of speed of light in the mediums it is travelling from and into
// where n_21  = v_1/v_2 (travelling from medium 1 to medium 2)

vec3 radiance(SpotLight light, vec3 position)
{
	float d = distance(light.position, position);
	float attenuation = 1 / (d*d);
	return light.diffuse * attenuation;
}

float fresnelTransmittance(float rri, vec3 theta)
{
	// normal incidence
	// otherwise

	return 0.5;
}

float multiScatter(vec3 x_i, vec3 w_i, vec3 x_o, vec3 w_o)
{
	float r = distance(x_o, x_i);
	// dipoleLookup[i] == R_d(i/10)
	// if wanting to get R_d at r, dipoleLookup[r*10.0]
	if (r > maxDistance / 10.0)
	{
		return 0.0;
	}
	else
	{
		int dipoleIndex = int(r * 10); 
		float R_d = texture(dipoleLookup, vec2(dipoleIndex, 0) / maxDistance).r;
		float result = fresnelTransmittance(material.rri, w_i) * R_d * fresnelTransmittance(material.rri, w_o);
		return (1/PI) * result;
	}
}

// TODO:
float singleScatter()
{
	return 0.0;
}

void main()
{
	if (fs_in.inVoid) discard;	

	vec3 outPoint = texture(viewPositions, gl_FragCoord.xy / windowSize).rgb;
	vec3 outDirection = normalize(viewPos - outPoint); 
	vec3 inPoint = fs_in.splatCenter;
	vec3 inDirection = normalize(inPoint - light.position);
	vec3 normal = normalize(fs_in.splatNormal);
	vec3 splatPoint = fs_in.FragPos;

	float scatter = multiScatter(inPoint, inDirection, outPoint, outDirection) + singleScatter();
	vec3 radiance = radiance(light, inPoint);
	float dotProd = dot(normal, -inDirection);
	vec3 outColour = scatter * radiance * dotProd;
	FragColor = vec4(outColour, 1.0);
}
