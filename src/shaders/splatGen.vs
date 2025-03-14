#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform sampler2D splatPositions;
uniform sampler2D splatNormals;
uniform int splatResolution;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

out VS_OUT {
	flat bool inVoid;
	vec3 FragPos;
	flat vec3 splatCenter;
	flat vec3 splatNormal;
} vs_out;

void main()
{
	vec2 splatIndex = vec2(gl_InstanceID % splatResolution, int(gl_InstanceID / splatResolution));
	vec4 worldPosition = texture(splatPositions, splatIndex / splatResolution);
	vec3 worldNormal = texture(splatNormals, splatIndex / splatResolution).rgb;

	vs_out.splatCenter = worldPosition.xyz;
	vs_out.splatNormal = normalize(worldNormal);

	vs_out.FragPos = aPos + worldPosition.xyz;
	// prevent n quads at 0,0,0 if in void
	vs_out.inVoid = worldPosition.w > 0.0 ? true : false;

	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

