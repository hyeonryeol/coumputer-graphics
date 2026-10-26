// =====================================================================
// 컴그 코테 창고
// =====================================================================
// 이 파일 하나만 프로젝트에 넣어도 바로 실행되는 기본 틀 + 복붙용 함수 모음
// 셰이더를 문자열로 박아 둬서 glsl 파일 없어도 돌아감
// 실행하면 m 키로 2D 예제 3D 예제 바꿔 보기 가능
//
// 쓰는 법
//   1. 새 cpp 만들고 이 파일 통째로 붙여넣기
//   2. 프로젝트의 다른 cpp는 빌드에서 제외
//      솔루션 탐색기 파일 오른쪽 클릭 속성 빌드에서 제외 예
//   3. 문제에 필요한 부분만 남기고 drawScene KeyCallback 고치기
//   4. 3D 문제면 glEnable GL_DEPTH_TEST 확인 그리고 glClear에 GL_DEPTH_BUFFER_BIT 확인
//
// 목차 Ctrl F로 [번호] 검색
//   [1]  헤더 전역 변수 공책
//   [2]  셰이더 문자열
//   [3]  main 창 만들기 콜백 등록 렌더링 루프
//   [4]  셰이더 컴파일 VAO VBO 공책 올리기
//   [5]  행렬 보내기 그리기 도우미
//   [6]  랜덤 좌표 변환 시간
//   [7]  2D 도형 공책에 적기 점 선 삼각형 사각형 원 다각형
//   [8]  2D 도형 구조체 선택 충돌 합치기 삭제
//   [9]  2D 애니메이션 튕기기 지그재그 스파이럴 테두리 따라가기 크기 색
//   [10] 2D 손 회전 손 크기 변환
//   [11] 보드판 칸 계산
//   [12] 3D 도형 육면체 사각뿔 축 구 원기둥 원뿔 궤도 원 바닥
//   [13] obj 파일 읽기
//   [14] 3D 변환 조각 자전 공전 제자리 크기 원점 크기 경첩 열기 계층
//   [15] 카메라 투영 뷰포트 여러 개
//   [16] 키보드 마우스 패턴
//   [17] 윈도우 뷰포트 변환 클리핑 계산
//   [18] 예제 장면 drawScene KeyCallback 마우스 콜백
//   [19] 자주 나는 실수 체크
// =====================================================================


// =====================================================================
// [1] 헤더 전역 변수 공책
// =====================================================================
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

// 함수 선언 main보다 아래에 몸통이 있는 것들
void make_shaderProgram();
void InitBuffer();
void upload();
void drawScene();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double x, double y);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void FramebufferSizeCallback(GLFWwindow* window, int w, int h);
void resetall();

// 창 크기 문제에서 정해 주면 여기만 바꿈
GLint width = 800, height = 600;
GLFWwindow* gwindow;          // 콜백 밖에서 창이 필요할 때

// 셰이더 프로그램 VAO VBO
GLuint shaderProgramID;
GLuint vao, vbo[2];           // vbo 0번 좌표 1번 색

// 꼭짓점 공책
// 매 프레임 n = 0 으로 비우고 다시 적은 다음 upload 로 GPU에 올림
// 구 obj를 여러 개 쓰면 꼭짓점이 많아지니 크기 넉넉히
const int MAXV = 30000;
GLfloat position[MAXV][3];
GLfloat color[MAXV][3];
int n = 0;                    // 공책에 적힌 꼭짓점 수

// 공책 어디서부터 몇 개가 어떤 도형인지 기억하는 표
// 사용법
//   int s = n; addrect(...);
//   그릴 때 glDrawArrays(GL_TRIANGLES, s, n - s)
struct Part {
	int start;
	int count;
	GLenum mode;              // GL_TRIANGLES GL_LINES GL_LINE_LOOP GL_POINTS
};

const float PI = 3.14159265f;


// =====================================================================
// [2] 셰이더 문자열
// =====================================================================
// 파일 대신 문자열 R"( ... )" 안에 그대로 적음
// 2D에서도 같은 셰이더 사용 그때는 세 행렬을 단위행렬로 보내면 됨 set2d 참고
const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 vPosition;   // 위치 attribute 0번
layout (location = 1) in vec3 vColor;      // 색 attribute 1번
out vec3 out_Color;                        // 프래그먼트로 보냄
uniform mat4 modelTransform;               // 모델 변환
uniform mat4 viewTransform;                // 카메라
uniform mat4 projectionTransform;          // 투영
void main()
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPosition, 1.0);
	out_Color = vColor;
}
)";

const char* fragmentSource = R"(
#version 330 core
in vec3 out_Color;          // 버텍스 셰이더에서 받은 색
out vec4 FragColor;         // 최종 픽셀 색
uniform int useColor;       // 1이면 아래 uColor로 덮어씀 선택 표시나 한 색 칠하기용
uniform vec3 uColor;
void main()
{
	if (useColor == 1)
		FragColor = vec4(uColor, 1.0);
	else
		FragColor = vec4(out_Color, 1.0);
}
)";

// 파일에서 읽고 싶을 때 쓰는 함수 vertex.glsl 처럼 파일이 있을 때
std::string filetobuf(const char* file)
{
	std::ifstream shaderFile(file);
	if (!shaderFile.is_open())
	{
		std::cerr << "파일 열기 실패: " << file << std::endl;
		return "";
	}
	std::string source((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	return source;
}


// =====================================================================
// [3] main 창 만들기 콜백 등록 렌더링 루프
// =====================================================================
int main()
{
	// GLFW 초기화
	if (!glfwInit())
		return -1;

	// OpenGL 3.3 코어 프로파일
	// glColor3f glRectf 같은 옛날 함수 쓰는 문제면 CORE 대신 GLFW_OPENGL_COMPAT_PROFILE
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 윈도우 생성
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "윈도우 생성 실패" << std::endl;
		glfwTerminate();
		return -1;
	}
	gwindow = window;
	glfwMakeContextCurrent(window);     // 컨텍스트 먼저
	glfwSwapInterval(1);                // 모니터 주사율에 맞춰 프레임 고정 애니메이션 속도 일정

	// GLEW 초기화 반드시 컨텍스트 만든 다음
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW 초기화 실패" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);            // 은면제거 2D만 쓰면 없어도 됨
	srand((unsigned int)time(NULL));    // 랜덤 시드 한 번만

	make_shaderProgram();               // 셰이더 프로그램
	InitBuffer();                       // VAO VBO
	resetall();                         // 문제 초기 상태

	// 콜백 등록 안 쓰는 건 지워도 됨
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// 렌더링 루프 매 프레임 그리기 버퍼 교체 이벤트 처리
	while (!glfwWindowShouldClose(window))
	{
		drawScene();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

// 창 크기 바뀌면 뷰포트도 다시 원근 투영 비율도 width height로 계산됨
void FramebufferSizeCallback(GLFWwindow* window, int w, int h)
{
	if (w == 0 || h == 0) return;    // 최소화
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}


// =====================================================================
// [4] 셰이더 컴파일 VAO VBO 공책 올리기
// =====================================================================
// 셰이더 하나 컴파일 실패하면 콘솔에 이유 출력
GLuint compileshader(GLenum type, const char* src)
{
	GLuint s = glCreateShader(type);           // 1 생성
	glShaderSource(s, 1, &src, NULL);          // 2 소스 연결
	glCompileShader(s);                        // 3 컴파일
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(s, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(s, 512, NULL, errorLog);
		std::cerr << "ERROR: shader 컴파일 실패\n" << errorLog << std::endl;
	}
	return s;
}

// 두 셰이더를 붙여 프로그램 만들기
// 파일로 쓰려면 vertexSource 자리에 filetobuf("vertex.glsl").c_str() 결과를 std::string에 담아서 넣기
void make_shaderProgram()
{
	GLuint vs = compileshader(GL_VERTEX_SHADER, vertexSource);
	GLuint fs = compileshader(GL_FRAGMENT_SHADER, fragmentSource);

	shaderProgramID = glCreateProgram();       // 프로그램 생성
	glAttachShader(shaderProgramID, vs);       // 붙이기
	glAttachShader(shaderProgramID, fs);
	glLinkProgram(shaderProgramID);            // 링크
	glDeleteShader(vs);                        // 링크 끝나면 셰이더 객체는 필요 없음
	glDeleteShader(fs);

	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
	}
	glUseProgram(shaderProgramID);
}

// VAO 하나 VBO 두 개 공책 최대 크기만큼 자리 잡기
void InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo);

	// 0번 VBO 좌표 attribute 0번 꼭짓점마다 float 3개
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 1번 VBO 색 attribute 1번
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

// 공책에 적힌 n개만 GPU로 올리기 공책 다 채운 다음 그리기 전에 한 번
void upload()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), color);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
}


// =====================================================================
// [5] 행렬 보내기 그리기 도우미
// =====================================================================
// uniform 이름은 셰이더 문자열과 똑같아야 함 다르면 -1 나오고 아무 일도 안 생김
void setmodel(glm::mat4 m)
{
	unsigned int loc = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}
void setview(glm::mat4 v)
{
	unsigned int loc = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}
void setproj(glm::mat4 p)
{
	unsigned int loc = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(p));
}

// 2D 문제 세 행렬 모두 단위행렬 좌표 그대로 -1 ~ 1 화면
void set2d()
{
	setmodel(glm::mat4(1.0f));
	setview(glm::mat4(1.0f));
	setproj(glm::mat4(1.0f));
}

// 한 색으로 덮어 칠하기 켜기 끄기 선택된 도형 테두리 표시 등에 사용
// 사용법 setcolor(true, 1, 0, 0); 그리기; setcolor(false);
void setcolor(bool on, float r = 0.0f, float g = 0.0f, float b = 0.0f)
{
	glUniform1i(glGetUniformLocation(shaderProgramID, "useColor"), on ? 1 : 0);
	glUniform3f(glGetUniformLocation(shaderProgramID, "uColor"), r, g, b);
}

// 행렬 보내고 공책 일부 그리기 제일 많이 쓰는 함수
void drawwith(glm::mat4 m, GLenum mode, int start, int count)
{
	setmodel(m);
	glDrawArrays(mode, start, count);
}

// 표 Part 하나 그리기
void drawpart(glm::mat4 m, Part p)
{
	drawwith(m, p.mode, p.start, p.count);
}

// 크기 1 육면체를 s 크기로 그리기 addcube로 적은 시작 번호 넘김
void drawcube(glm::mat4 m, glm::vec3 s, int start)
{
	drawwith(glm::scale(m, s), GL_TRIANGLES, start, 36);
}


// =====================================================================
// [6] 랜덤 좌표 변환 시간
// =====================================================================
// 0 ~ 1 실수
float randf()
{
	return rand() / (float)RAND_MAX;
}

// a ~ b 실수
float randrange(float a, float b)
{
	return a + (b - a) * randf();
}

// a ~ b 정수 둘 다 포함
int randint(int a, int b)
{
	return a + rand() % (b - a + 1);
}

// 랜덤 색 세 칸 배열에 넣기 너무 어두우면 안 보이니 최소 0.2
void randcolor(float c[3])
{
	c[0] = 0.2f + 0.8f * randf();
	c[1] = 0.2f + 0.8f * randf();
	c[2] = 0.2f + 0.8f * randf();
}

// 마우스 픽셀 좌표를 GL 좌표 -1 ~ 1 로
// 픽셀은 왼쪽 위가 0 0 이고 아래로 y 증가 GL은 가운데가 0 0 이고 위로 y 증가
void mousetogl(double mx, double my, float& gx, float& gy)
{
	gx = (float)(mx / width * 2.0 - 1.0);
	gy = (float)(1.0 - my / height * 2.0);
}

// GL 좌표를 픽셀로 거꾸로
void gltomouse(float gx, float gy, double& mx, double& my)
{
	mx = (gx + 1.0) / 2.0 * width;
	my = (1.0 - gy) / 2.0 * height;
}

// 타이머 흉내 일정 시간마다 true
// 사용법
//   static double last = 0;
//   if (every(last, 0.5)) { 0.5초마다 할 일 }
bool every(double& last, double sec)
{
	double now = glfwGetTime();
	if (now - last >= sec)
	{
		last = now;
		return true;
	}
	return false;
}

// 프레임 사이 시간 속도를 초당 값으로 쓰고 싶을 때
// 사용법 float dt = deltatime(); x += speed * dt;
float deltatime()
{
	static double last = glfwGetTime();
	double now = glfwGetTime();
	float dt = (float)(now - last);
	last = now;
	return dt;
}


// =====================================================================
// [7] 2D 도형 공책에 적기
// =====================================================================
// 모든 add 함수는 공책 n 뒤에 이어 적음
// 그리기 모드
//   add 결과가 삼각형 묶음이면 GL_TRIANGLES
//   선 두 점이면 GL_LINES  이어진 테두리면 GL_LINE_LOOP

// 점 하나 공책에 적기 모든 add 함수의 기본
void addvertex(float x, float y, float z, float r, float g, float b)
{
	if (n >= MAXV) return;    // 공책 넘침 방지
	position[n][0] = x;
	position[n][1] = y;
	position[n][2] = z;
	color[n][0] = r;
	color[n][1] = g;
	color[n][2] = b;
	n++;
}

// 선 하나 GL_LINES로 그림 꼭짓점 2개
void addline(float x1, float y1, float x2, float y2, float r, float g, float b)
{
	addvertex(x1, y1, 0.0f, r, g, b);
	addvertex(x2, y2, 0.0f, r, g, b);
}

// 삼각형 하나 꼭짓점 3개 반시계로 주면 앞면
void addtri(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b)
{
	addvertex(x1, y1, 0.0f, r, g, b);
	addvertex(x2, y2, 0.0f, r, g, b);
	addvertex(x3, y3, 0.0f, r, g, b);
}

// 사각형 왼쪽 아래 x1 y1 오른쪽 위 x2 y2 삼각형 2개 꼭짓점 6개
void addrect(float x1, float y1, float x2, float y2, float r, float g, float b)
{
	addtri(x1, y1, x2, y1, x2, y2, r, g, b);
	addtri(x1, y1, x2, y2, x1, y2, r, g, b);
}

// 사각형 가운데 cx cy 반폭 hw 반높이 hh 로 주기
void addrectc(float cx, float cy, float hw, float hh, float r, float g, float b)
{
	addrect(cx - hw, cy - hh, cx + hw, cy + hh, r, g, b);
}

// 사각형 테두리 GL_LINE_LOOP로 그림 꼭짓점 4개
void addrectline(float x1, float y1, float x2, float y2, float r, float g, float b)
{
	addvertex(x1, y1, 0.0f, r, g, b);
	addvertex(x2, y1, 0.0f, r, g, b);
	addvertex(x2, y2, 0.0f, r, g, b);
	addvertex(x1, y2, 0.0f, r, g, b);
}

// 점 문제에서 점 대신 작은 사각형
void addpoint(float x, float y, float r, float g, float b)
{
	addrectc(x, y, 0.01f, 0.01f, r, g, b);
}

// 이등변 삼각형 가운데 cx cy 크기 s 꼭짓점이 위 실습8 9 모양
void addisotri(float cx, float cy, float s, float r, float g, float b)
{
	addtri(cx - s, cy - s, cx + s, cy - s, cx, cy + s, r, g, b);
}

// 이등변 삼각형 angle 도 만큼 돌린 것 꼭짓점이 진행 방향 볼 때
void addisotrirot(float cx, float cy, float s, float angle, float r, float g, float b)
{
	float ox[3] = { -s, s, 0.0f };
	float oy[3] = { -s, -s, s };
	float c = cos(glm::radians(angle)), sn = sin(glm::radians(angle));
	for (int k = 0; k < 3; k++)
		addvertex(cx + ox[k] * c - oy[k] * sn, cy + ox[k] * sn + oy[k] * c, 0.0f, r, g, b);
}

// 원 채우기 조각 seg개 삼각형 seg개
// 창이 정사각형이 아니면 찌그러짐 그럴 땐 rx 에 r * height / width 넣기
void addcircle(float cx, float cy, float rx, float ry, int seg, float r, float g, float b)
{
	for (int k = 0; k < seg; k++) {
		float a1 = 2.0f * PI * k / seg;
		float a2 = 2.0f * PI * (k + 1) / seg;
		addtri(cx, cy, cx + rx * cos(a1), cy + ry * sin(a1), cx + rx * cos(a2), cy + ry * sin(a2), r, g, b);
	}
}

// 원 테두리 GL_LINE_LOOP 꼭짓점 seg개
void addcircleline(float cx, float cy, float rad, int seg, float r, float g, float b)
{
	for (int k = 0; k < seg; k++) {
		float a = 2.0f * PI * k / seg;
		addvertex(cx + rad * cos(a), cy + rad * sin(a), 0.0f, r, g, b);
	}
}

// 정 k 각형 채우기 첫 꼭짓점이 위쪽 오각형 육각형 등
void addngon(float cx, float cy, float rad, int k, float r, float g, float b)
{
	for (int i = 0; i < k; i++) {
		float a1 = PI / 2 + 2.0f * PI * i / k;
		float a2 = PI / 2 + 2.0f * PI * (i + 1) / k;
		addtri(cx, cy, cx + rad * cos(a1), cy + rad * sin(a1), cx + rad * cos(a2), cy + rad * sin(a2), r, g, b);
	}
}

// 별 꼭짓점 5개 바깥 반지름 ro 안쪽 반지름 ri
void addstar(float cx, float cy, float ro, float ri, float r, float g, float b)
{
	for (int i = 0; i < 10; i++) {
		float a1 = PI / 2 + PI * i / 5;
		float a2 = PI / 2 + PI * (i + 1) / 5;
		float r1 = (i % 2 == 0) ? ro : ri;
		float r2 = (i % 2 == 0) ? ri : ro;
		addtri(cx, cy, cx + r1 * cos(a1), cy + r1 * sin(a1), cx + r2 * cos(a2), cy + r2 * sin(a2), r, g, b);
	}
}

// 화면 가운데 십자선 사분면 나누기 GL_LINES 꼭짓점 4개
void addcross(float r, float g, float b)
{
	addline(-1.0f, 0.0f, 1.0f, 0.0f, r, g, b);
	addline(0.0f, -1.0f, 0.0f, 1.0f, r, g, b);
}

// 점선 x1 y1 에서 x2 y2 까지 dash 길이마다 끊어서 GL_LINES
void adddash(float x1, float y1, float x2, float y2, float dash, float r, float g, float b)
{
	float len = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	int cnt = (int)(len / dash);
	for (int i = 0; i < cnt; i += 2) {
		float t1 = (float)i / cnt, t2 = (float)(i + 1) / cnt;
		addline(x1 + (x2 - x1) * t1, y1 + (y2 - y1) * t1, x1 + (x2 - x1) * t2, y1 + (y2 - y1) * t2, r, g, b);
	}
}

// 사분면 번호 1 오른쪽 위 2 왼쪽 위 3 왼쪽 아래 4 오른쪽 아래
int quadrant(float x, float y)
{
	if (x >= 0 && y >= 0) return 1;
	if (x < 0 && y >= 0) return 2;
	if (x < 0 && y < 0) return 3;
	return 4;
}

// 사분면 가운데 좌표
void quadcenter(int q, float& cx, float& cy)
{
	cx = (q == 1 || q == 4) ? 0.5f : -0.5f;
	cy = (q == 1 || q == 2) ? 0.5f : -0.5f;
}


// =====================================================================
// [8] 2D 도형 구조체 선택 충돌 합치기 삭제
// =====================================================================
// 도형 하나 정보 문제마다 필요한 칸만 추가
struct Shape {
	int type;             // 0 점 1 선 2 삼각형 3 사각형 4 원 등 마음대로
	float x, y;           // 가운데
	float hw, hh;         // 반폭 반높이 삼각형이면 크기 s 로 hw 사용
	float r, g, b;
	float dx, dy;         // 한 프레임 이동량
	float angle;          // 회전 각도 도
	float scale;          // 크기 배율
	float ox, oy;         // 처음 위치 원래 자리로 돌아가기 용
	bool alive;
};
const int MAXS = 50;
Shape shapes[MAXS];
int scount = 0;           // 도형 수
int selected = -1;        // 선택된 도형 번호 없으면 -1

// 도형 하나 만들기 랜덤 크기 색
void makeshape(Shape& s, int type, float x, float y)
{
	s.type = type;
	s.x = s.ox = x;
	s.y = s.oy = y;
	s.hw = randrange(0.05f, 0.12f);
	s.hh = randrange(0.05f, 0.12f);
	s.r = randrange(0.2f, 1.0f);
	s.g = randrange(0.2f, 1.0f);
	s.b = randrange(0.2f, 1.0f);
	s.dx = (randf() < 0.5f ? -1 : 1) * randrange(0.005f, 0.015f);
	s.dy = (randf() < 0.5f ? -1 : 1) * randrange(0.005f, 0.015f);
	s.angle = 0.0f;
	s.scale = 1.0f;
	s.alive = true;
}

// 도형 추가 최대 넘으면 무시 성공하면 번호
int pushshape(int type, float x, float y)
{
	if (scount >= MAXS) return -1;
	makeshape(shapes[scount], type, x, y);
	return scount++;
}

// i번 도형 삭제 뒤에 것들 한 칸씩 당김
void removeshape(int i)
{
	for (int k = i; k < scount - 1; k++)
		shapes[k] = shapes[k + 1];
	scount--;
	if (selected == i) selected = -1;
	else if (selected > i) selected--;
}

// 점이 사각형 안에 있나
bool inrect(float px, float py, float x1, float y1, float x2, float y2)
{
	return px >= x1 && px <= x2 && py >= y1 && py <= y2;
}

// 점이 가운데 기준 사각형 안에 있나
bool inrectc(float px, float py, float cx, float cy, float hw, float hh)
{
	return fabs(px - cx) <= hw && fabs(py - cy) <= hh;
}

// 두 사각형이 겹치나 왼쪽 아래 오른쪽 위 좌표
bool rectoverlap(float ax1, float ay1, float ax2, float ay2, float bx1, float by1, float bx2, float by2)
{
	return ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1;
}

// 두 도형 겹치나 가운데와 반폭으로
bool shapeoverlap(const Shape& a, const Shape& b)
{
	return fabs(a.x - b.x) < a.hw + b.hw && fabs(a.y - b.y) < a.hh + b.hh;
}

// 점이 원 안에 있나
bool incircle(float px, float py, float cx, float cy, float rad)
{
	return (px - cx) * (px - cx) + (py - cy) * (py - cy) <= rad * rad;
}

// 두 원이 부딪치나
bool circlehit(float x1, float y1, float r1, float x2, float y2, float r2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= (r1 + r2) * (r1 + r2);
}

// 점이 삼각형 안에 있나 세 변 기준 부호가 모두 같으면 안
float edgesign(float px, float py, float ax, float ay, float bx, float by)
{
	return (px - bx) * (ay - by) - (ax - bx) * (py - by);
}
bool intri(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3)
{
	float d1 = edgesign(px, py, x1, y1, x2, y2);
	float d2 = edgesign(px, py, x2, y2, x3, y3);
	float d3 = edgesign(px, py, x3, y3, x1, y1);
	bool neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
	return !(neg && pos);
}

// 점에서 선분까지 거리 선 클릭 선택용 가까우면 선택
float distseg(float px, float py, float x1, float y1, float x2, float y2)
{
	float vx = x2 - x1, vy = y2 - y1;
	float t = ((px - x1) * vx + (py - y1) * vy) / (vx * vx + vy * vy + 1e-6f);
	t = glm::clamp(t, 0.0f, 1.0f);
	float qx = x1 + vx * t, qy = y1 + vy * t;
	return sqrt((px - qx) * (px - qx) + (py - qy) * (py - qy));
}

// 클릭한 곳의 맨 위 도형 찾기 나중에 그린 게 위라서 뒤에서부터 검사 없으면 -1
int pickshape(float gx, float gy)
{
	for (int i = scount - 1; i >= 0; i--) {
		Shape& s = shapes[i];
		float hw = s.hw * s.scale, hh = s.hh * s.scale;
		if (inrectc(gx, gy, s.x, s.y, hw, hh))
			return i;
	}
	return -1;
}

// 선택한 도형 맨 위로 올리기 배열 맨 뒤로 보내기
int bringtofront(int i)
{
	Shape t = shapes[i];
	for (int k = i; k < scount - 1; k++)
		shapes[k] = shapes[k + 1];
	shapes[scount - 1] = t;
	return scount - 1;
}

// 두 사각형 감싸는 큰 사각형으로 합치기 a 자리에 저장 b는 지우기 실습3
void mergeshape(int a, int b)
{
	float x1 = glm::min(shapes[a].x - shapes[a].hw, shapes[b].x - shapes[b].hw);
	float y1 = glm::min(shapes[a].y - shapes[a].hh, shapes[b].y - shapes[b].hh);
	float x2 = glm::max(shapes[a].x + shapes[a].hw, shapes[b].x + shapes[b].hw);
	float y2 = glm::max(shapes[a].y + shapes[a].hh, shapes[b].y + shapes[b].hh);
	shapes[a].x = (x1 + x2) / 2;  shapes[a].y = (y1 + y2) / 2;
	shapes[a].hw = (x2 - x1) / 2; shapes[a].hh = (y2 - y1) / 2;
	shapes[a].r = randf(); shapes[a].g = randf(); shapes[a].b = randf();
	removeshape(b);
}

// 사각형 하나를 네 조각으로 쪼개기 실습6 조각마다 날아갈 방향 dirs
// 사용법 splitfour(i, 0) 좌우상하  splitfour(i, 1) 대각선
void splitfour(int i, int mode)
{
	Shape base = shapes[i];
	removeshape(i);
	float qx[4] = { -1, 1, -1, 1 }, qy[4] = { 1, 1, -1, -1 };     // 네 조각 자리
	float sx[4] = { 0, 0, -1, 1 }, sy[4] = { 1, -1, 0, 0 };       // 상하좌우
	for (int k = 0; k < 4; k++) {
		int j = pushshape(base.type, base.x + qx[k] * base.hw / 2, base.y + qy[k] * base.hh / 2);
		if (j < 0) return;
		shapes[j].hw = base.hw / 2;  shapes[j].hh = base.hh / 2;
		shapes[j].r = base.r; shapes[j].g = base.g; shapes[j].b = base.b;
		float ddx = (mode == 0) ? sx[k] : qx[k];
		float ddy = (mode == 0) ? sy[k] : qy[k];
		shapes[j].dx = ddx * 0.01f;
		shapes[j].dy = ddy * 0.01f;
	}
}

// 도형 전부 공책에 적기 type별로 모양 다르게
// 사각형 type 3 삼각형 type 2 원 type 4 나머지는 점
void addshapes()
{
	for (int i = 0; i < scount; i++) {
		Shape& s = shapes[i];
		if (!s.alive) continue;
		float hw = s.hw * s.scale, hh = s.hh * s.scale;
		if (s.type == 3) addrectc(s.x, s.y, hw, hh, s.r, s.g, s.b);
		else if (s.type == 2) addisotrirot(s.x, s.y, hw, s.angle, s.r, s.g, s.b);
		else if (s.type == 4) addcircle(s.x, s.y, hw, hw, 24, s.r, s.g, s.b);
		else addpoint(s.x, s.y, s.r, s.g, s.b);
	}
}


// =====================================================================
// [9] 2D 애니메이션
// =====================================================================
// 매 프레임 drawScene 안에서 한 번 불러서 위치 바꾸기

// 튕기기 대각선 이동 벽에 닿으면 그 축 방향만 반대로
void movebounce(Shape& s)
{
	s.x += s.dx;
	s.y += s.dy;
	if (s.x + s.hw >= 1.0f || s.x - s.hw <= -1.0f) s.dx = -s.dx;
	if (s.y + s.hh >= 1.0f || s.y - s.hh <= -1.0f) s.dy = -s.dy;
	s.x = glm::clamp(s.x, -1.0f + s.hw, 1.0f - s.hw);   // 벽 안으로 넣어서 끼임 방지
	s.y = glm::clamp(s.y, -1.0f + s.hh, 1.0f - s.hh);
}

// 가로 지그재그 좌우로 가다가 벽에 닿으면 한 줄 아래로 내려가고 방향 반대
// 아래 끝에 닿으면 위로 올라가기 시작
void movezigzag(Shape& s, float rowstep)
{
	s.x += s.dx;
	if (s.x + s.hw >= 1.0f || s.x - s.hw <= -1.0f)
	{
		s.dx = -s.dx;
		s.x = glm::clamp(s.x, -1.0f + s.hw, 1.0f - s.hw);
		s.y += (s.dy > 0 ? rowstep : -rowstep);
		if (s.y + s.hh >= 1.0f || s.y - s.hh <= -1.0f) s.dy = -s.dy;
	}
}

// 상하 뾰족 지그재그 위아래는 빠르게 옆으로는 느리게 실습9 3번
void movesharpzigzag(Shape& s)
{
	s.x += s.dx / 4;
	s.y += s.dy;
	if (s.x + s.hw >= 1.0f || s.x - s.hw <= -1.0f) s.dx = -s.dx;
	if (s.y + s.hh >= 1.0f || s.y - s.hh <= -1.0f) s.dy = -s.dy;
}

// 원 스파이럴 각도 돌면서 반지름이 커졌다 작아졌다
// theta radius rdir 는 도형마다 따로 들고 있어야 함
void movespiral(float& x, float& y, float& theta, float& radius, int& rdir, float cx, float cy)
{
	theta += 0.05f;
	radius += rdir * 0.002f;
	if (radius >= 0.9f) rdir = -1;
	if (radius <= 0.05f) rdir = 1;
	x = cx + radius * cos(theta);
	y = cy + radius * sin(theta);
}

// 원 궤도 그대로 돌기
void movecircle(float& x, float& y, float& theta, float cx, float cy, float radius, float speed)
{
	theta += speed;
	x = cx + radius * cos(theta);
	y = cy + radius * sin(theta);
}

// 창 테두리 따라 시계 방향으로 돌기 실습4 3번
// 위 변은 오른쪽 오른 변은 아래 아래 변은 왼쪽 왼 변은 위로
void moveedge(Shape& s, float speed)
{
	float L = -1.0f + s.hw, R = 1.0f - s.hw, B = -1.0f + s.hh, T = 1.0f - s.hh;
	if (s.y >= T && s.x < R) { s.y = T; s.x = glm::min(s.x + speed, R); }
	else if (s.x >= R && s.y > B) { s.x = R; s.y = glm::max(s.y - speed, B); }
	else if (s.y <= B && s.x > L) { s.y = B; s.x = glm::max(s.x - speed, L); }
	else if (s.x <= L && s.y < T) { s.x = L; s.y = glm::min(s.y + speed, T); }
	else { s.y = glm::min(s.y + speed, T); }     // 테두리에 아직 없으면 위로 붙기
}

// 목표 지점으로 조금씩 다가가기 도착하면 true 원래 자리로 m 키 실습4
bool moveto(float& x, float& y, float tx, float ty, float speed)
{
	float dx = tx - x, dy = ty - y;
	float d = sqrt(dx * dx + dy * dy);
	if (d <= speed) { x = tx; y = ty; return true; }
	x += dx / d * speed;
	y += dy / d * speed;
	return false;
}

// 크기 커졌다 작아졌다 반복 dir 는 1 또는 -1 저장해 두기
void pulse(float& sc, int& dir, float lo, float hi, float step)
{
	sc += step * dir;
	if (sc >= hi) { sc = hi; dir = -1; }
	if (sc <= lo) { sc = lo; dir = 1; }
}

// 색 점점 어둡게 0 이 되면 멈춤 실습6
void darken(float& r, float& g, float& b, float step)
{
	r = glm::max(r - step, 0.0f);
	g = glm::max(g - step, 0.0f);
	b = glm::max(b - step, 0.0f);
}

// 색 점점 밝게 흰색 쪽으로
void brighten(float& r, float& g, float& b, float step)
{
	r = glm::min(r + step, 1.0f);
	g = glm::min(g + step, 1.0f);
	b = glm::min(b + step, 1.0f);
}

// 두 값 사이 t 만큼 t 는 0 ~ 1 애니메이션 진행도
float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

// 자취 남기기 지나온 점 저장 GL_LINE_STRIP으로 그림 실습9
const int MAXTRAIL = 2000;
float trailx[MAXTRAIL], traily[MAXTRAIL];
int trailn = 0;
void recordtrail(float x, float y)
{
	if (trailn < MAXTRAIL) { trailx[trailn] = x; traily[trailn] = y; trailn++; }
}
void addtrail(float r, float g, float b)
{
	for (int i = 0; i < trailn; i++)
		addvertex(trailx[i], traily[i], 0.0f, r, g, b);
}


// =====================================================================
// [10] 2D 손 회전 손 크기 변환
// =====================================================================
// 셰이더 행렬 안 쓰고 좌표를 직접 바꾸는 방법

// 점 px py 를 가운데 cx cy 기준으로 angle 도 돌리기
void rotatepoint(float& px, float& py, float cx, float cy, float angle)
{
	float a = glm::radians(angle);
	float x = px - cx, y = py - cy;
	px = cx + x * cos(a) - y * sin(a);
	py = cy + x * sin(a) + y * cos(a);
}

// 점을 가운데 기준으로 s 배
void scalepoint(float& px, float& py, float cx, float cy, float s)
{
	px = cx + (px - cx) * s;
	py = cy + (py - cy) * s;
}

// 셰이더 행렬로 2D 도형 하나 제자리에서 돌리고 키우기
// 도형을 원점 기준으로 공책에 적고 이 행렬로 위치를 줌
glm::mat4 model2d(float x, float y, float angle, float s)
{
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
	m = glm::rotate(m, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	m = glm::scale(m, glm::vec3(s, s, 1.0f));
	return m;
}


// =====================================================================
// [11] 보드판 칸 계산
// =====================================================================
// 화면을 cols x rows 칸으로 나눈 보드 실습11 20 x 20
const int COLS = 20, ROWS = 20;

// 칸 가운데 좌표 col 0 이 왼쪽 row 0 이 위
void cellcenter(int col, int row, float& x, float& y)
{
	float cw = 2.0f / COLS, ch = 2.0f / ROWS;
	x = -1.0f + cw * (col + 0.5f);
	y = 1.0f - ch * (row + 0.5f);
}

// 좌표가 몇 번째 칸인가
void cellof(float x, float y, int& col, int& row)
{
	col = (int)((x + 1.0f) / 2.0f * COLS);
	row = (int)((1.0f - y) / 2.0f * ROWS);
	col = glm::clamp(col, 0, COLS - 1);
	row = glm::clamp(row, 0, ROWS - 1);
}

// 보드 격자선 GL_LINES
void addgrid(float r, float g, float b)
{
	for (int i = 0; i <= COLS; i++) {
		float x = -1.0f + 2.0f * i / COLS;
		addline(x, -1.0f, x, 1.0f, r, g, b);
	}
	for (int j = 0; j <= ROWS; j++) {
		float y = -1.0f + 2.0f * j / ROWS;
		addline(-1.0f, y, 1.0f, y, r, g, b);
	}
}

// 가까운 칸 가운데로 딱 붙이기 모양판 맞추기 실습10
void snaptocell(float& x, float& y)
{
	int c, r;
	cellof(x, y, c, r);
	cellcenter(c, r, x, y);
}


// =====================================================================
// [12] 3D 도형
// =====================================================================
// 전부 크기 1 가운데가 원점 drawcube 나 glm::scale 로 크기 정함
// 면은 바깥에서 볼 때 반시계 순서 컬링 써도 제대로 보임

// 육면체 꼭짓점 8개 면 순서 앞 뒤 오른 왼 위 아래
float cubep[8][3] = {
	{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, 0.5f,  0.5f }, { -0.5f, 0.5f,  0.5f },
	{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
};
int cubeface[6][4] = { { 0, 1, 2, 3 }, { 5, 4, 7, 6 }, { 1, 5, 6, 2 }, { 4, 0, 3, 7 }, { 3, 2, 6, 7 }, { 4, 5, 1, 0 } };
const int F_FRONT = 0, F_BACK = 1, F_RIGHT = 2, F_LEFT = 3, F_TOP = 4, F_BOTTOM = 5;

// 한 색 육면체 꼭짓점 36개 면마다 밝기 조금 다르게 해서 모서리가 보임
void addcube(float r, float g, float b)
{
	float shade[6] = { 1.0f, 0.75f, 0.85f, 0.85f, 1.1f, 0.6f };
	int order[6] = { 0, 1, 2, 0, 2, 3 };
	for (int k = 0; k < 6; k++)
		for (int j = 0; j < 6; j++) {
			int v = cubeface[k][order[j]];
			float s = shade[k];
			addvertex(cubep[v][0], cubep[v][1], cubep[v][2], glm::min(r * s, 1.0f), glm::min(g * s, 1.0f), glm::min(b * s, 1.0f));
		}
}

// 면마다 색 다른 육면체 fc 순서 앞 뒤 오른 왼 위 아래
// 면 하나만 그리기 drawwith(m, GL_TRIANGLES, start + 면번호 * 6, 6)
void addcubefaces(float fc[6][3])
{
	int order[6] = { 0, 1, 2, 0, 2, 3 };
	for (int k = 0; k < 6; k++)
		for (int j = 0; j < 6; j++) {
			int v = cubeface[k][order[j]];
			addvertex(cubep[v][0], cubep[v][1], cubep[v][2], fc[k][0], fc[k][1], fc[k][2]);
		}
}

// 꼭짓점마다 색 다른 육면체 실습14 색이 면 안에서 섞여 보임
void addcubevertexcolor(float vc[8][3])
{
	int order[6] = { 0, 1, 2, 0, 2, 3 };
	for (int k = 0; k < 6; k++)
		for (int j = 0; j < 6; j++) {
			int v = cubeface[k][order[j]];
			addvertex(cubep[v][0], cubep[v][1], cubep[v][2], vc[v][0], vc[v][1], vc[v][2]);
		}
}

// 사각뿔 바닥 정사각형 y -0.5 꼭대기 0 0.5 0 꼭짓점 18개
// 면 순서 앞 오른 뒤 왼 삼각형 4개 각 3개 그다음 바닥 6개
// 옆면 하나 그리기 drawwith(m, GL_TRIANGLES, start + k * 3, 3)  바닥 start + 12, 6
void addpyramid(float fc[5][3])
{
	float top[3] = { 0.0f, 0.5f, 0.0f };
	float base[4][3] = { { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f } };
	for (int k = 0; k < 4; k++) {
		float* a = base[k];
		float* b = base[(k + 1) % 4];
		addvertex(a[0], a[1], a[2], fc[k][0], fc[k][1], fc[k][2]);
		addvertex(b[0], b[1], b[2], fc[k][0], fc[k][1], fc[k][2]);
		addvertex(top[0], top[1], top[2], fc[k][0], fc[k][1], fc[k][2]);
	}
	int order[6] = { 3, 2, 1, 3, 1, 0 };   // 아래에서 볼 때 반시계
	for (int j = 0; j < 6; j++) {
		float* p = base[order[j]];
		addvertex(p[0], p[1], p[2], fc[4][0], fc[4][1], fc[4][2]);
	}
}

// 좌표축 x 빨강 y 초록 z 파랑 GL_LINES 꼭짓점 6개 길이 len
void addaxes(float len)
{
	addvertex(-len, 0, 0, 1, 0, 0); addvertex(len, 0, 0, 1, 0, 0);
	addvertex(0, -len, 0, 0, 1, 0); addvertex(0, len, 0, 0, 1, 0);
	addvertex(0, 0, -len, 0, 0, 1); addvertex(0, 0, len, 0, 0, 1);
}

// 구 반지름 0.5 obj 없이 직접 만들기 stacks 위아래 조각 slices 둘레 조각
// 꼭짓점 수 stacks * slices * 6
void addsphere(int stacks, int slices, float r, float g, float b)
{
	auto P = [&](int i, int j, float out[3]) {
		float lat = -PI / 2 + PI * i / stacks;      // 위도 아래에서 위로
		float lon = 2.0f * PI * j / slices;         // 경도
		out[0] = 0.5f * cos(lat) * sin(lon);
		out[1] = 0.5f * sin(lat);
		out[2] = 0.5f * cos(lat) * cos(lon);
	};
	for (int i = 0; i < stacks; i++)
		for (int j = 0; j < slices; j++) {
			float a[3], bb[3], c[3], d[3];
			P(i, j, a); P(i, j + 1, bb); P(i + 1, j + 1, c); P(i + 1, j, d);
			addvertex(a[0], a[1], a[2], r, g, b); addvertex(bb[0], bb[1], bb[2], r, g, b); addvertex(c[0], c[1], c[2], r, g, b);
			addvertex(a[0], a[1], a[2], r, g, b); addvertex(c[0], c[1], c[2], r, g, b); addvertex(d[0], d[1], d[2], r, g, b);
		}
}

// 원기둥 원뿔 y -0.5 ~ 0.5 아래 반지름 rb 위 반지름 rt
// rt = 0 이면 원뿔 rb = rt 면 원기둥 꼭짓점 수 slices * 12
void addcylinder(float rb, float rt, int slices, float r, float g, float b)
{
	for (int j = 0; j < slices; j++) {
		float a1 = 2.0f * PI * j / slices, a2 = 2.0f * PI * (j + 1) / slices;
		float b1x = rb * sin(a1), b1z = rb * cos(a1), b2x = rb * sin(a2), b2z = rb * cos(a2);
		float t1x = rt * sin(a1), t1z = rt * cos(a1), t2x = rt * sin(a2), t2z = rt * cos(a2);
		// 옆면
		addvertex(b1x, -0.5f, b1z, r, g, b); addvertex(b2x, -0.5f, b2z, r, g, b); addvertex(t2x, 0.5f, t2z, r, g, b);
		addvertex(b1x, -0.5f, b1z, r, g, b); addvertex(t2x, 0.5f, t2z, r, g, b); addvertex(t1x, 0.5f, t1z, r, g, b);
		// 윗면 아랫면 조금 어둡게
		float s = 0.8f;
		addvertex(0, 0.5f, 0, r * s, g * s, b * s); addvertex(t1x, 0.5f, t1z, r * s, g * s, b * s); addvertex(t2x, 0.5f, t2z, r * s, g * s, b * s);
		addvertex(0, -0.5f, 0, r * s, g * s, b * s); addvertex(b2x, -0.5f, b2z, r * s, g * s, b * s); addvertex(b1x, -0.5f, b1z, r * s, g * s, b * s);
	}
}

// xz 평면 원 반지름 1 궤도 그리기 GL_LINE_LOOP 꼭짓점 seg개 scale로 반지름 조절
void addorbit(int seg, float r, float g, float b)
{
	for (int k = 0; k < seg; k++) {
		float t = 2.0f * PI * k / seg;
		addvertex(cos(t), 0.0f, sin(t), r, g, b);
	}
}

// xz 바닥 판 크기 1 y = 0 삼각형 2개 scale로 크기 조절
void addfloor(float r, float g, float b)
{
	addvertex(-0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, -0.5f, r, g, b);
	addvertex(-0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, -0.5f, r, g, b); addvertex(-0.5f, 0, -0.5f, r, g, b);
}


// =====================================================================
// [13] obj 파일 읽기
// =====================================================================
// v x y z 와 f 를 읽음  f 1 2 3  f 1/2/3 ...  f 1//3 ...  사각형 면 f 1 2 3 4 모두 처리
// obj 번호는 1부터라 1 빼서 저장
struct Vertex { float x, y, z; };
struct Face { int a, b, c; };

void loadobj(const char* name, std::vector<Vertex>& vv, std::vector<Face>& ff)
{
	vv.clear();
	ff.clear();
	std::ifstream file(name);
	if (!file) { std::cout << "열기 실패: " << name << std::endl; return; }
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream ss(line);
		std::string type;
		ss >> type;
		if (type == "v") {
			Vertex p;
			ss >> p.x >> p.y >> p.z;
			vv.push_back(p);
		}
		else if (type == "f") {
			std::vector<int> idx;
			std::string tok;
			while (ss >> tok)
				idx.push_back(std::stoi(tok.substr(0, tok.find('/'))) - 1);   // 슬래시 앞 숫자만
			for (int k = 1; k + 1 < (int)idx.size(); k++)                     // 사각형은 삼각형 두 개로
				ff.push_back({ idx[0], idx[k], idx[k + 1] });
		}
	}
}

// 읽은 obj를 공책에 적기 한 색
void addobj(const std::vector<Vertex>& vv, const std::vector<Face>& ff, float r, float g, float b)
{
	for (const Face& f : ff) {
		int id[3] = { f.a, f.b, f.c };
		for (int j = 0; j < 3; j++)
			addvertex(vv[id[j]].x, vv[id[j]].y, vv[id[j]].z, r, g, b);
	}
}

// 사용법
//   std::vector<Vertex> sv; std::vector<Face> sf;
//   main 에서 한 번 loadobj("sphere.obj", sv, sf);
//   drawScene 에서 int s = n; addobj(sv, sf, 0, 0, 1); drawwith(m, GL_TRIANGLES, s, n - s);
//   obj 파일은 cpp와 같은 폴더 VS 실행 위치가 프로젝트 폴더라서


// =====================================================================
// [14] 3D 변환 조각
// =====================================================================
// 핵심 규칙
//   m = translate(m, ...) 는 m * T  그래서 코드에 나중에 쓴 변환이 정점에 먼저 적용
//   제자리 회전 크기   먼저 translate 로 자리 잡고 그다음 rotate scale
//   공전              먼저 rotate 그다음 translate 거리
//   자식 부품은 부모 행렬에서 이어서 곱함

const glm::vec3 AX(1.0f, 0.0f, 0.0f), AY(0.0f, 1.0f, 0.0f), AZ(0.0f, 0.0f, 1.0f);

// 제자리 자전 위치 p 에서 각도만큼 돌기
glm::mat4 spinat(glm::mat4 base, glm::vec3 p, float ax, float ay)
{
	glm::mat4 m = glm::translate(base, p);
	m = glm::rotate(m, glm::radians(ax), AX);
	m = glm::rotate(m, glm::radians(ay), AY);
	return m;
}

// 원점 중심 y축 공전 반지름 r 각도 a  자기 방향도 같이 돎
glm::mat4 orbity(glm::mat4 base, float a, float r)
{
	glm::mat4 m = glm::rotate(base, glm::radians(a), AY);
	return glm::translate(m, glm::vec3(r, 0.0f, 0.0f));
}

// 공전은 하되 자기 방향은 그대로 두기 돌아가는 만큼 다시 거꾸로 돌림 실습19 상부 몸체
glm::mat4 orbitkeep(glm::mat4 base, float a, glm::vec3 offset)
{
	glm::mat4 m = glm::rotate(base, glm::radians(a), AY);
	m = glm::translate(m, offset);
	return glm::rotate(m, glm::radians(-a), AY);
}

// 제자리 크기 a/A 위치 그대로 도형만 커짐
glm::mat4 scaleinplace(glm::mat4 base, glm::vec3 p, float s)
{
	glm::mat4 m = glm::translate(base, p);
	return glm::scale(m, glm::vec3(s));
}

// 원점 기준 크기 b/B 위치까지 원점에서 멀어지거나 가까워짐
glm::mat4 scaleorigin(glm::mat4 base, glm::vec3 p, float s)
{
	glm::mat4 m = glm::scale(base, glm::vec3(s));
	return glm::translate(m, p);
}

// 경첩 기준 회전 pivot 점을 지나는 axis 축으로 angle 도
// 문 열기 면 열기 팔다리 흔들기 전부 이것
// 사용법 육면체 앞면을 윗변 기준으로 열기 실습15 f
//   glm::mat4 f = hinge(cube, glm::vec3(0, 0.5f, 0.5f), AX, -openangle);
//   drawwith(f, GL_TRIANGLES, start + F_FRONT * 6, 6);
// 사각뿔 앞면을 바닥 모서리 기준으로 열기 실습16
//   hinge(py, glm::vec3(0, -0.5f, 0.5f), AX, openangle)  양수가 바깥쪽
glm::mat4 hinge(glm::mat4 base, glm::vec3 pivot, glm::vec3 axis, float angle)
{
	glm::mat4 m = glm::translate(base, pivot);
	m = glm::rotate(m, glm::radians(angle), axis);
	return glm::translate(m, -pivot);
}

// 면 하나를 제자리에서 작게 뒷면 사라지기 실습15 b  면 가운데 c 기준
glm::mat4 shrinkface(glm::mat4 base, glm::vec3 c, float s)
{
	glm::mat4 m = glm::translate(base, c);
	m = glm::scale(m, glm::vec3(s));
	return glm::translate(m, -c);
}

// 한 변 기준으로 위쪽으로 납작해지며 열리기 문 슬라이드 실습23 앞면
// top 은 윗변 높이 h 는 면 높이 s 는 1 닫힘 0 열림
glm::mat4 slideup(glm::mat4 base, float top, float s)
{
	glm::mat4 m = glm::translate(base, glm::vec3(0.0f, top, 0.0f));
	m = glm::scale(m, glm::vec3(1.0f, s, 1.0f));
	return glm::translate(m, glm::vec3(0.0f, -top, 0.0f));
}

// 두 물체 자리 바꾸기 진행도 t 0 ~ 1 실습17
// 원점 지나며 직선으로 바꾸기 t 키
glm::vec3 swapline(glm::vec3 from, glm::vec3 to, float t)
{
	return from + (to - from) * t;
}
// 하나는 위로 하나는 아래로 반원 그리며 바꾸기 u 키 up 은 1 또는 -1
glm::vec3 swaparc(glm::vec3 from, glm::vec3 to, float t, float h, float up)
{
	glm::vec3 p = from + (to - from) * t;
	p.y += up * h * sin(PI * t);
	return p;
}

// 걷기 팔다리 흔들기 각도 phase 를 매 프레임 증가 max 가 최대 각도
float swingangle(float phase, float maxangle)
{
	return sin(phase) * maxangle;
}

// 계층 예 로봇 팔다리 실습23 그대로 쓰면 됨
// robot 은 발바닥 위치와 방향 cubestart 는 addcube 시작 번호
void drawrobot(glm::mat4 robot, float swing, int bodystart, int headstart, int nosestart, int leftstart, int rightstart)
{
	for (int i = 0; i < 2; i++) {
		float side = (i == 0) ? -1.0f : 1.0f;
		int part = (i == 0) ? leftstart : rightstart;
		// 다리 엉덩이 높이 0.28 기준으로 흔들기
		glm::mat4 leg = hinge(robot, glm::vec3(0.055f * side, 0.28f, 0.0f), AX, swing * side);
		drawcube(glm::translate(leg, glm::vec3(0.055f * side, 0.14f, 0.0f)), glm::vec3(0.08f, 0.28f, 0.08f), part);
		// 팔 어깨 높이 0.56 기준 다리와 반대로
		glm::mat4 arm = hinge(robot, glm::vec3(0.155f * side, 0.56f, 0.0f), AX, -swing * side);
		drawcube(glm::translate(arm, glm::vec3(0.155f * side, 0.44f, 0.0f)), glm::vec3(0.07f, 0.24f, 0.07f), part);
	}
	drawcube(glm::translate(robot, glm::vec3(0.0f, 0.43f, 0.0f)), glm::vec3(0.24f, 0.3f, 0.14f), bodystart);
	glm::mat4 head = glm::translate(robot, glm::vec3(0.0f, 0.66f, 0.0f));
	drawcube(head, glm::vec3(0.16f), headstart);
	drawcube(glm::translate(head, glm::vec3(0.0f, 0.0f, 0.1f)), glm::vec3(0.04f, 0.04f, 0.05f), nosestart);   // 코는 앞 +z
}

// 이동 방향 dx dz 로 몸 돌리는 각도 로봇 탱크가 가는 쪽 보기
float yawof(float dx, float dz)
{
	return glm::degrees(atan2(dx, dz));
}

// 행렬이 옮긴 위치 꺼내기 달 궤도 중심을 행성 위치로 할 때 실습18
glm::vec3 posof(glm::mat4 m)
{
	return glm::vec3(m * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

// 3D 상자끼리 겹치나 가운데 c 반크기 h
bool boxhit(glm::vec3 c1, glm::vec3 h1, glm::vec3 c2, glm::vec3 h2)
{
	return fabs(c1.x - c2.x) < h1.x + h2.x && fabs(c1.y - c2.y) < h1.y + h2.y && fabs(c1.z - c2.z) < h1.z + h2.z;
}

// 방 안에서 공 튕기기 벽 lo hi 반지름 rad 실습21
void bounce3d(glm::vec3& p, glm::vec3& v, glm::vec3 lo, glm::vec3 hi, float rad)
{
	p += v;
	for (int k = 0; k < 3; k++) {
		if (p[k] > hi[k] - rad) { p[k] = hi[k] - rad; v[k] = -v[k]; }
		if (p[k] < lo[k] + rad) { p[k] = lo[k] + rad; v[k] = -v[k]; }
	}
}

// 점프 중력 y 높이 vy 속도 ground 발밑 높이 땅이면 true
bool jumpstep(float& y, float& vy, float ground)
{
	vy -= 0.003f;
	y += vy;
	if (y <= ground) { y = ground; vy = 0.0f; return true; }
	return false;
}


// =====================================================================
// [15] 카메라 투영 뷰포트 여러 개
// =====================================================================
// 투영 원근 직각 바꾸기 p/P
glm::mat4 makeproj(bool perspective, float aspect, float orthosize)
{
	if (perspective)
		return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);   // fovy 라디안 aspect 가로나누기세로 near far 양수
	return glm::ortho(-orthosize * aspect, orthosize * aspect, -orthosize, orthosize, -100.0f, 100.0f);
}

// 고정 카메라 eye 에서 at 을 봄
glm::mat4 lookcam(glm::vec3 eye, glm::vec3 at)
{
	return glm::lookAt(eye, at, AY);
}

// 카메라 자전 제자리에서 보는 방향만 돎 yaw 0 이면 -z 를 봄 pitch 는 아래로 숙이는 양
glm::mat4 yawcam(glm::vec3 pos, float yaw, float pitch)
{
	glm::vec3 dir(-sin(glm::radians(yaw)), -pitch, -cos(glm::radians(yaw)));
	return glm::lookAt(pos, pos + dir, AY);
}

// 카메라 공전 원점 y축 기준 위치 pos 를 a 도 돌림 같이 원점을 바라보게 하려면 yaw 도 a 만큼 더함
glm::vec3 orbitpos(glm::vec3 pos, float a)
{
	float r = glm::radians(a);
	return glm::vec3(pos.x * cos(r) + pos.z * sin(r), pos.y, -pos.x * sin(r) + pos.z * cos(r));
}

// 쿼터뷰 중심을 비스듬히 위에서 거리 dist 각도 angle 높이 비율 hr
glm::mat4 quartercam(float dist, float angle, float hr, glm::vec3 center)
{
	float a = glm::radians(angle);
	glm::vec3 eye = center + glm::vec3(dist * sin(a), dist * hr, dist * cos(a));
	return glm::lookAt(eye, center, AY);
}

// 뷰포트 하나 자리 잡고 그 칸만 지우기 화면 나누기 실습22
// 사용법 drawScene 안에서 칸마다 setviewport 그다음 setproj setview 그다음 같은 그리기 함수
void setviewport(int x, int y, int w, int h, float r, float g, float b)
{
	glViewport(x, y, w, h);              // 왼쪽 아래가 x y
	glScissor(x, y, w, h);
	glEnable(GL_SCISSOR_TEST);           // 이 칸만 지우기
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
}

// 평면도 위에서 내려다보기 xz 평면 화면 위쪽이 -z
glm::mat4 topview()
{
	return glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
}

// 정면도 앞에서 xy 평면
glm::mat4 frontview()
{
	return glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f), AY);
}

// 측면도 오른쪽에서 zy 평면
glm::mat4 sideview()
{
	return glm::lookAt(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f), AY);
}

// 상태 켜고 끄기 모음
// 은면제거 h        glEnable(GL_DEPTH_TEST) / glDisable(GL_DEPTH_TEST)
// 와이어 솔리드 w/W  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) / GL_FILL
// 컬링              glEnable(GL_CULL_FACE); glCullFace(GL_BACK); glFrontFace(GL_CCW);
// 방 안쪽만 보이게   방 그릴 때 glCullFace(GL_FRONT) 방 안 물체는 다시 GL_BACK
// 점 크기 선 굵기    glPointSize(5.0f); glLineWidth(2.0f);


// =====================================================================
// [16] 키보드 마우스 패턴
// =====================================================================
// 소문자 대문자 구분  bool shift = mods & GLFW_MOD_SHIFT;  key 는 대소문자 같은 값
//   case GLFW_KEY_X: anglex += shift ? -2 : 2; break;
// 누를 때 한 번      if (action == GLFW_PRESS)
// 누르고 있는 동안   if (action == GLFW_PRESS || action == GLFW_REPEAT)
// 켜고 끄기         spin = !spin;
// 같은 키 다시 누르면 멈춤 방향 키
//   int want = shift ? 1 : -1; dir = (dir == want) ? 0 : want;
// 계속 눌림 부드러운 이동 KeyCallback 말고 drawScene 안에서
//   if (glfwGetKey(gwindow, GLFW_KEY_LEFT) == GLFW_PRESS) x -= 0.01f;
// 숫자키  GLFW_KEY_1 ~ GLFW_KEY_9 GLFW_KEY_0   + 는 GLFW_KEY_EQUAL 과 GLFW_KEY_KP_ADD  - 는 GLFW_KEY_MINUS 와 GLFW_KEY_KP_SUBTRACT
// 화살표 GLFW_KEY_LEFT RIGHT UP DOWN   엔터 GLFW_KEY_ENTER   스페이스 GLFW_KEY_SPACE   ESC GLFW_KEY_ESCAPE
// 마우스 오른쪽 GLFW_MOUSE_BUTTON_RIGHT  가운데 GLFW_MOUSE_BUTTON_MIDDLE

// 드래그 상태 마우스 콜백 둘이 같이 씀
bool dragging = false;
float lastgx, lastgy;       // 직전 마우스 GL 좌표
double lastmx, lastmy;      // 직전 마우스 픽셀 좌표


// =====================================================================
// [17] 윈도우 뷰포트 변환 클리핑 계산
// =====================================================================
// 윈도우 xwl ~ xwr  ywb ~ ywt 를 뷰포트 xvl ~ xvr  yvb ~ yvt 로
void windowtoviewport(float xw, float yw, float xwl, float ywb, float xwr, float ywt,
	float xvl, float yvb, float xvr, float yvt, float& xv, float& yv)
{
	float sx = (xvr - xvl) / (xwr - xwl);
	float sy = (yvt - yvb) / (ywt - ywb);
	xv = xvl + (xw - xwl) * sx;
	yv = yvb + (yw - ywb) * sy;
}

// Cohen Sutherland 영역 코드 왼쪽 1 오른쪽 2 아래 4 위 8
int outcode(float x, float y, float xl, float yb, float xr, float yt)
{
	int c = 0;
	if (x < xl) c |= 1;
	if (x > xr) c |= 2;
	if (y < yb) c |= 4;
	if (y > yt) c |= 8;
	return c;
}

// Cohen Sutherland 선분 자르기 안 보이면 false 보이면 잘린 끝점으로 바꿔 줌
bool cohenclip(float& x1, float& y1, float& x2, float& y2, float xl, float yb, float xr, float yt)
{
	int c1 = outcode(x1, y1, xl, yb, xr, yt), c2 = outcode(x2, y2, xl, yb, xr, yt);
	while (true) {
		if ((c1 | c2) == 0) return true;      // 둘 다 안
		if (c1 & c2) return false;            // 같은 쪽 바깥
		int c = c1 ? c1 : c2;                 // 바깥 끝점 하나 골라 경계까지 당기기
		float x = 0, y = 0;
		if (c & 8) { x = x1 + (x2 - x1) * (yt - y1) / (y2 - y1); y = yt; }
		else if (c & 4) { x = x1 + (x2 - x1) * (yb - y1) / (y2 - y1); y = yb; }
		else if (c & 2) { y = y1 + (y2 - y1) * (xr - x1) / (x2 - x1); x = xr; }
		else { y = y1 + (y2 - y1) * (xl - x1) / (x2 - x1); x = xl; }
		if (c == c1) { x1 = x; y1 = y; c1 = outcode(x1, y1, xl, yb, xr, yt); }
		else { x2 = x; y2 = y; c2 = outcode(x2, y2, xl, yb, xr, yt); }
	}
}

// Liang Barsky 선분 자르기 u 구간 ustart ~ uend
bool liangclip(float& x1, float& y1, float& x2, float& y2, float xl, float yb, float xr, float yt)
{
	float dx = x2 - x1, dy = y2 - y1;
	float p[4] = { -dx, dx, -dy, dy };
	float q[4] = { x1 - xl, xr - x1, y1 - yb, yt - y1 };
	float us = 0.0f, ue = 1.0f;
	for (int k = 0; k < 4; k++) {
		if (p[k] == 0) { if (q[k] < 0) return false; continue; }   // 평행한데 밖
		float u = q[k] / p[k];
		if (p[k] < 0) us = glm::max(us, u);    // 밖에서 안으로
		else ue = glm::min(ue, u);             // 안에서 밖으로
	}
	if (us > ue) return false;
	float nx1 = x1 + us * dx, ny1 = y1 + us * dy;
	x2 = x1 + ue * dx; y2 = y1 + ue * dy;
	x1 = nx1; y1 = ny1;
	return true;
}


// =====================================================================
// [18] 예제 장면 drawScene KeyCallback 마우스 콜백
// =====================================================================
// 실행해서 함수들이 제대로 도는지 확인하는 용도 문제 풀 때 통째로 갈아엎으면 됨
// m 2D 3D 바꾸기
// 2D  왼쪽 클릭 사각형 만들기 드래그 이동  오른쪽 클릭 삭제  1 튕기기  2 지그재그  3 테두리  c 전부 삭제
// 3D  x/X y/Y 자전  r/R 공전  o 앞면 열기  p/P 원근 직각  w/W 와이어 솔리드  h 은면제거  화살표 이동
// q ESC 종료

bool mode3d = true;

// 2D 상태
int anim2d = 0;              // 0 멈춤 1 튕기기 2 지그재그 3 테두리

// 3D 상태
float spinx = 0, spiny = 0;  // 자전 각도
int spinxdir = 0, spinydir = 0;
float orbitangle = 0;        // 공전 각도
int orbitdir = 0;
bool frontopen = false;      // 앞면 열기
float openangle = 0;
bool perspectiveon = true;
bool wireon = false;
bool depthon = true;
float movex = 0, movey = 0;

void resetall()
{
	scount = 0; selected = -1; anim2d = 0;
	spinx = spiny = 0; spinxdir = spinydir = 0;
	orbitangle = 0; orbitdir = 0;
	frontopen = false; openangle = 0;
	perspectiveon = true; wireon = false; depthon = true;
	movex = movey = 0;
}

void draw2d()
{
	// 움직임
	for (int i = 0; i < scount; i++) {
		if (anim2d == 1) movebounce(shapes[i]);
		if (anim2d == 2) movezigzag(shapes[i], 0.1f);
		if (anim2d == 3) moveedge(shapes[i], 0.01f);
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 공책 채우기
	n = 0;
	int crossstart = n;
	addcross(0.6f, 0.6f, 0.6f);
	int shapestart = n;
	addshapes();
	int shapecount = n - shapestart;
	int selstart = n;
	if (selected >= 0) {
		Shape& s = shapes[selected];
		addrectline(s.x - s.hw * s.scale, s.y - s.hh * s.scale, s.x + s.hw * s.scale, s.y + s.hh * s.scale, 0, 0, 0);
	}

	upload();
	set2d();
	glDrawArrays(GL_LINES, crossstart, 4);
	glDrawArrays(GL_TRIANGLES, shapestart, shapecount);
	if (selected >= 0) {
		glLineWidth(3.0f);
		glDrawArrays(GL_LINE_LOOP, selstart, 4);    // 선택 표시 검은 테두리
		glLineWidth(1.0f);
	}
}

void draw3d()
{
	// 움직임
	spinx += 2.0f * spinxdir;
	spiny += 2.0f * spinydir;
	orbitangle += 1.0f * orbitdir;
	if (frontopen && openangle < 90.0f) openangle += 2.0f;
	if (!frontopen && openangle > 0.0f) openangle -= 2.0f;

	glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (depthon) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);

	// 공책 채우기
	n = 0;
	int axesstart = n;
	addaxes(1.5f);
	float fc[6][3] = { { 1, 1, 0 }, { 0, 1, 1 }, { 1, 0, 1 }, { 1, 0.5f, 0 }, { 0.6f, 0.6f, 0.6f }, { 0.5f, 0, 1 } };
	int cubestart = n;
	addcubefaces(fc);
	float pc[5][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 0 }, { 0.5f, 0.5f, 0.5f } };
	int pystart = n;
	addpyramid(pc);
	int spherestart = n;
	addsphere(12, 20, 0.3f, 0.8f, 1.0f);
	int spherecount = n - spherestart;
	int orbitstart = n;
	addorbit(72, 0.7f, 0.7f, 0.7f);

	upload();
	setcolor(false);
	float aspect = (float)width / height;
	setproj(makeproj(perspectiveon, aspect, 1.5f));
	setview(lookcam(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f)));

	// 전체를 x y 30도 기울이기
	glm::mat4 world = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), AX);
	world = glm::rotate(world, glm::radians(-30.0f), AY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawwith(world, GL_LINES, axesstart, 6);                              // 축
	drawwith(glm::scale(world, glm::vec3(1.2f)), GL_LINE_LOOP, orbitstart, 72);   // 궤도

	glPolygonMode(GL_FRONT_AND_BACK, wireon ? GL_LINE : GL_FILL);

	// 왼쪽 육면체 제자리 자전 앞면은 윗변 기준으로 열림
	glm::mat4 cube = spinat(world, glm::vec3(-0.8f + movex, movey, 0.0f), spinx, spiny);
	cube = glm::scale(cube, glm::vec3(0.6f));
	for (int k = 1; k < 6; k++)
		drawwith(cube, GL_TRIANGLES, cubestart + k * 6, 6);
	drawwith(hinge(cube, glm::vec3(0.0f, 0.5f, 0.5f), AX, -openangle), GL_TRIANGLES, cubestart + F_FRONT * 6, 6);

	// 오른쪽 사각뿔 제자리 자전
	glm::mat4 py = spinat(world, glm::vec3(0.8f + movex, movey, 0.0f), spinx, spiny);
	drawwith(glm::scale(py, glm::vec3(0.6f)), GL_TRIANGLES, pystart, 18);

	// 구 원점 y축 공전 궤도 반지름 1.2
	glm::mat4 ball = orbity(world, orbitangle, 1.2f);
	drawwith(glm::scale(ball, glm::vec3(0.4f)), GL_TRIANGLES, spherestart, spherecount);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// 그리기 매 프레임
void drawScene()
{
	if (mode3d) draw3d();
	else draw2d();
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);

	// 누르고 있는 동안 반복
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT:  movex -= 0.05f; break;
		case GLFW_KEY_RIGHT: movex += 0.05f; break;
		case GLFW_KEY_UP:    movey += 0.05f; break;
		case GLFW_KEY_DOWN:  movey -= 0.05f; break;
		}
	}

	// 누를 때 한 번
	if (action != GLFW_PRESS) return;
	switch (key)
	{
	case GLFW_KEY_Q:
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_M:        // 2D 3D 바꾸기
		mode3d = !mode3d;
		break;
	case GLFW_KEY_X:        // x 양 X 음 자전 같은 키 다시 누르면 멈춤
	{
		int want = shift ? -1 : 1;
		spinxdir = (spinxdir == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_Y:
	{
		int want = shift ? -1 : 1;
		spinydir = (spinydir == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_R:        // r 양 R 음 공전
	{
		int want = shift ? -1 : 1;
		orbitdir = (orbitdir == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_O: frontopen = !frontopen; break;
	case GLFW_KEY_P: perspectiveon = !shift; break;     // p 원근 P 직각
	case GLFW_KEY_W: wireon = !shift; break;            // w 와이어 W 솔리드
	case GLFW_KEY_H: depthon = !depthon; break;
	case GLFW_KEY_1: anim2d = (anim2d == 1) ? 0 : 1; break;
	case GLFW_KEY_2: anim2d = (anim2d == 2) ? 0 : 2; break;
	case GLFW_KEY_3: anim2d = (anim2d == 3) ? 0 : 3; break;
	case GLFW_KEY_C: scount = 0; selected = -1; break;
	case GLFW_KEY_S: resetall(); break;
	}
}

// 마우스 버튼 콜백
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	float gx, gy;
	mousetogl(mx, my, gx, gy);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		int hit = pickshape(gx, gy);
		if (hit >= 0) {
			selected = bringtofront(hit);   // 잡은 도형 맨 위로
			dragging = true;
			lastgx = gx; lastgy = gy;
		}
		else {
			selected = pushshape(3, gx, gy);   // 빈 곳이면 그 자리에 사각형
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// 놓았을 때 겹치면 합치기 실습3
		if (dragging && selected >= 0)
			for (int i = 0; i < scount; i++)
				if (i != selected && shapeoverlap(shapes[i], shapes[selected])) {
					int keep = (i < selected) ? i : selected - 1;
					mergeshape(i, selected);
					selected = keep;
					break;
				}
		dragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		int hit = pickshape(gx, gy);
		if (hit >= 0) removeshape(hit);
	}
}

// 마우스 이동 콜백 드래그 중일 때만 이동량만큼 옮김
void CursorPosCallback(GLFWwindow* window, double x, double y)
{
	if (!dragging || selected < 0) return;
	float gx, gy;
	mousetogl(x, y, gx, gy);
	shapes[selected].x += gx - lastgx;
	shapes[selected].y += gy - lastgy;
	lastgx = gx;
	lastgy = gy;
}

// 마우스 휠 앞으로 1 뒤로 -1 줌 등에 사용
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (selected >= 0) {
		shapes[selected].scale += 0.1f * (float)yoffset;
		shapes[selected].scale = glm::clamp(shapes[selected].scale, 0.3f, 3.0f);
	}
}


// =====================================================================
// [19] 자주 나는 실수 체크
// =====================================================================
// 화면이 비었다
//   upload 를 안 불렀다  n 을 매 프레임 0 으로 안 비웠다  glDrawArrays 시작 번호 개수 틀림
//   glUseProgram glBindVertexArray 빠짐  uniform 이름 오타로 행렬이 0 행렬
//   glm::mat4 m; 만 쓰면 0 행렬 반드시 glm::mat4(1.0f)
// 3D 가 뒤죽박죽 겹친다
//   glEnable(GL_DEPTH_TEST) 빠짐  glClear 에 GL_DEPTH_BUFFER_BIT 빠짐
// 원근 투영인데 아무것도 안 보인다
//   카메라가 물체 안에 있음 lookAt eye 를 z 5 정도로  near 가 0 이하
// 회전이 이상한 곳을 중심으로 돈다
//   translate rotate 순서 바뀜  제자리는 translate 다음 rotate  공전은 rotate 다음 translate
// 각도가 너무 빨리 돈다
//   glm::rotate 는 라디안 glm::radians(각도) 로 감싸기
// 대문자 키가 안 먹는다
//   key 는 대소문자 같은 값 mods & GLFW_MOD_SHIFT 로 구분
// 마우스 위치가 위아래 반대
//   gy = 1 - my / height * 2  y 뒤집기
// 키 한 번에 여러 번 실행
//   GLFW_REPEAT 까지 받는 블록에 켜고 끄는 키를 넣었음 PRESS 블록으로
// obj 가 안 읽힌다
//   파일 위치 cpp 옆  f 줄에 슬래시 있는지  번호 1 빼기
// 공책 넘침
//   MAXV 키우기 구 obj 는 하나에 2880 개
// =====================================================================
