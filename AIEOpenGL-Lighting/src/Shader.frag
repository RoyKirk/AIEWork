#version 410

in vec2 vTexCoord;
in vec4 vNormal;

out vec4 FragColor;

uniform sampler2D diffuse;

void main()	
{
	float d = max(0, dot(normalize(vNormal.xyz), vec3(0,1,0)));
	FragColor = vec4(d,d,d,1);
}