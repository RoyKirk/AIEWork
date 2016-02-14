#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Colour;

out vec4 vColour;

uniform mat4 ProjectionView;
uniform float time;
uniform float heightScale;

void main()
{ 
	vec4 C = Colour; 
	C.x += sin(time + Position.x) * sin(time + Position.z)*0.3; 
	C.y += sin(time + Position.x) * sin(time + Position.z)*0.4; 
	C.z += sin(time + Position.x) * sin(time + Position.z)*0.5; 
	vColour = C; 
	vec4 P = Position; P.y += sin(time + Position.x) * sin(time + Position.z) * heightScale; 
	gl_Position = ProjectionView * P;
	//vColour = Colour; 
	//gl_Position = ProjectionView * Position;
}