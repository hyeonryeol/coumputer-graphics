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
GLint width = 800, height = 600;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];

// 꼭짓점 공책 위치와 법선
GLfloat position[100][3];
GLfloat normal[100][3];
int n = 0;   // 공책에 적힌 꼭짓점 수
int cubestart;   // 공책에서 육면체 위치

// 부품 번호
const int LOWER = 0;    // 아래 몸체
const int MIDDLE = 1;   // 중앙 몸체
const int UPPER = 2;    // 상부 몸체
const int BARREL = 3;   // 포신
const int FLAG = 4;     // 깃대
const int FLOOR = 5;    // 바닥
glm::vec3 partcolor[6];

// 탱크 상태
float tankx = 0.0f;           // 탱크 위치 xz
float tankz = 0.0f;
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
glm::vec3 campos = glm::vec3(0.0f, 1.6f, 4.0f);   // 카메라 위치
float camyaw = 0.0f;          // 카메라가 보는 방향 y축 각도
int camorbit = 0;             // r R 공전

// 조명
glm::vec3 lightstart = glm::vec3(2.2f, 1.4f, -2.2f);   // 처음 자리 화면 오른쪽 위
float lightangle = 0.0f;      // y Y 화면 중심 y축 기준 공전 각도
int lightorbit = 0;           // 1 양 -1 음 0 멈춤
bool lighton = true;          // m 켜기 끄기
glm::vec3 lightcolor[3] = { { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.3f, 0.25f }, { 0.35f, 0.6f, 1.0f } };   // 흰색 빨강 파랑
int lightcolorindex = 0;      // c 조명 색 번호

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

	partcolor[LOWER] = glm::vec3(0.35f, 0.35f, 0.38f);    // 진회색
	partcolor[MIDDLE] = glm::vec3(0.75f, 0.75f, 0.75f);   // 밝은 회색
	partcolor[UPPER] = glm::vec3(0.45f, 0.8f, 0.25f);     // 연두
	partcolor[BARREL] = glm::vec3(0.95f, 0.85f, 0.2f);    // 노랑
	partcolor[FLAG] = glm::vec3(0.95f, 0.6f, 0.1f);       // 주황
	partcolor[FLOOR] = glm::vec3(0.8f, 0.88f, 0.95f);     // 하늘색

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
	std::string vertexSource = filetobuf("vertex27.glsl");
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
	std::string fragmentSource = filetobuf("fragment27.glsl");
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

	// 1번 VBO는 법선, attribute 1번
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

// 점 하나 공책에 적기
void addvertex(float x, float y, float z, float nx, float ny, float nz)
{
	position[n][0] = x;
	position[n][1] = y;
	position[n][2] = z;
	normal[n][0] = nx;
	normal[n][1] = ny;
	normal[n][2] = nz;
	n++;
}

// 크기 1 육면체 공책에 적기, 가운데가 원점 면마다 바깥 방향 법선
void addcube()
{
	float p[8][3] = {
		{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, 0.5f,  0.5f }, { -0.5f, 0.5f,  0.5f },
		{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
	};
	// 앞 뒤 오른 왼 위 아래 바깥에서 보면 반시계
	int face[6][4] = { { 0, 1, 2, 3 }, { 5, 4, 7, 6 }, { 1, 5, 6, 2 }, { 4, 0, 3, 7 }, { 3, 2, 6, 7 }, { 4, 5, 1, 0 } };
	float nv[6][3] = { { 0, 0, 1 }, { 0, 0, -1 }, { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 } };
	int order[6] = { 0, 1, 2, 0, 2, 3 };
	for (int k = 0; k < 6; k++)
		for (int j = 0; j < 6; j++) {
			int v = face[k][order[j]];
			addvertex(p[v][0], p[v][1], p[v][2], nv[k][0], nv[k][1], nv[k][2]);
		}
}

// 육면체 하나 그리기 크기 s 색 c
void drawcube(glm::mat4 m, glm::vec3 s, glm::vec3 c)
{
	m = glm::scale(m, s);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), c.x, c.y, c.z);
	glDrawArrays(GL_TRIANGLES, cubestart, 36);
}

// 지금 조명 위치, 화면 중심 y축 기준으로 돌아감
glm::vec3 lightpos()
{
	glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(lightangle), glm::vec3(0.0f, 1.0f, 0.0f));
	return glm::vec3(r * glm::vec4(lightstart, 1.0f));
}

// 움직임
void update()
{
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

	// 조명 공전
	lightangle += 1.0f * lightorbit;

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

// 그리기
void drawScene()
{
	update();

	glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	n = 0;
	cubestart = n;
	addcube();

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), normal);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 원근 투영
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// 카메라 camyaw 방향으로 조금 아래를 봄
	glm::vec3 dir = glm::vec3(-sin(glm::radians(camyaw)), -0.35f, -cos(glm::radians(camyaw)));
	glm::mat4 view = glm::lookAt(campos, campos + dir, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// 조명 보내기
	glm::vec3 lp = lightpos();
	glm::vec3 lc = lightcolor[lightcolorindex];
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lp.x, lp.y, lp.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), lc.x, lc.y, lc.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), campos.x, campos.y, campos.z);
	glUniform1i(glGetUniformLocation(shaderProgramID, "lightOn"), lighton ? 1 : 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "noLight"), 0);

	// 바닥 4 x 4 판
	drawcube(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f)), glm::vec3(4.0f, 0.02f, 4.0f), partcolor[FLOOR]);

	// 탱크 기준
	glm::mat4 tank = glm::translate(glm::mat4(1.0f), glm::vec3(tankx, 0.0f, tankz));

	// 아래 몸체 2개
	float lowerh = 0.2f;
	for (int i = 0; i < 2; i++) {
		float side = (i == 0) ? -0.23f : 0.23f;
		glm::mat4 m = glm::translate(tank, glm::vec3(side, lowerh / 2.0f, 0.0f));
		drawcube(m, glm::vec3(0.4f, lowerh, 0.55f), partcolor[LOWER]);
	}

	// 중앙 몸체 아래 몸체 위에서 y축 회전
	glm::mat4 middle = glm::translate(tank, glm::vec3(0.0f, lowerh + 0.04f, 0.0f));
	middle = glm::rotate(middle, glm::radians(midangle), glm::vec3(0.0f, 1.0f, 0.0f));
	drawcube(middle, glm::vec3(0.9f, 0.08f, 0.5f), partcolor[MIDDLE]);

	// 상부 몸체 2개 중앙 몸체 따라 돌고 l이면 가운데 기준으로 180도 돌아 자리 바꿈
	for (int i = 0; i < 2; i++) {
		float side = (i == 0) ? -0.25f : 0.25f;
		float sign = (i == 0) ? 1.0f : -1.0f;   // 포신 깃대 반대 방향

		glm::mat4 upper = glm::rotate(middle, glm::radians(swapangle), glm::vec3(0.0f, 1.0f, 0.0f));
		upper = glm::translate(upper, glm::vec3(side, 0.04f + 0.075f, 0.0f));
		upper = glm::rotate(upper, glm::radians(-swapangle), glm::vec3(0.0f, 1.0f, 0.0f));   // 몸체 방향은 그대로
		drawcube(upper, glm::vec3(0.25f, 0.15f, 0.25f), partcolor[UPPER]);

		// 포신 앞면 붙은 곳 기준으로 y축 회전
		glm::mat4 barrel = glm::translate(upper, glm::vec3(0.0f, 0.0f, 0.125f));
		barrel = glm::rotate(barrel, glm::radians(barrelangle * sign), glm::vec3(0.0f, 1.0f, 0.0f));
		barrel = glm::translate(barrel, glm::vec3(0.0f, 0.0f, 0.15f));
		drawcube(barrel, glm::vec3(0.04f, 0.04f, 0.3f), partcolor[BARREL]);

		// 깃대 윗면 붙은 곳 기준으로 x축 회전
		glm::mat4 flag = glm::translate(upper, glm::vec3(0.0f, 0.075f, 0.0f));
		flag = glm::rotate(flag, glm::radians(flagangle * sign), glm::vec3(1.0f, 0.0f, 0.0f));
		flag = glm::translate(flag, glm::vec3(0.0f, 0.15f, 0.0f));
		drawcube(flag, glm::vec3(0.03f, 0.3f, 0.03f), partcolor[FLAG]);
	}

	// 조명 표시 작은 육면체, 조명 색 그대로, 꺼지면 회색
	glUniform1i(glGetUniformLocation(shaderProgramID, "noLight"), 1);
	drawcube(glm::translate(glm::mat4(1.0f), lp), glm::vec3(0.15f), lighton ? lc : glm::vec3(0.3f));
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
		case GLFW_KEY_W:       // 카메라 z 음 방향
			campos.z -= 0.1f;
			break;
		case GLFW_KEY_S:       // 카메라 z 양 방향
			campos.z += 0.1f;
			break;
		case GLFW_KEY_A:       // 카메라 x 음 방향
			campos.x -= 0.1f;
			break;
		case GLFW_KEY_D:       // 카메라 x 양 방향
			campos.x += 0.1f;
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
		case GLFW_KEY_M:       // 조명 켜기 끄기
			lighton = !lighton;
			break;
		case GLFW_KEY_C:       // 조명 색 세 가지 돌아가며
			lightcolorindex = (lightcolorindex + 1) % 3;
			break;
		case GLFW_KEY_Y:       // y 양 Y 음 조명 공전 같은 키 다시 누르면 멈춤
		{
			int want = shift ? -1 : 1;
			lightorbit = (lightorbit == want) ? 0 : want;
			break;
		}
		case GLFW_KEY_R:       // r 양 R 음 카메라 공전 같은 키 다시 누르면 멈춤
		{
			int want = shift ? -1 : 1;
			camorbit = (camorbit == want) ? 0 : want;
			break;
		}
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
		case GLFW_KEY_O:       // 탱크 색 바꾸기 바닥 빼고
			for (int i = 0; i < 5; i++)
				partcolor[i] = glm::vec3(0.2f + 0.8f * randf(), 0.2f + 0.8f * randf(), 0.2f + 0.8f * randf());
			break;
		}
	}
}
