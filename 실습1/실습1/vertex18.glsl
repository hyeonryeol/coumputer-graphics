#version 330 core

layout (location = 0) in vec3 vPosition;   // 위치 attribute 0번
layout (location = 1) in vec3 vColor;      // 색상 attribute 1번

out vec3 out_Color;                        // 프래그먼트 세이더로 전달
uniform mat4 modelTransform;               // 모델 변환
uniform mat4 viewTransform;                // 카메라
uniform mat4 projectionTransform;          // 투영

void main()
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPosition, 1.0);
	out_Color = vColor;
}
