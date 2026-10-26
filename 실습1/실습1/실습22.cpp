#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

// 사용자 정의 함수
std::string filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
void drawScene();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// 필요한 변수
GLint width = 1000, height = 600;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];

// 꼭짓점 공책
GLfloat position[2000][3];
GLfloat color[2000][3];
int n = 0;   // 공책에 적힌 꼭짓점 수

// 부품 번호
const int LOWER = 0;    // 아래 몸체
const int MIDDLE = 1;   // 중앙 몸체
const int UPPER = 2;    // 상부 몸체
const int BARREL = 3;   // 포신
const int FLAG = 4;     // 깃대
const int FLOOR = 5;    // 바닥
float partcolor[6][3];
int partstart[6];       // 공책에서 부품 육면체 위치
int floorstart;

// 탱크 상태
float tankx = 0.0f;           // 탱크 위치 xz
float tankz = 0.0f;
float lowersize = 1.0f;       // d 아래 몸체 y 크기
int lowerdir = 1;
bool loweranim = false;
float midangle = 0.0f;        // t 중앙 몸체 회전
bool midspin = false;
float swapangle = 0.0f;       // l 상부 몸체 자리 바꾸기
float swaptarget = 0.0f;
float barrelangle = 0.0f;     // g 포신 회전
int barreldir = 1;
bool barrelanim = false;
float flagangle = 0.0f;       // p 깃대 회전
int flagdir = 1;
bool flaganim = false;

// 카메라
glm::vec3 campos;             // 카메라 위치
float camyaw;                 // 카메라가 보는 방향 y축 각도
int camspin = 0;              // y Y 자전 1 반시계 -1 시계
int camorbit = 0;             // r R 공전

void resetall()
{
	tankx = 0.0f; tankz = 0.0f;
	lowersize = 1.0f; lowerdir = 1; loweranim = false;
	midangle = 0.0f; midspin = false;
	swapangle = 0.0f; swaptarget = 0.0f;
	barrelangle = 0.0f; barreldir = 1; barrelanim = false;
	flagangle = 0.0f; flagdir = 1; flaganim = false;
	campos = glm::vec3(0.0f, 1.6f, 4.0f);
	camyaw = 0.0f;
	camspin = 0; camorbit = 0;

	float base[6][3] = {
		{ 0.35f, 0.35f, 0.38f },   // 아래 몸체 진회색
		{ 0.75f, 0.75f, 0.75f },   // 중앙 몸체 밝은 회색
		{ 0.45f, 0.8f, 0.25f },    // 상부 몸체 연두
		{ 0.95f, 0.85f, 0.2f },    // 포신 노랑
		{ 0.95f, 0.6f, 0.1f },     // 깃대 주황
		{ 0.8f, 0.88f, 0.95f },    // 바닥 하늘색
	};
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 3; j++)
			partcolor[i][j] = base[i][j];
}

int main()
{
	// GLFW 초기화
	if (!glfwInit())
		return -1;

	// OpenGL 3.3 코어 프로파일
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
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);   // 모니터 주사율에 맞춰 속도 고정

	// GLEW 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW 초기화 실패" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);   // 은면제거
	srand((unsigned int)time(NULL));
	resetall();

	// 세이더 읽어서 세이더 프로그램 만들기
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	// VAO, VBO 만들기
	InitBuffer();

	// 콜백 등록
	glfwSetKeyCallback(window, KeyCallback);

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

// 파일 전체를 문자열로 읽기
std::string filetobuf(const char* file)
{
	std::ifstream shaderFile(file);
	if (!shaderFile.is_open())
	{
		std::cerr << "파일 열기 실패: " << file << std::endl;
		return "";
	}
	std::string source((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	return source;
}

// 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	std::string vertexSource = filetobuf("vertex18.glsl");
	const char* source = vertexSource.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &source, NULL);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
	}
}

// 프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	std::string fragmentSource = filetobuf("fragment.glsl");
	const char* source = fragmentSource.c_str();

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &source, NULL);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
	}
}

// 세이더 프로그램 만들고 두 세이더 연결
GLuint make_shaderProgram()
{
	GLint result;
	GLchar errorLog[512];

	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);

	// 링크 끝났으니 세이더 객체는 삭제
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return 0;
	}
	glUseProgram(shaderID);
	return shaderID;
}

// VAO, VBO 만들고 공책 크기만큼 자리 잡기
void InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo);

	// 0번 VBO는 좌표, attribute 0번
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 1번 VBO는 색, attribute 1번
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

// 점 하나 공책에 적기
void addvertex(float x, float y, float z, float r, float g, float b)
{
	position[n][0] = x;
	position[n][1] = y;
	position[n][2] = z;
	color[n][0] = r;
	color[n][1] = g;
	color[n][2] = b;
	n++;
}

// 크기 1 육면체 공책에 적기 가운데가 원점 면마다 밝기 조금 다르게
void addcube(float r, float g, float b)
{
	float p[8][3] = {
		{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, 0.5f,  0.5f }, { -0.5f, 0.5f,  0.5f },
		{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
	};
	// 앞 뒤 오른 왼 위 아래 바깥에서 보면 반시계
	int face[6][4] = { { 0, 1, 2, 3 }, { 5, 4, 7, 6 }, { 1, 5, 6, 2 }, { 4, 0, 3, 7 }, { 3, 2, 6, 7 }, { 4, 5, 1, 0 } };
	float shade[6] = { 1.0f, 0.75f, 0.85f, 0.85f, 1.1f, 0.6f };
	for (int k = 0; k < 6; k++) {
		int order[6] = { 0, 1, 2, 0, 2, 3 };
		for (int j = 0; j < 6; j++) {
			int v = face[k][order[j]];
			float s = shade[k];
			addvertex(p[v][0], p[v][1], p[v][2], glm::min(r * s, 1.0f), glm::min(g * s, 1.0f), glm::min(b * s, 1.0f));
		}
	}
}

// 행렬 보내고 그리기
void drawwith(glm::mat4 m, GLenum mode, int start, int count)
{
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(m));
	glDrawArrays(mode, start, count);
}

// 육면체 하나 그리기 크기 s
void drawcube(glm::mat4 m, glm::vec3 s, int start)
{
	drawwith(glm::scale(m, s), GL_TRIANGLES, start, 36);
}

// 움직임
void update()
{
	if (loweranim)   // d 아래 몸체 y 크기
	{
		lowersize += 0.02f * lowerdir;
		if (lowersize > 1.6f) lowerdir = -1;
		if (lowersize < 0.5f) lowerdir = 1;
	}
	if (midspin)     // t 중앙 몸체 회전
		midangle += 1.5f;
	if (swapangle < swaptarget)   // l 180도까지 돌아서 자리 바꿈
		swapangle += 3.0f;
	if (barrelanim)  // g 포신 흔들기
	{
		barrelangle += 1.5f * barreldir;
		if (barrelangle > 45.0f) barreldir = -1;
		if (barrelangle < -45.0f) barreldir = 1;
	}
	if (flaganim)    // p 깃대 흔들기
	{
		flagangle += 1.5f * flagdir;
		if (flagangle > 45.0f) flagdir = -1;
		if (flagangle < -45.0f) flagdir = 1;
	}

	// 카메라 자전 제자리에서 보는 방향만 돌기
	camyaw += 1.0f * camspin;

	// 카메라 공전 원점 y축 기준으로 위치 돌리고 보는 방향도 같이 돌기
	if (camorbit != 0)
	{
		float a = glm::radians(1.0f * camorbit);
		float x = campos.x * cos(a) + campos.z * sin(a);
		float z = -campos.x * sin(a) + campos.z * cos(a);
		campos.x = x;
		campos.z = z;
		camyaw += 1.0f * camorbit;
	}
}

// 뷰포트 하나 자리 잡고 그 칸만 배경색으로 지우기
void setviewport(int x, int y, int w, int h, float r, float g, float b)
{
	glViewport(x, y, w, h);
	glScissor(x, y, w, h);
	glEnable(GL_SCISSOR_TEST);
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
}

// 투영 카메라 보내기
void setcamera(glm::mat4 projection, glm::mat4 view)
{
	unsigned int projLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
}

void drawtank();

// 그리기
void drawScene()
{
	update();

	// 여기서 공책 채우기
	n = 0;
	for (int i = 0; i < 6; i++) {
		partstart[i] = n;
		addcube(partcolor[i][0], partcolor[i][1], partcolor[i][2]);
	}

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), color);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int bigw = width * 6 / 10;      // 왼쪽 큰 칸 너비
	int smallw = width - bigw;      // 오른쪽 작은 칸 너비
	int halfh = height / 2;

	// 뷰포트 1 왼쪽 큰 칸 원근 투영 실습19 카메라 그대로
	setviewport(0, 0, bigw, height, 0.2f, 0.2f, 0.25f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)bigw / height, 0.1f, 100.0f);
	glm::vec3 dir = glm::vec3(-sin(glm::radians(camyaw)), -0.35f, -cos(glm::radians(camyaw)));
	glm::mat4 view = glm::lookAt(campos, campos + dir, glm::vec3(0.0f, 1.0f, 0.0f));
	setcamera(projection, view);
	drawtank();

	// 직각 투영 범위 칸 비율 맞춤
	float a = (float)smallw / halfh;
	glm::mat4 ortho = glm::ortho(-2.2f * a, 2.2f * a, -2.2f, 2.2f, 0.1f, 20.0f);

	// 뷰포트 2 오른쪽 위 평면도 위에서 아래로 xz 평면 화면 위쪽이 -z
	setviewport(bigw, halfh, smallw, height - halfh, 0.12f, 0.18f, 0.15f);
	view = glm::lookAt(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	setcamera(ortho, view);
	drawtank();

	// 뷰포트 3 오른쪽 아래 정면도 앞에서 xy 평면
	setviewport(bigw, 0, smallw, halfh, 0.15f, 0.12f, 0.12f);
	view = glm::lookAt(glm::vec3(0.0f, 0.5f, 5.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	setcamera(ortho, view);
	drawtank();
}

// 바닥과 탱크 그리기 뷰포트마다 한 번씩
void drawtank()
{
	// 바닥 4 x 4 판
	drawcube(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f)), glm::vec3(4.0f, 0.02f, 4.0f), partstart[FLOOR]);

	// 탱크 기준
	glm::mat4 tank = glm::translate(glm::mat4(1.0f), glm::vec3(tankx, 0.0f, tankz));

	// 아래 몸체 2개 바닥에 붙어서 y로만 커짐
	float lowerh = 0.2f * lowersize;
	for (int i = 0; i < 2; i++) {
		float side = (i == 0) ? -0.23f : 0.23f;
		glm::mat4 m = glm::translate(tank, glm::vec3(side, lowerh / 2.0f, 0.0f));
		drawcube(m, glm::vec3(0.4f, lowerh, 0.55f), partstart[LOWER]);
	}

	// 중앙 몸체 아래 몸체 위에서 y축 회전
	glm::mat4 middle = glm::translate(tank, glm::vec3(0.0f, lowerh + 0.04f, 0.0f));
	middle = glm::rotate(middle, glm::radians(midangle), glm::vec3(0.0f, 1.0f, 0.0f));
	drawcube(middle, glm::vec3(0.9f, 0.08f, 0.5f), partstart[MIDDLE]);

	// 상부 몸체 2개 중앙 몸체 따라 돌고 l이면 가운데 기준으로 180도 돌아 자리 바꿈
	for (int i = 0; i < 2; i++) {
		float side = (i == 0) ? -0.25f : 0.25f;
		float sign = (i == 0) ? 1.0f : -1.0f;   // 포신 깃대 반대 방향

		glm::mat4 upper = glm::rotate(middle, glm::radians(swapangle), glm::vec3(0.0f, 1.0f, 0.0f));
		upper = glm::translate(upper, glm::vec3(side, 0.04f + 0.075f, 0.0f));
		upper = glm::rotate(upper, glm::radians(-swapangle), glm::vec3(0.0f, 1.0f, 0.0f));   // 몸체 방향은 그대로
		drawcube(upper, glm::vec3(0.25f, 0.15f, 0.25f), partstart[UPPER]);

		// 포신 앞면 붙은 곳 기준으로 y축 회전
		glm::mat4 barrel = glm::translate(upper, glm::vec3(0.0f, 0.0f, 0.125f));
		barrel = glm::rotate(barrel, glm::radians(barrelangle * sign), glm::vec3(0.0f, 1.0f, 0.0f));
		barrel = glm::translate(barrel, glm::vec3(0.0f, 0.0f, 0.15f));
		drawcube(barrel, glm::vec3(0.04f, 0.04f, 0.3f), partstart[BARREL]);

		// 깃대 윗면 붙은 곳 기준으로 x축 회전
		glm::mat4 flag = glm::translate(upper, glm::vec3(0.0f, 0.075f, 0.0f));
		flag = glm::rotate(flag, glm::radians(flagangle * sign), glm::vec3(1.0f, 0.0f, 0.0f));
		flag = glm::translate(flag, glm::vec3(0.0f, 0.15f, 0.0f));
		drawcube(flag, glm::vec3(0.03f, 0.3f, 0.03f), partstart[FLAG]);
	}
}

// 0 1 사이 랜덤
float randf()
{
	return rand() / (float)RAND_MAX;
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT:    // 탱크 이동 바닥 밖으로 못 나감
			tankx -= 0.05f;
			break;
		case GLFW_KEY_RIGHT:
			tankx += 0.05f;
			break;
		case GLFW_KEY_UP:
			tankz -= 0.05f;
			break;
		case GLFW_KEY_DOWN:
			tankz += 0.05f;
			break;
		case GLFW_KEY_Z:       // z 카메라 z 양 Z 음
			campos.z += shift ? -0.1f : 0.1f;
			break;
		case GLFW_KEY_X:       // x 카메라 x 양 X 음
			campos.x += shift ? -0.1f : 0.1f;
			break;
		}
		tankx = glm::clamp(tankx, -1.5f, 1.5f);
		tankz = glm::clamp(tankz, -1.6f, 1.6f);
	}
	if (action == GLFW_PRESS)   // 켜고 끄는 키는 한 번만
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_D:       // 아래 몸체 확대 축소 시작 멈춤
			loweranim = !loweranim;
			break;
		case GLFW_KEY_T:       // 중앙 몸체 회전 시작 멈춤
			midspin = !midspin;
			break;
		case GLFW_KEY_L:       // 상부 몸체 자리 바꾸기 한 번
			if (swapangle >= swaptarget)
				swaptarget += 180.0f;
			break;
		case GLFW_KEY_G:       // 포신 회전 시작 멈춤
			barrelanim = !barrelanim;
			break;
		case GLFW_KEY_P:       // 깃대 회전 시작 멈춤
			flaganim = !flaganim;
			break;
		case GLFW_KEY_Y:       // y 시계 Y 반시계 자전 같은 키 다시 누르면 멈춤
		{
			int want = shift ? 1 : -1;
			camspin = (camspin == want) ? 0 : want;
			break;
		}
		case GLFW_KEY_R:       // r 시계 R 반시계 공전
		{
			int want = shift ? 1 : -1;
			camorbit = (camorbit == want) ? 0 : want;
			break;
		}
		case GLFW_KEY_O:       // 색 바꾸기 바닥 빼고
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 3; j++)
					partcolor[i][j] = 0.2f + 0.8f * randf();
			break;
		case GLFW_KEY_C:       // 초기화
			resetall();
			break;
		}
	}
}
