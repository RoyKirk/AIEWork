#version 410


in vec4 vNormal;
in vec2 vTexCoord;


out vec4 FragColor;

uniform sampler2D diffuse;

void main()	
{
	float d = max(0, dot(normalize(vNormal.xyz), vec3(0,0.5,0)));
	//FragColor = vec4(d,d,d,1);
	FragColor = vec4(d,d,d,1) + (vNormal*0.2);
}