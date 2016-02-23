#version 410

in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec3 LightDir;
uniform vec3 LightColour;
uniform vec3 CameraPos;
uniform float SpecPow;

void main()	
{
	float d = max(0, dot(normalize(vNormal.xyz), normalize(LightDir)));
	vec3 E = normalize(CameraPos - vPosition.xyz);
	vec3 R = reflect(-LightDir, vNormal.xyz);
	R = normalize(R);
	float s = max(0,dot(E,R));
	s = pow(s, SpecPow);
	FragColor = vec4(LightColour * d + LightColour * s,1) + texture(diffuse,vTexCoord);
	//FragColor = texture(diffuse,vTexCoord);
	//float n = max(0, dot(normalize(vNormal.xyz), normalize(vec3(0,1,0))));
	//FragColor = vec4(n,n,n,1) + vec4(LightColour * d + LightColour * s,1);
	//FragColor = vec4(d,d,d,1) + (vNormal*0.2);
}