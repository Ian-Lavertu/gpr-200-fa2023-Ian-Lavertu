#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPosition, WorldNormal;
}vs_out;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

struct Light {
	vec3 position, color;
};

#define MAX_LIGHTS 4

uniform Light _Lights[MAX_LIGHTS];

void main(){
	vs_out.UV = vUV;
	vs_out.WorldPosition = vec3(_Model * vec4(vPos, 1));
	vs_out.WorldNormal = vec3(_Model * vec4(vNormal, 0));

	gl_Position = _ViewProjection * _Model * vec4(vPos, 1);
}