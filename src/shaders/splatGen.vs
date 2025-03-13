#version 330

layout (location = 0) in vec3 aPos;

uniform sampler2D splatPositions;
uniform int splatResolution;
uniform mat4 model;
// vector perpendicular to view vector
uniform vec3 viewUp;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

out VS_OUT {
	vec3 FragPos;
	vec3 splatCenter;
} vs_out;

void main()
{
	vec2 splatIndex = vec2(gl_InstanceID % splatResolution, int(gl_InstanceID / splatResolution));
	vec3 worldPosition = texture2D(splatPositions, splatIndex / splatResolution).rgb;

	vs_out.splatCenter = worldPosition;
	vs_out.FragPos = aPos + worldPosition;
	gl_Position = projection * view * vec4(aPos + worldPosition, 1.0);
}

