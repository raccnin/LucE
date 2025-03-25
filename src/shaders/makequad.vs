#version 330

vec3 offsets[6] = vec3[6](
	vec3(-1, -1, 0),
	vec3( 1, -1, 0),
	vec3( 1,  1, 0),
	vec3( 1,  1, 0),
	vec3(-1,  1, 0),
	vec3(-1, -1, 0)
);

void main()
{
	gl_Position = vec4(vec3(0.5, 0.5, 1) + offsets[gl_VertexID], 1.0);
}
