#version 410

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec3 LightDir;
uniform vec3 CameraPos;
uniform float SpecPow;

void main()	
{
	FragColor = texture(diffuse,vTexCoord).rrrr;
	FragColor.a = 1;
}