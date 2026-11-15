#version 330 core

in vec3 out_Color;           // 버텍스 세이더에서 받은 색, 객체 색으로 씀
in vec3 FragPos;             // 위치값
in vec3 Normal;              // 법선
out vec4 FragColor;          // 최종 픽셀 색

uniform vec3 lightPos;       // 조명 위치
uniform vec3 lightColor;     // 조명 색
uniform vec3 viewPos;        // 카메라 위치
uniform int noLight;         // 0 조명 계산 1 받은 색 그대로 2 조명 색 그대로

void main()
{
	if (noLight == 1) {
		FragColor = vec4(out_Color, 1.0);
		return;
	}
	if (noLight == 2) {
		FragColor = vec4(lightColor, 1.0);
		return;
	}

	// 주변 조명
	float ambientLight = 0.3;
	vec3 ambient = ambientLight * lightColor;

	// 산란 반사 조명
	vec3 normalVector = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;

	// 거울 반사 조명
	float shininess = 128.0;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normalVector);
	float specularLight = max(dot(viewDir, reflectDir), 0.0);
	specularLight = pow(specularLight, shininess);
	vec3 specular = specularLight * lightColor;

	vec3 result = (ambient + diffuse + specular) * out_Color;
	FragColor = vec4(result, 1.0);
}
