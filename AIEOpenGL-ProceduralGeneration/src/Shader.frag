#version 410

in vec4 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec3 LightDir;
uniform vec3 LightColour;

void main()	
{
	//float d = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
	//FragColor = vec4(LightColour * d,1);
	FragColor = texture(diffuse,vTexCoord).rrrr;
	FragColor.a = 1;
}