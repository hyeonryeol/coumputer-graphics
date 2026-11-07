// =====================================================================
// 컴그 코테 틀
// =====================================================================
// 시험 시작하면 제일 먼저 이 파일 통째로 붙여넣기
// cube.obj 를 cpp 와 같은 프로젝트 폴더에 넣고 Ctrl F5
// 실행해서 흰 화면에 파란 육면체 하나 나오면 성공 obj 못 찾으면 콘솔에 열기 실패
// 그다음 코테창고.cpp 에서 필요한 기능만 골라 붙여넣기
//   창고 함수 넣는 자리  [창고 붙이는 자리] 검색
//   문제 푸는 자리      [문제 풀이] 검색  initobj drawScene KeyCallback 마우스 콜백 고치기
// 이 틀에 이미 있는 것  창고 [1] ~ [5] 전부  [6] mousetogl  [7] addvertex addline addtri addrect  [13] 전부
// =====================================================================


// =====================================================================
// 헤더 전역 변수 공책
// =====================================================================
// lib 링크 프로젝트 링커 설정 대신
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

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
void initobj();
void resetall();

// 창 크기 문제에서 정해 주면 여기만 바꿈
GLint width = 800, height = 600;
GLFWwindow* gwindow;          // 콜백 밖에서 창이 필요할 때

// 셰이더 프로그램 VAO VBO
GLuint shaderProgramID;
GLuint vao, vbo[2];           // vbo 0번 좌표 1번 색

// 꼭짓점 공책
// 매 프레임 n = 0 으로 비우고 다시 적은 다음 upload 로 GPU에 올림
const int MAXV = 30000;
GLfloat position[MAXV][3];
GLfloat color[MAXV][3];
int n = 0;                    // 공책에 적힌 꼭짓점 수

// 공책 어디서부터 몇 개가 어떤 도형인지
struct Part {
	int start;
	int count;
	GLenum mode;              // GL_TRIANGLES GL_LINES GL_LINE_LOOP GL_POINTS
};

const float PI = 3.14159265f;


// =====================================================================
// 셰이더 문자열
// =====================================================================
// 2D 문제도 같은 셰이더 세 행렬을 단위행렬로 보내면 됨 set2d
const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
out vec3 out_Color;
uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
void main()
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPosition, 1.0);
	out_Color = vColor;
}
)";

const char* fragmentSource = R"(
#version 330 core
in vec3 out_Color;
out vec4 FragColor;
uniform int useColor;
uniform vec3 uColor;
void main()
{
	if (useColor == 1)
		FragColor = vec4(uColor, 1.0);
	else
		FragColor = vec4(out_Color, 1.0);
}
)";


// =====================================================================
// main 창 만들기 콜백 등록 렌더링 루프
// =====================================================================
int main()
{
	if (!glfwInit())
		return -1;

	// OpenGL 3.3 코어
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "윈도우 생성 실패" << std::endl;
		glfwTerminate();
		return -1;
	}
	gwindow = window;
	glfwMakeContextCurrent(window);     // 컨텍스트 먼저
	glfwSwapInterval(1);                // 프레임 고정

	// GLEW 초기화 컨텍스트 만든 다음
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW 초기화 실패" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);            // 은면제거
	srand((unsigned int)time(NULL));    // 랜덤 시드

	make_shaderProgram();
	InitBuffer();
	initobj();                          // obj 파일 읽기 한 번
	resetall();

	// 콜백 등록
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// 렌더링 루프
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

// 창 크기 바뀌면 뷰포트도 다시
void FramebufferSizeCallback(GLFWwindow* window, int w, int h)
{
	if (w == 0 || h == 0) return;    // 최소화
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}


// =====================================================================
// 셰이더 컴파일 VAO VBO 공책 올리기
// =====================================================================
GLuint compileshader(GLenum type, const char* src)
{
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &src, NULL);
	glCompileShader(s);
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

void make_shaderProgram()
{
	GLuint vs = compileshader(GL_VERTEX_SHADER, vertexSource);
	GLuint fs = compileshader(GL_FRAGMENT_SHADER, fragmentSource);

	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vs);
	glAttachShader(shaderProgramID, fs);
	glLinkProgram(shaderProgramID);
	glDeleteShader(vs);
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

// VAO 하나 VBO 두 개 공책 최대 크기만큼
void InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo);

	// 0번 좌표
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 1번 색
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

// 공책 n개 GPU로 그리기 전에 한 번
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
// 행렬 보내기 그리기 도우미
// =====================================================================
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

// 2D 문제 세 행렬 단위행렬
void set2d()
{
	setmodel(glm::mat4(1.0f));
	setview(glm::mat4(1.0f));
	setproj(glm::mat4(1.0f));
}

// 한 색 덮어 칠하기  setcolor(true, 1, 0, 0); 그리기; setcolor(false);
void setcolor(bool on, float r = 0.0f, float g = 0.0f, float b = 0.0f)
{
	glUniform1i(glGetUniformLocation(shaderProgramID, "useColor"), on ? 1 : 0);
	glUniform3f(glGetUniformLocation(shaderProgramID, "uColor"), r, g, b);
}

// 행렬 보내고 공책 일부 그리기
void drawwith(glm::mat4 m, GLenum mode, int start, int count)
{
	setmodel(m);
	glDrawArrays(mode, start, count);
}

// Part 하나 그리기
void drawpart(glm::mat4 m, Part p)
{
	drawwith(m, p.mode, p.start, p.count);
}


// =====================================================================
// 공책 기본 쓰기 마우스 좌표
// =====================================================================
// 점 하나
void addvertex(float x, float y, float z, float r, float g, float b)
{
	if (n >= MAXV) return;
	position[n][0] = x;
	position[n][1] = y;
	position[n][2] = z;
	color[n][0] = r;
	color[n][1] = g;
	color[n][2] = b;
	n++;
}

// 선 GL_LINES 꼭짓점 2개
void addline(float x1, float y1, float x2, float y2, float r, float g, float b)
{
	addvertex(x1, y1, 0.0f, r, g, b);
	addvertex(x2, y2, 0.0f, r, g, b);
}

// 삼각형 꼭짓점 3개
void addtri(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b)
{
	addvertex(x1, y1, 0.0f, r, g, b);
	addvertex(x2, y2, 0.0f, r, g, b);
	addvertex(x3, y3, 0.0f, r, g, b);
}

// 사각형 왼쪽 아래 x1 y1 오른쪽 위 x2 y2 꼭짓점 6개
void addrect(float x1, float y1, float x2, float y2, float r, float g, float b)
{
	addtri(x1, y1, x2, y1, x2, y2, r, g, b);
	addtri(x1, y1, x2, y2, x1, y2, r, g, b);
}

// 마우스 픽셀 좌표를 GL 좌표 -1 ~ 1 로 y 뒤집기
void mousetogl(double mx, double my, float& gx, float& gy)
{
	gx = (float)(mx / width * 2.0 - 1.0);
	gy = (float)(1.0 - my / height * 2.0);
}


// =====================================================================
// obj 파일 읽기
// =====================================================================
// v 와 f 읽기  f 1 2 3  f 1/2/3  f 1//3  사각형 면 모두 처리  번호 1 빼서 저장
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

// 읽은 obj 공책에 적기 한 색
void addobj(const std::vector<Vertex>& vv, const std::vector<Face>& ff, float r, float g, float b)
{
	for (const Face& f : ff) {
		int id[3] = { f.a, f.b, f.c };
		for (int j = 0; j < 3; j++)
			addvertex(vv[id[j]].x, vv[id[j]].y, vv[id[j]].z, r, g, b);
	}
}


// =====================================================================
// [창고 붙이는 자리]
// =====================================================================
// 코테창고.cpp [6] ~ [17] 에서 필요한 함수 여기 붙여넣기
// 위에 이미 있는 함수는 또 붙이면 재정의 에러
const float OBJSIZE = 0.6f;
const glm::vec3 AX(1.0f, 0.0f, 0.0f), AY(0.0f, 1.0f, 0.0f), AZ(0.0f, 0.0f, 1.0f);

void drawchecker(glm::mat4 base, int count, int first, int second)
{
	float half = (count - 1) / 2.0f;
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < count; j++)
		{
			glm::mat4 cell = glm::translate(base, glm::vec3(i - half, 0.0f, j - half));
			int start = ((i + j) % 2 == 0) ? first : second;
			drawwith(cell, GL_TRIANGLES, start, 6);
		}
	}
}
void addfloor(float r, float g, float b)
{
	addvertex(-0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, -0.5f, r, g, b);
	addvertex(-0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, -0.5f, r, g, b); addvertex(-0.5f, 0, -0.5f, r, g, b);
}
void drawobj(glm::mat4 m, glm::vec3 s, int start, int count)
{
	drawwith(glm::scale(m, s / OBJSIZE), GL_TRIANGLES, start, count);
}

glm::mat4 hinge(glm::mat4 base, glm::vec3 pivot, glm::vec3 axis, float angle)
{
	glm::mat4 m = glm::translate(base, pivot);
	m = glm::rotate(m, glm::radians(angle), axis);
	return glm::translate(m, -pivot);
}
glm::mat4 orbity(glm::mat4 base, float a, float r)
{
	glm::mat4 m = glm::rotate(base, glm::radians(a), AY);
	return glm::translate(m, glm::vec3(r, 0.0f, 0.0f));
}
// =====================================================================
// [문제 풀이]
// =====================================================================
// obj 담는 곳 도형마다 한 쌍
std::vector<Vertex> cubev; std::vector<Face> cubef;

// obj 파일 읽기 main 에서 한 번  더 쓰면 위에 한 쌍 추가하고 여기 한 줄 추가
void initobj()
{
	loadobj("cube.obj", cubev, cubef);
}

// 상태 변수 여기
float a1 = 0.0f;
bool apress = false;
int a1dir = 0;
float jinja = 30.0f;
float s = 1.0f;
float a2 = 0.0f;

// 처음 상태 s 키 리셋에도 사용
void resetall()
{
	a1 = 0.0f;
	apress = false;
	a1dir = 0;
	jinja = 30.0f;
	s = 1.0f;
	a2 = 0.0f;
}

// 그리기 매 프레임
void drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 공책 비우고 적기
	n = 0;
	int cube = n;
	addobj(cubev, cubef, 1.0f, 0.5f, 0.5f);
	int cubecount = n - cube;
	int blue = n; addfloor(0.2f, 0.2f, 1.0f);
	int white = n; addfloor(0.9f, 0.9f, 0.9f);
	int cube2 = n;
	addobj(cubev, cubef, 1.0f, 0.0f, 1.0f);
	int cube3 = n;
	addobj(cubev, cubef, 0.0f, 0.5f, 1.0f);

	int cube4 = n;
	addobj(cubev, cubef, 0.0f, 0.5f, 0.0f);
	if (apress == true)
	{
		if (a1 >= jinja)
		{
			a1dir = -1;
		}
		else if (a1 <= -jinja)
		{
			a1dir = 1;
		}
		if (a1dir == 1)
		{
			a1 += 0.5f;
		}
		else if (a1dir == -1)
		{
			a1 -= 0.5f;
		}
	}
	a2 += 0.1f;

	upload();
	setcolor(false);

	// 카메라 원근 투영 z 3 에서 원점 보기
	setproj(glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f));
	setview(glm::lookAt(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	drawchecker(glm::mat4(1.0f), 4, blue, white); 

	glm::mat4 m = orbity(glm::mat4(1.0f), a2, 0.0f);
	
	drawobj(m, glm::vec3(0.2f, 2.0f, 0.2f), cube, cubecount);
	m = glm::translate(m, glm::vec3(0.5f, 0.0f, 0.0f));
	drawobj(m, glm::vec3(0.2f, 2.0f, 0.2f), cube, cubecount);
	m = glm::translate(m, glm::vec3(0.0f, 0.9f, 0.0f));
	drawobj(m, glm::vec3(1.0f, 0.2f, 0.2f), cube, cubecount);


	glm::mat4 m2 = hinge(m, glm::vec3(0.0f, 0.0f, 0.0f), AX, a1);
	drawobj(glm::translate(m2, glm::vec3(-0.3f, -0.4f, 0.0f)), glm::vec3(0.1f, 0.8f, 0.1f), cube2, cubecount);
	drawobj(glm::translate(m2, glm::vec3(0.3f, -0.4f, 0.0f)), glm::vec3(0.1f, 0.8f, 0.1f), cube2, cubecount);
	drawobj(glm::translate(m2, glm::vec3(0.0f, -0.8f, 0.0f)), glm::vec3(0.8f, 0.1f, 0.2f), cube2, cubecount);
	//사람
	drawobj(glm::translate(m2, glm::vec3(0.0f, -0.6f, 0.0f)), glm::vec3(0.2f, 0.3f, 0.2f), cube3, cubecount);

	glm::mat4 m3 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	drawobj(glm::translate(m3, glm::vec3(0.0f, -0.8f, 0.0f)), glm::vec3(0.3f*s, 0.3f*s, 0.3f*s), cube4, cubecount);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	setcolor(false);

	// 2D 문제면 setproj setview 대신 set2d 그다음 drawwith
}

// 키보드
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);

	// 누르고 있는 동안 반복 이동 키
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT:  break;
		case GLFW_KEY_RIGHT: break;
		}
	}

	// 누를 때 한 번 켜고 끄는 키
	if (action != GLFW_PRESS) return;
	switch (key)
	{
	case GLFW_KEY_Q:
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_S:
		resetall();
		break;
	case GLFW_KEY_P:
		apress = !apress;
		a1dir = 1;
		break;
	case GLFW_KEY_EQUAL:
		if(jinja < 180.0f)
			jinja += 10.0f;
		break;
	case GLFW_KEY_MINUS:
		if (jinja > 10.0f)
			jinja -= 10.0f;
		break;
	
	
	}
}

// 마우스 버튼
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	float gx, gy;
	mousetogl(mx, my, gx, gy);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
	}
}

// 마우스 이동 드래그
void CursorPosCallback(GLFWwindow* window, double x, double y)
{
}

// 마우스 휠 앞 1 뒤 -1
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
}
