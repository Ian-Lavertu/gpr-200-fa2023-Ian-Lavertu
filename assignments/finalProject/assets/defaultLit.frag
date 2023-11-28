#version 450
#version 450
#define MAX_LIGHTS 4

out vec4 FragColor;

in Surface {
	vec2 UV; 
	vec3 WorldPosition, WorldNormal; 
}fs_in;

struct Light {
	vec3 position, color;
};

uniform sampler2D _Texture;
uniform vec3 _LightColor, _CameraPos;

uniform float _Ambient, _Diffuse, _Specular, _Shininess;
uniform int _NumOfLights;

uniform Light _Lights[MAX_LIGHTS];

void main(){
	vec3 worldNormal = normalize(fs_in.WorldNormal);
	vec3 cameraDirection = normalize(_CameraPos - fs_in.WorldPosition);
	
	vec3 allLightColors;

	vec3 ambientLight = _LightColor * _Ambient;

	for(int i = 0; i < _NumOfLights; i++) {\
		vec3 direction = normalize(_Lights[i].position - fs_in.WorldPosition);

		float dIntensity = _Diffuse * max(dot(worldNormal, direction), 0);
		vec3 diffuseLight = _Lights[i].color * dIntensity;

		vec3 reflection = reflect(-direction, worldNormal);
		float sIntensity = _Specular * pow(max(dot(reflection, cameraDirection), 0), _Shininess);

		vec3 specularLight = _Lights[i].color * sIntensity;

		allLightColors += diffuseLight + specularLight;
	}

	vec3 finalColor = ambientLight + allLightColors;

	vec4 textureColor = texture(_Texture, fs_in.UV);

	FragColor = vec4(textureColor.xyz * finalColor, textureColor.w);
}