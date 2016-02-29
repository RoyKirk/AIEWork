#version 410

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform sampler2D ground_textureD;
uniform sampler2D ground_textureN;
uniform vec3 LightDir;
uniform vec3 LightColour;

void main()	
{

	vec3 N = texture(ground_textureN, vTexCoord).xyz * 2 - 1;
	float d1 = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
	float d2 = max(0, dot(normalize(N), normalize(LightDir)));
	if(vPosition.y < 40)
	{
		FragColor = texture(ground_textureD,vTexCoord)*0.6;
		FragColor += vec4(LightColour * d1 * d2,1);
	}
	else if(vPosition.y > 40)
	{
		FragColor = vec4(0.7,0.7,0.8,1)*0.6;
		FragColor += vec4(LightColour * d1 ,1);
	}

	
	//FragColor += texture(diffuse,vTexCoord).rrrr;
	//FragColor.a = 1;
}