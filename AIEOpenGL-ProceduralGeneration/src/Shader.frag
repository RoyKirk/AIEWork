#version 410

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform sampler2D ground_textureD;
uniform sampler2D ground_textureN;
uniform sampler2D snow_textureD;
uniform sampler2D snow_textureN;
uniform vec3 LightDir;
uniform vec3 LightColour;

void main()	
{
	if(vPosition.y < 49.9)
	{
		vec3 N = texture(ground_textureN, vTexCoord).xyz * 2 - 1;
		float d1 = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
		float d2 = max(0, dot(normalize(N), normalize(LightDir)));
		FragColor = texture(ground_textureD,vTexCoord)*0.5;
		FragColor += vec4(LightColour * d1 * d2,1);
	}
	else if(vPosition.y > 50.1)
	{
		vec3 N = texture(snow_textureN, vTexCoord).xyz * 2 - 1;
		float d1 = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
		float d2 = max(0, dot(normalize(N), normalize(LightDir)));
		FragColor = texture(snow_textureD,vTexCoord/5)*0.5;
		FragColor += vec4(LightColour * d1 * d2,1);
	}
	else
	{
		vec3 N1 = texture(snow_textureN, vTexCoord).xyz * 2 - 1;
		float d1 = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
		float d2 = max(0, dot(normalize(N1), normalize(LightDir)));
		FragColor = texture(snow_textureD,vTexCoord/5)*0.5;
		vec3 N2 = texture(ground_textureN, vTexCoord).xyz * 2 - 1;
		float d3 = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
		float d4 = max(0, dot(normalize(N2), normalize(LightDir)));
		FragColor = texture(ground_textureD,vTexCoord)*0.6 + texture(snow_textureD,vTexCoord/5)*0.15;
		FragColor += vec4(LightColour * (d1 * d2 * 0.5) * (d3 * d4 * 0.5),1);
	}

	
	FragColor = texture(diffuse,vTexCoord).rrrr;
	FragColor.a = 1;
}