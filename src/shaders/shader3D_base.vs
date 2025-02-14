#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;
uniform mat4 lightTransform;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
		vec4 LightSpacePos;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = aNormal;
    vs_out.TexCoord = aTexCoord;
		vs_out.LightSpacePos = lightTransform *  model * vec4(aPos, 1.0); // visualising light transform

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

