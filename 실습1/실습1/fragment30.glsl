#version 330 core

in vec2 TexCoord;            // 텍스처 좌표
out vec4 FragColor;          // 최종 픽셀 색

uniform sampler2D outTexture;   // 지금 붙인 텍스처
uniform int useTexture;         // 1 텍스처 0 단색
uniform vec3 objectColor;       // 단색일 때 색
uniform float alpha;            // 투명도 1이면 불투명

void main()
{
	if (useTexture == 1)
		FragColor = vec4(texture(outTexture, TexCoord).rgb, alpha);
	else
		FragColor = vec4(objectColor, alpha);
}
