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

// 꼭짓점 공책
GLfloat position[2000][3];
GLfloat color[2000][3];
int n = 0;   // 공책에 적힌 꼭짓점 수


// 무대 크기 x z는 -2 ~ 2 y는 0 ~ 2
const float SX = 2.0f, SH = 2.0f, SZ = 2.0f;

// 공책 위치
int roomstart;        // 무대 면마다 색 다름
int obstart[3];       // 장애물
int partstart[5];     // 로봇 부품 색
const int P_HEAD = 0, P_NOSE = 1, P_BODY = 2, P_SIDEA = 3, P_SIDEB = 4;

// 장애물 3개 바닥 위 육면체
struct Obstacle {
	float x, z;          // 가운데
	float w, h, d;       // 크기
};
Obstacle ob[3];
float obcolor[3][3] = { { 0.85f, 0.5f, 0.15f }, { 0.55f, 0.35f, 0.7f }, { 0.6f, 0.6f, 0.6f } };

// 로봇 상태
float rx, ry, rz;        // 발바닥 위치
float dirx, dirz;        // 걷는 방향
float speed;             // 한 프레임 이동 거리
float phase;             // 팔다리 흔들기 진행
float vy;                // 점프 위아래 속도
const float ROBOTR = 0.15f;   // 로봇 반경 충돌용

// 무대 앞면
bool dooropen = false;
float doorscale = 1.0f;  // 1 닫힘 0 열림

// 카메라
float camx, camz;        // x X z Z 이동
float camorbit;          // y Y 중심 y축 공전

float randf()
{
	return rand() / (float)RAND_MAX;
}

// 장애물 랜덤 배치 로봇 시작 자리 가운데는 비움
void makeobstacles()
{
	for (int i = 0; i < 3; i++) {
		ob[i].w = 0.3f + 0.3f * randf();
		ob[i].d = 0.3f + 0.3f * randf();
		ob[i].h = 0.12f + 0.2f * randf();
		do {
			ob[i].x = -1.5f + 3.0f * randf();
			ob[i].z = -1.5f + 3.0f * randf();
		} while (fabs(ob[i].x) < 0.6f && fabs(ob[i].z) < 0.6f);
	}
}

void resetall()
{
	rx = 0.0f; ry = 0.0f; rz = 0.0f;
	dirx = 0.0f; dirz = 1.0f;
	speed = 0.012f;
	phase = 0.0f;
	vy = 0.0f;
	dooropen = false;
	doorscale = 1.0f;
	camx = 0.0f; camz = 6.0f;
	camorbit = 0.0f;
	makeobstacles();
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

// 면마다 다른 색 육면체 공책에 적기 순서 앞 뒤 오른 왼 위 아래
void addcubefaces(float fc[6][3])
{
	float p[8][3] = {
		{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, 0.5f,  0.5f }, { -0.5f, 0.5f,  0.5f },
		{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
	};
	int face[6][4] = { { 0, 1, 2, 3 }, { 5, 4, 7, 6 }, { 1, 5, 6, 2 }, { 4, 0, 3, 7 }, { 3, 2, 6, 7 }, { 4, 5, 1, 0 } };
	int order[6] = { 0, 1, 2, 0, 2, 3 };
	for (int k = 0; k < 6; k++)
		for (int j = 0; j < 6; j++) {
			int v = face[k][order[j]];
			addvertex(p[v][0], p[v][1], p[v][2], fc[k][0], fc[k][1], fc[k][2]);
		}
}

// x z 위치가 장애물 위에 있나 로봇 반경만큼 넓혀서 검사
bool onobstacle(int i, float x, float z)
{
	return fabs(x - ob[i].x) < ob[i].w / 2.0f + ROBOTR && fabs(z - ob[i].z) < ob[i].d / 2.0f + ROBOTR;
}

// 발밑 높이 지금 높이보다 낮은 장애물 윗면 중 가장 높은 것
float groundat(float x, float z, float y)
{
	float g = 0.0f;
	for (int i = 0; i < 3; i++)
		if (onobstacle(i, x, z) && y >= ob[i].h - 0.01f && ob[i].h > g)
			g = ob[i].h;
	return g;
}

// 움직임
void update()
{
	// 앞면 열고 닫기
	if (dooropen && doorscale > 0.0f) doorscale -= 0.02f;
	if (!dooropen && doorscale < 1.0f) doorscale += 0.02f;
	doorscale = glm::clamp(doorscale, 0.0f, 1.0f);

	// 다음 자리 검사 가장자리나 높은 장애물이면 뒤로 돌기
	float nx = rx + dirx * speed;
	float nz = rz + dirz * speed;
	bool blocked = fabs(nx) > SX - ROBOTR || fabs(nz) > SZ - ROBOTR;
	for (int i = 0; i < 3; i++)
		if (onobstacle(i, nx, nz) && ry < ob[i].h - 0.01f)
			blocked = true;
	if (blocked)
	{
		dirx = -dirx;
		dirz = -dirz;
	}
	else
	{
		rx = nx;
		rz = nz;
	}

	// 중력 발밑보다 높으면 떨어짐
	float g = groundat(rx, rz, ry);
	if (ry > g || vy > 0.0f)
	{
		vy -= 0.003f;
		ry += vy;
		if (ry <= g)
		{
			ry = g;
			vy = 0.0f;
		}
	}

	// 걷는 속도만큼 팔다리 흔들기
	phase += speed * 8.0f;
}

// 그리기
void drawScene()
{
	update();

	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	n = 0;
	float fc[6][3] = {
		{ 0.75f, 0.8f, 0.9f },    // 앞 문
		{ 0.75f, 0.2f, 0.2f },    // 뒤 빨강
		{ 0.15f, 0.25f, 0.6f },   // 오른쪽 파랑
		{ 0.15f, 0.5f, 0.25f },   // 왼쪽 초록
		{ 0.3f, 0.3f, 0.35f },    // 위
		{ 0.45f, 0.6f, 0.45f },   // 바닥
	};
	roomstart = n;
	addcubefaces(fc);
	for (int i = 0; i < 3; i++) {
		obstart[i] = n;
		addcube(obcolor[i][0], obcolor[i][1], obcolor[i][2]);
	}
	float pc[5][3] = {
		{ 0.95f, 0.8f, 0.6f },   // 머리
		{ 0.9f, 0.1f, 0.1f },    // 코
		{ 0.85f, 0.85f, 0.9f },  // 몸통
		{ 0.2f, 0.3f, 0.9f },    // 한쪽 팔다리 파랑
		{ 0.9f, 0.2f, 0.5f },    // 다른 쪽 팔다리 분홍
	};
	for (int i = 0; i < 5; i++) {
		partstart[i] = n;
		addcube(pc[i][0], pc[i][1], pc[i][2]);
	}

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), color);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 원근 투영
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
	unsigned int projLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));

	// 카메라 x z로 옮긴 다음 중심 y축으로 공전
	glm::mat4 orbit = glm::rotate(glm::mat4(1.0f), glm::radians(camorbit), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 eye = glm::vec3(orbit * glm::vec4(camx, 1.8f, camz, 1.0f));
	glm::vec3 at = glm::vec3(orbit * glm::vec4(camx, 0.7f, camz - 6.0f, 1.0f));
	glm::mat4 view = glm::lookAt(eye, at, glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 I = glm::mat4(1.0f);

	// 무대 앞면 빼고 앞면 컬링 카메라가 돌아도 안쪽이 보임
	glm::mat4 stage = glm::translate(I, glm::vec3(0.0f, SH / 2.0f, 0.0f));
	stage = glm::scale(stage, glm::vec3(SX * 2, SH, SZ * 2));
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	for (int k = 1; k < 6; k++)
		drawwith(stage, GL_TRIANGLES, roomstart + k * 6, 6);
	glDisable(GL_CULL_FACE);

	// 앞면 문 윗변 기준으로 y 스케일 줄어들며 위로 열림
	if (doorscale > 0.0f)
	{
		glm::mat4 door = glm::translate(I, glm::vec3(0.0f, SH, 0.0f));
		door = glm::scale(door, glm::vec3(1.0f, doorscale, 1.0f));
		door = glm::translate(door, glm::vec3(0.0f, -SH / 2.0f, 0.0f));
		door = glm::scale(door, glm::vec3(SX * 2, SH, SZ * 2));
		drawwith(door, GL_TRIANGLES, roomstart, 6);
	}

	// 장애물
	for (int i = 0; i < 3; i++)
		drawcube(glm::translate(I, glm::vec3(ob[i].x, ob[i].h / 2.0f, ob[i].z)), glm::vec3(ob[i].w, ob[i].h, ob[i].d), obstart[i]);

	// 로봇 발바닥 기준 로봇의 앞은 +z
	float yaw = glm::degrees(atan2(dirx, dirz));
	glm::mat4 robot = glm::translate(I, glm::vec3(rx, ry, rz));
	robot = glm::rotate(robot, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

	float swingmax = glm::clamp(speed * 2000.0f, 10.0f, 60.0f);   // 빠를수록 크게 흔듦
	float swing = sin(phase) * swingmax;
	if (ry > groundat(rx, rz, ry) + 0.001f) swing = 0.0f;         // 공중에서는 차렷

	for (int i = 0; i < 2; i++) {
		float side = (i == 0) ? -1.0f : 1.0f;
		int part = (i == 0) ? P_SIDEA : P_SIDEB;

		// 다리 엉덩이 기준으로 흔들기
		glm::mat4 leg = glm::translate(robot, glm::vec3(0.055f * side, 0.28f, 0.0f));
		leg = glm::rotate(leg, glm::radians(swing * side), glm::vec3(1.0f, 0.0f, 0.0f));
		leg = glm::translate(leg, glm::vec3(0.0f, -0.14f, 0.0f));
		drawcube(leg, glm::vec3(0.08f, 0.28f, 0.08f), partstart[part]);

		// 팔 어깨 기준으로 같은 쪽 다리와 반대로 흔들기
		glm::mat4 arm = glm::translate(robot, glm::vec3(0.155f * side, 0.56f, 0.0f));
		arm = glm::rotate(arm, glm::radians(-swing * side), glm::vec3(1.0f, 0.0f, 0.0f));
		arm = glm::translate(arm, glm::vec3(0.0f, -0.12f, 0.0f));
		drawcube(arm, glm::vec3(0.07f, 0.24f, 0.07f), partstart[part]);
	}

	// 몸통 머리 코
	drawcube(glm::translate(robot, glm::vec3(0.0f, 0.43f, 0.0f)), glm::vec3(0.24f, 0.3f, 0.14f), partstart[P_BODY]);
	glm::mat4 head = glm::translate(robot, glm::vec3(0.0f, 0.66f, 0.0f));
	drawcube(head, glm::vec3(0.16f), partstart[P_HEAD]);
	drawcube(glm::translate(head, glm::vec3(0.0f, 0.0f, 0.1f)), glm::vec3(0.04f, 0.04f, 0.05f), partstart[P_NOSE]);
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_EQUAL:      // + 빨라짐
		case GLFW_KEY_KP_ADD:
			speed += 0.002f;
			break;
		case GLFW_KEY_MINUS:      // - 느려짐
		case GLFW_KEY_KP_SUBTRACT:
			speed -= 0.002f;
			break;
		case GLFW_KEY_Z:          // z 앞 Z 뒤
			camz += shift ? 0.1f : -0.1f;
			break;
		case GLFW_KEY_X:          // x 왼쪽 X 오른쪽
			camx += shift ? 0.1f : -0.1f;
			break;
		case GLFW_KEY_Y:          // y Y 중심 y축 공전
			camorbit += shift ? -2.0f : 2.0f;
			break;
		}
		speed = glm::clamp(speed, 0.004f, 0.03f);
	}
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_O:          // 앞면 열기 닫기
			dooropen = !dooropen;
			break;
		case GLFW_KEY_W:          // 앞 안쪽으로
			dirx = 0.0f; dirz = -1.0f;
			break;
		case GLFW_KEY_S:          // 뒤 카메라 쪽으로
			dirx = 0.0f; dirz = 1.0f;
			break;
		case GLFW_KEY_A:          // 왼쪽
			dirx = -1.0f; dirz = 0.0f;
			break;
		case GLFW_KEY_D:          // 오른쪽
			dirx = 1.0f; dirz = 0.0f;
			break;
		case GLFW_KEY_J:          // 땅에 있을 때만 점프
			if (ry <= groundat(rx, rz, ry) + 0.001f)
				vy = 0.06f;
			break;
		case GLFW_KEY_I:          // 전부 리셋
			resetall();
			break;
		}
	}
}
