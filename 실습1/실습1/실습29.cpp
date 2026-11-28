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
GLfloat position[3000][3];
GLfloat normal[3000][3];
int n = 0;          // 공책에 적힌 꼭짓점 수
int cubestart;      // 공책에서 육면체 위치
int pyramidstart;   // 사각뿔 위치
int spherestart;    // 구 위치
int spherecount;    // 구 꼭짓점 수
int circlestart;    // 궤도 원 위치
const int CIRCLE = 72;   // 궤도 원 점 개수

// 행성 수성 금성 지구
const int PLANET = 3;
float orbitr[PLANET] = { 1.0f, 1.4f, 1.8f };          // 공전 반지름
float orbittilt[PLANET] = { 0.0f, 15.0f, -15.0f };    // 궤도 z축 기울기
float planetsize[PLANET] = { 0.15f, 0.22f, 0.26f };   // 지름
float orbitspeed[PLANET] = { 2.0f, 1.4f, 1.0f };      // 한 프레임 각도
float planetangle[PLANET] = { 0.0f, 120.0f, 240.0f };
glm::vec3 planetcolor[PLANET] = { { 0.9f, 0.2f, 0.15f }, { 0.2f, 0.3f, 0.95f }, { 0.15f, 0.8f, 0.25f } };
float orbity = 0.6f;     // 공전 중심 높이

// 눈
struct Snow {
	float x, y, z;
	float speed;
};
const int SNOW = 400;
Snow snow[SNOW];
bool snowfall = false;   // s 내리기 멈추기
bool snowshow = false;   // 처음 s 누르기 전에는 안 보임

// 조명
float lightangle = 0.0f;   // r 화면 중앙 y축 기준 공전 각도
bool lightorbit = false;
float lightdist = 2.4f;    // n f 중앙에서 거리
float lighty = 2.0f;       // 조명 높이
float lightpower = 1.0f;   // + - 조명 세기

// 카메라
glm::vec3 campos = glm::vec3(0.0f, 2.2f, 5.0f);

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
	std::string vertexSource = filetobuf("vertex29.glsl");
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
	std::string fragmentSource = filetobuf("fragment29.glsl");
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
void addvertex(glm::vec3 p, glm::vec3 nv)
{
	position[n][0] = p.x;
	position[n][1] = p.y;
	position[n][2] = p.z;
	normal[n][0] = nv.x;
	normal[n][1] = nv.y;
	normal[n][2] = nv.z;
	n++;
}

// 삼각형 하나 적기 법선은 면에 수직
void addtriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 nv = glm::normalize(glm::cross(b - a, c - a));
	addvertex(a, nv);
	addvertex(b, nv);
	addvertex(c, nv);
}

// 크기 1 육면체 가운데가 원점
void addcube()
{
	glm::vec3 p[8] = {
		{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, 0.5f,  0.5f }, { -0.5f, 0.5f,  0.5f },
		{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
	};
	// 앞 뒤 오른 왼 위 아래 바깥에서 보면 반시계
	int face[6][4] = { { 0, 1, 2, 3 }, { 5, 4, 7, 6 }, { 1, 5, 6, 2 }, { 4, 0, 3, 7 }, { 3, 2, 6, 7 }, { 4, 5, 1, 0 } };
	for (int k = 0; k < 6; k++) {
		addtriangle(p[face[k][0]], p[face[k][1]], p[face[k][2]]);
		addtriangle(p[face[k][0]], p[face[k][2]], p[face[k][3]]);
	}
}

// 사각뿔 밑면 가로세로 1 높이 1 밑면이 y 0
void addpyramid()
{
	glm::vec3 top(0.0f, 1.0f, 0.0f);
	glm::vec3 b[4] = { { -0.5f, 0.0f, 0.5f }, { 0.5f, 0.0f, 0.5f }, { 0.5f, 0.0f, -0.5f }, { -0.5f, 0.0f, -0.5f } };
	for (int k = 0; k < 4; k++)   // 옆면 4개
		addtriangle(b[k], b[(k + 1) % 4], top);
	addtriangle(b[0], b[3], b[2]);   // 밑면
	addtriangle(b[0], b[2], b[1]);
}

// 지름 1 구 법선은 중심에서 바깥
void addsphere()
{
	const int STACK = 16, SLICE = 24;
	for (int i = 0; i < STACK; i++) {
		float t0 = glm::pi<float>() * i / STACK;
		float t1 = glm::pi<float>() * (i + 1) / STACK;
		for (int j = 0; j < SLICE; j++) {
			float p0 = 2.0f * glm::pi<float>() * j / SLICE;
			float p1 = 2.0f * glm::pi<float>() * (j + 1) / SLICE;
			glm::vec3 a(sin(t0) * cos(p0), cos(t0), sin(t0) * sin(p0));
			glm::vec3 b(sin(t1) * cos(p0), cos(t1), sin(t1) * sin(p0));
			glm::vec3 c(sin(t1) * cos(p1), cos(t1), sin(t1) * sin(p1));
			glm::vec3 d(sin(t0) * cos(p1), cos(t0), sin(t0) * sin(p1));
			addvertex(a * 0.5f, a); addvertex(d * 0.5f, d); addvertex(c * 0.5f, c);
			addvertex(a * 0.5f, a); addvertex(c * 0.5f, c); addvertex(b * 0.5f, b);
		}
	}
}

// 반지름 1 xz 평면 원 궤도용
void addcircle()
{
	for (int i = 0; i < CIRCLE; i++) {
		float a = 2.0f * glm::pi<float>() * i / CIRCLE;
		addvertex(glm::vec3(cos(a), 0.0f, sin(a)), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

// 모델 행렬과 색 보내기
void setmodel(glm::mat4 m, glm::vec3 c)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), c.x, c.y, c.z);
}

// 조명 효과 끄고 켜기 1이면 색 그대로
void setnolight(int v)
{
	glUniform1i(glGetUniformLocation(shaderProgramID, "noLight"), v);
}

// 0 1 사이 랜덤
float randf()
{
	return rand() / (float)RAND_MAX;
}

// 눈 하나 위에서 다시 시작 처음이면 높이도 랜덤
void resetsnow(Snow& s, bool first)
{
	s.x = -2.0f + 4.0f * randf();
	s.z = -2.0f + 4.0f * randf();
	s.y = first ? 4.0f * randf() : 4.0f + randf();
	s.speed = 0.01f + 0.03f * randf();
}

// 지금 조명 위치 화면 중앙 y축 기준
glm::vec3 lightpos()
{
	float a = glm::radians(lightangle);
	return glm::vec3(lightdist * cos(a), lighty, -lightdist * sin(a));
}

// 움직임
void update()
{
	for (int i = 0; i < PLANET; i++)
		planetangle[i] += orbitspeed[i];

	if (lightorbit)
		lightangle += 1.0f;

	if (snowfall) {
		for (int i = 0; i < SNOW; i++) {
			snow[i].y -= snow[i].speed;
			if (snow[i].y < 0.02f)   // 바닥에 닿으면 다시 위에서
				resetsnow(snow[i], false);
		}
	}
}

// 그리기
void drawScene()
{
	update();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	n = 0;
	cubestart = n;
	addcube();
	pyramidstart = n;
	addpyramid();
	spherestart = n;
	addsphere();
	spherecount = n - spherestart;
	circlestart = n;
	addcircle();

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), normal);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 원근 투영과 카메라
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 view = glm::lookAt(campos, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// 조명 보내기 세기는 조명 색에 곱함
	glm::vec3 lp = lightpos();
	glm::vec3 lc = glm::vec3(1.0f) * lightpower;
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lp.x, lp.y, lp.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), lc.x, lc.y, lc.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), campos.x, campos.y, campos.z);
	glUniform1i(glGetUniformLocation(shaderProgramID, "lightOn"), 1);
	setnolight(0);

	// 바닥 4 x 4 판
	setmodel(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f)), glm::vec3(4.0f, 0.02f, 4.0f)), glm::vec3(0.6f));
	glDrawArrays(GL_TRIANGLES, cubestart, 36);

	// 가운데 사각뿔
	setmodel(glm::scale(glm::mat4(1.0f), glm::vec3(0.9f, 0.8f, 0.9f)), glm::vec3(0.1f, 0.85f, 0.85f));
	glDrawArrays(GL_TRIANGLES, pyramidstart, 18);

	// 행성 세 개 궤도마다 z축으로 기울어짐
	for (int i = 0; i < PLANET; i++) {
		glm::mat4 orbit = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, orbity, 0.0f));
		orbit = glm::rotate(orbit, glm::radians(orbittilt[i]), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 planet = glm::rotate(orbit, glm::radians(planetangle[i]), glm::vec3(0.0f, 1.0f, 0.0f));
		planet = glm::translate(planet, glm::vec3(orbitr[i], 0.0f, 0.0f));
		planet = glm::scale(planet, glm::vec3(planetsize[i]));
		setmodel(planet, planetcolor[i]);
		glDrawArrays(GL_TRIANGLES, spherestart, spherecount);

		// 궤도 원 조명 없이 행성 색
		setnolight(1);
		setmodel(glm::scale(orbit, glm::vec3(orbitr[i])), planetcolor[i] * 0.7f);
		glDrawArrays(GL_LINE_LOOP, circlestart, CIRCLE);
		setnolight(0);
	}

	// 눈 작은 육면체
	if (snowshow) {
		for (int i = 0; i < SNOW; i++) {
			glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(snow[i].x, snow[i].y, snow[i].z));
			setmodel(glm::scale(m, glm::vec3(0.04f)), glm::vec3(1.0f));
			glDrawArrays(GL_TRIANGLES, cubestart, 36);
		}
	}

	// 조명 표시 흰 육면체 조명 받지 않음
	setnolight(1);
	setmodel(glm::scale(glm::translate(glm::mat4(1.0f), lp), glm::vec3(0.2f)), glm::vec3(1.0f));
	glDrawArrays(GL_TRIANGLES, cubestart, 36);
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)   // 누르고 있으면 계속
	{
		switch (key)
		{
		case GLFW_KEY_N:       // 조명 중앙으로 가까이
			lightdist -= 0.1f;
			if (lightdist < 0.5f) lightdist = 0.5f;
			break;
		case GLFW_KEY_F:       // 조명 멀리
			lightdist += 0.1f;
			if (lightdist > 5.0f) lightdist = 5.0f;
			break;
		case GLFW_KEY_EQUAL:   // + 조명 세기 올리기
		case GLFW_KEY_KP_ADD:
			lightpower += 0.1f;
			if (lightpower > 2.0f) lightpower = 2.0f;
			break;
		case GLFW_KEY_MINUS:   // - 조명 세기 내리기
		case GLFW_KEY_KP_SUBTRACT:
			lightpower -= 0.1f;
			if (lightpower < 0.0f) lightpower = 0.0f;
			break;
		}
	}
	if (action == GLFW_PRESS)   // 켜고 끄는 키는 한 번만
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_S:       // 눈 내리기 멈추기
			if (!snowshow) {   // 처음이면 눈 만들기
				for (int i = 0; i < SNOW; i++)
					resetsnow(snow[i], true);
				snowshow = true;
			}
			snowfall = !snowfall;
			break;
		case GLFW_KEY_R:       // 조명 공전 멈추기
			lightorbit = !lightorbit;
			break;
		}
	}
}
