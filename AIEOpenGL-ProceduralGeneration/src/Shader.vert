#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec2 TexCoord;
layout(location=2) in vec4 Normal;

out vec2 vTexCoord;
out vec4 vNormal;

uniform mat4 ProjectionView;
uniform sampler2D m_texture;
uniform unsigned int dimension;



void main()
{ 
	//const vec2 size = vec2(2.0,0.0);
	//const ivec3 off = ivec3(-1,0,1);
	vec4 pos = Position;
	pos.y += texture(m_texture,TexCoord).r * 50;
	vTexCoord = TexCoord; 
	vNormal = Normal;
	gl_Position = ProjectionView * pos;
}



