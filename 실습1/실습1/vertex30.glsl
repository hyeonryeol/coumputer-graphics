#version 330 core

layout (location = 0) in vec3 vPos;        // 위치 attribute 0번
layout (location = 1) in vec2 vTexCoord;   // 텍스처 좌표 attribute 1번

out vec2 TexCoord;                         // 프래그먼트로 넘길 텍스처 좌표

uniform mat4 model;                        // 모델 변환
uniform mat4 view;                         // 카메라
uniform mat4 projection;                   // 투영

void main()
{
	gl_Position = projection * view * model * vec4(vPos, 1.0);
	TexCoord = vTexCoord;
}
