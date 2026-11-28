#version 330 core

layout (location = 0) in vec3 vPos;        // 위치 attribute 0번
layout (location = 1) in vec3 vNormal;     // 법선 attribute 1번

out vec3 FragPos;                          // 월드 좌표 위치
out vec3 Normal;                           // 월드 좌표 법선

uniform mat4 model;                        // 모델 변환
uniform mat4 view;                         // 카메라
uniform mat4 projection;                   // 투영

void main()
{
	gl_Position = projection * view * model * vec4(vPos, 1.0);
	FragPos = vec3(model * vec4(vPos, 1.0));
	// 돌리고 늘린 물체도 법선이 표면에 수직이게
	Normal = mat3(transpose(inverse(model))) * vNormal;
}
