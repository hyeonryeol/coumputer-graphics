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
#include <sstream>   // istringstream
#include <vector>    // vector

struct Vertex { float x, y, z; };   // 점 하나
struct Face { int a, b, c; };       // 삼각형 하나 = 점 번호 3개

std::vector<Vertex> spherev;        // 구 점 목록
std::vector<Face> spheref;          // 구 삼각형 목록

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
GLfloat position[5000][3];
GLfloat normal[5000][3];
int n = 0;   // 공책에 적힌 꼭짓점 수

int spherestart, spherecount;   // 공책에서 구 위치
int cubestart;                  // 공책에서 조명 표시 육면체 위치

// 구 세 개 가운데 빨강 왼쪽에 초록 파랑
glm::vec3 ballpos[3] = { { 0.0f, 0.0f, 0.0f }, { -1.5f, 0.0f, 0.0f }, { -2.5f, 0.0f, 0.0f } };
float ballsize[3] = { 1.6f, 1.05f, 0.8f };
glm::vec3 ballcolor[3] = { { 1.0f, 0.1f, 0.1f }, { 0.1f, 0.9f, 0.1f }, { 0.15f, 0.2f, 1.0f } };

// 조명
glm::vec3 lightcolor[3] = { { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.85f, 0.4f }, { 0.45f, 0.75f, 1.0f } };   // 흰색 노랑 하늘
int lightcolorindex = 0;      // c 조명 색 번호
float lightangle = 0.0f;      // r R 가운데 구 y축 기준 회전 각도
float lightdist = 2.0f;       // 가운데 구에서 조명까지 거리, 처음에는 화면 앞쪽

glm::vec3 campos = glm::vec3(-0.6f, 0.0f, 6.0f);   // 카메라 위치

// obj 파일 읽기
void loadobj(const char* name, std::vector<Vertex>& vv, std::vector<Face>& ff)
{
	vv.clear();
	ff.clear();
	std::ifstream file(name);
	if (!file) std::cout << "열기 실패: " << name << std::endl;
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
			Face t;
			ss >> t.a >> t.b >> t.c;
			t.a--; t.b--; t.c--;
			ff.push_back(t);
		}
	}
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
	glDepthFunc(GL_LESS);      // 투영 쓰면 z 작은 쪽이 가까움
	glClearDepth(1.0);
	srand((unsigned int)time(NULL));

	loadobj("sphere.obj", spherev, spheref);   // 반지름 0.3 구

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

// 구 하나를 공책에 적기, 가운데가 원점이라 법선은 위치 방향
void addsphere()
{
	for (int k = 0; k < spheref.size(); k++) {
		int idx[3] = { spheref[k].a, spheref[k].b, spheref[k].c };
		for (int j = 0; j < 3; j++) {
			Vertex v = spherev[idx[j]];
			glm::vec3 nv = glm::normalize(glm::vec3(v.x, v.y, v.z));
			addvertex(v.x, v.y, v.z, nv.x, nv.y, nv.z);
		}
	}
}

// 크기 1 육면체 공책에 적기, 면마다 바깥 방향 법선
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

// 행렬과 색 보내고 그리기
void drawwith(glm::mat4 m, glm::vec3 c, int start, int count)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), c.x, c.y, c.z);
	glDrawArrays(GL_TRIANGLES, start, count);
}

// 지금 조명 위치, 가운데 구 y축 기준으로 돌아감
glm::vec3 lightpos()
{
	float a = glm::radians(lightangle);
	return ballpos[0] + glm::vec3(lightdist * sin(a), 0.4f, lightdist * cos(a));
}

// 그리기
void drawScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	n = 0;
	spherestart = n;
	addsphere();
	spherecount = n - spherestart;
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

	// 카메라 앞에서 구 세 개를 봄
	glm::mat4 view = glm::lookAt(campos, glm::vec3(campos.x, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// 조명 보내기
	glm::vec3 lp = lightpos();
	glm::vec3 lc = lightcolor[lightcolorindex];
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lp.x, lp.y, lp.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), lc.x, lc.y, lc.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), campos.x, campos.y, campos.z);
	glUniform1i(glGetUniformLocation(shaderProgramID, "lightOn"), 1);

	// 구 세 개
	glUniform1i(glGetUniformLocation(shaderProgramID, "noLight"), 0);
	for (int i = 0; i < 3; i++) {
		glm::mat4 m = glm::translate(glm::mat4(1.0f), ballpos[i]);
		m = glm::scale(m, glm::vec3(ballsize[i]));
		drawwith(m, ballcolor[i], spherestart, spherecount);
	}

	// 조명 위치 표시 작은 육면체, 조명 색 그대로
	glUniform1i(glGetUniformLocation(shaderProgramID, "noLight"), 1);
	glm::mat4 m = glm::translate(glm::mat4(1.0f), lp);
	m = glm::scale(m, glm::vec3(0.1f));
	drawwith(m, lc, cubestart, 36);
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_R:   // r 양 방향 R 음 방향 회전
			lightangle += shift ? -5.0f : 5.0f;
			break;
		}
	}
	if (action == GLFW_PRESS)   // 한 번만
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_C:   // 조명 색 세 가지 돌아가며
			lightcolorindex = (lightcolorindex + 1) % 3;
			break;
		}
	}
}
