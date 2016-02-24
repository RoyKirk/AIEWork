#version 410

in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D specular;
uniform vec3 LightDir;
uniform vec3 CameraPos;
uniform float SpecPow;

void main()	
{
	vec3 E = normalize(CameraPos - vPosition.xyz);
	vec3 R = reflect(-LightDir, vNormal.xyz);
	R = normalize(R);
	float s = max(0,dot(E,R));
	float Spec = texture(specular,vTexCoord).x * SpecPow;
	s = pow(s, Spec);

	mat3 TBN = mat3(normalize(vTangent), normalize(vBiTangent), normalize(vNormal));
	vec3 N = texture(normal, vTexCoord).xyz * 2 - 1;
	float d = max(0,dot(normalize(TBN*N),normalize(LightDir)));
	//FragColor = vec4(0.8,0.8,1,1);
	FragColor = texture(diffuse,vTexCoord);
	FragColor.rgb = FragColor.rgb * d + FragColor.rgb * s;
}