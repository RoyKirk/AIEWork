#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec2 TexCoord;
layout(location=2) in vec4 Normal;

out vec2 vTexCoord;
out vec4 vNormal;
out vec4 vPosition;

uniform mat4 ProjectionView;
uniform sampler2D m_texture;
uniform unsigned int dimension;



void main()
{ 
	//const vec2 size = vec2(2.0,0.0);
	//const ivec3 off = ivec3(-1,0,1);
	vec4 pos = Position;
	vTexCoord = vec2(TexCoord.y,TexCoord.x); 
	//pos.y = texture(m_texture,vTexCoord).r * 50;
	vNormal = Normal;
	vPosition = Position;
	//pos.y = 0;
	gl_Position = ProjectionView * pos;
}



