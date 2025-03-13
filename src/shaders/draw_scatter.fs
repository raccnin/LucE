#version 330

out vec4 FragColor;

uniform sampler2D scatterTexture;
uniform vec2 windowSize;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
		vec4 LightSpacePos;
} fs_in;

void main()
{
	FragColor = texture(scatterTexture, gl_FragCoord.xy / windowSize);	
}
