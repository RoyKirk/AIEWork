#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec2 TexCoord;

out vec2 vTexCoord;

uniform mat4 ProjectionView;
uniform sampler2D m_texture;


void main()
{ 
	vec4 pos = Position;
	pos.y += texture(m_texture,TexCoord).r * 5;
	vTexCoord = TexCoord; 


	gl_Position = ProjectionView * pos;
}