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
	vec3 N = (texture(ground_textureN,vTexCoord)*(1-texture(diffuse,vTexCoord).rrrr+0.3) + texture(snow_textureN,vTexCoord)*(texture(diffuse,vTexCoord).rrrr-0.3)).xyz * 2 - 1;
	float d1 = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
	float d2 = max(0, dot(normalize(N), normalize(LightDir)));	
	FragColor = 0.4*(texture(ground_textureD,vTexCoord)*(1-texture(diffuse,vTexCoord).rrrr+0.3) + texture(snow_textureD,vTexCoord)*(texture(diffuse,vTexCoord).rrrr-0.3));
	FragColor += 0.6*vec4(LightColour * d1,1);
	FragColor += 0.2*vec4(LightColour * d2,1);
	//FragColor = texture(diffuse,vTexCoord).rrrr;
	FragColor.a = 1;

}