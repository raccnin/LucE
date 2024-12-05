#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;
uniform vec3 viewPos;
uniform Light light;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

void main()
{
	vec3 fragPos = vec3(model * vec4(aPos, 1.0))
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = aNormal;
	vs_out.TexCoord = aTexCoord;

	// tangent space mapping
	// 1. translate tangent basisinto world space
	// 2. make inverse change of basis matrix
	// 3. transform the important vectors (viewPos and lightPos)
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(vec3(normalMatrix * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(normalMatrix * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(normalMatrix * vec4(aNormal, 0.0))); 
	// tranposing basis vectors is equivalent to inverting, but cheaper!
	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentLightPos = (TBN * light.position);
	vs_out.TangentViewPos = (TBN * viewPos);
	vs_out.TangentFragPos = (TBN * vs_out.FragPos));

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
