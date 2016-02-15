#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoord;


out vec2 vTexCoord;
out vec4 vNormal;

uniform mat4 ProjectionView;
uniform float time;
uniform float heightScale;

void main()
{ 
	vNormal = Normal;
	vTexCoord = TexCoord; 
	//vec4 P = Position; P.y += sin(time + Position.x) * sin(time + Position.z) * heightScale; 
	//gl_Position = ProjectionView * P;
	gl_Position = ProjectionView * Position;
}