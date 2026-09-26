#version 330 core

in vec3 out_Color;         // 버텍스 세이더에서 받은 색
out vec4 FragColor;        // 최종 픽셀 색

void main()
{
	FragColor = vec4(out_Color, 1.0);
}
