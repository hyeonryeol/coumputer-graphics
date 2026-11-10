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
GLint width = 600, height = 600;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];

// 꼭짓점 공책
GLfloat position[22000][3];
GLfloat color[22000][3];
int n = 0;   // 공책에 적힌 꼭짓점 수

const int CIRCLE = 72;               // 궤도 원 꼭짓점 수
int circlestart, circlecount;        // 공책에서 궤도 원 위치
int sunstart, suncount;              // 공책에서 중심 구 위치
int planetstart[3], moonstart[3];    // 공책에서 행성 달 위치
int spherecount;                     // 구 하나 꼭짓점 수

// 색
float planetcolor[3][3] = { { 0.1f, 0.7f, 0.2f }, { 0.95f, 0.55f, 0.1f }, { 0.6f, 0.2f, 0.8f } };
float mooncolor[3][3] = { { 0.9f, 0.1f, 0.1f }, { 1.0f, 0.45f, 0.75f }, { 0.55f, 0.35f, 0.15f } };

// 공전
float planetangle[3] = { 0.0f, 120.0f, 240.0f };   // 행성 공전 각도
float planetspeed[3] = { 1.0f, 0.6f, 1.4f };        // 행성마다 다른 속도
float pathtilt[3] = { 0.0f, 45.0f, -45.0f };        // 경로 기울기 z축 기준
float moonangle[3] = { 0.0f, 90.0f, 180.0f };       // 달 공전 각도
float moonspeed[3] = { 3.0f, 2.2f, 4.0f };
float orbitr = 1.2f;                                 // 행성 궤도 반지름
float moonr = 0.3f;                                  // 달 궤도 반지름
float radiusscale = 1.0f;                            // 1번 키 궤도 크기

// 키 상태
bool perspectiveon = true;   // p 직각 P 원근
bool wireon = false;         // m 솔리드 M 와이어
float movex = 0.0f;          // wasd 이동
float movey = 0.0f;
float movez = 0.0f;          // + - 이동

bool sunanim = false;        // 2번 중심 구 확대 축소
float sunsize = 1.0f;
int sundir = 1;

bool moonspin = false;       // 3번 달 궤도 자전
float moonorbitangle = 0.0f;

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

// 궤도 원을 공책에 적기 xz 평면 반지름 1
void addcircle()
{
	for (int k = 0; k < CIRCLE; k++) {
		float t = glm::radians(360.0f * k / CIRCLE);
		position[n][0] = cos(t);
		position[n][1] = 0.0f;
		position[n][2] = sin(t);
		color[n][0] = 0.45f;
		color[n][1] = 0.45f;
		color[n][2] = 0.45f;
		n++;
	}
}

// 구 하나를 공책에 적기
void addsphere(float r, float g, float b)
{
	for (int k = 0; k < spheref.size(); k++) {
		int idx[3] = { spheref[k].a, spheref[k].b, spheref[k].c };
		for (int j = 0; j < 3; j++) {
			position[n][0] = spherev[idx[j]].x;
			position[n][1] = spherev[idx[j]].y;
			position[n][2] = spherev[idx[j]].z;
			color[n][0] = r;
			color[n][1] = g;
			color[n][2] = b;
			n++;
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

// 그리기
void drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 움직임
	for (int i = 0; i < 3; i++) {
		planetangle[i] += planetspeed[i];
		moonangle[i] += moonspeed[i];
	}
	if (sunanim)   // 2번 확대 축소
	{
		sunsize += 0.01f * sundir;
		if (sunsize > 1.4f) sundir = -1;
		if (sunsize < 0.6f) sundir = 1;
	}
	if (moonspin)   // 3번 달 궤도 자전
		moonorbitangle += 1.0f;

	// 여기서 공책 채우기
	n = 0;
	circlestart = n;
	addcircle();
	circlecount = n - circlestart;

	float t = (sunsize - 0.6f) / 0.8f;   // 크기에 따라 색 바뀜
	sunstart = n;
	addsphere(0.0f, 0.2f + 0.6f * t, 1.0f - 0.4f * t);
	suncount = n - sunstart;
	spherecount = suncount;
	for (int i = 0; i < 3; i++) {
		planetstart[i] = n;
		addsphere(planetcolor[i][0], planetcolor[i][1], planetcolor[i][2]);
		moonstart[i] = n;
		addsphere(mooncolor[i][0], mooncolor[i][1], mooncolor[i][2]);
	}

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), color);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 투영
	glm::mat4 projection;
	if (perspectiveon)
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);   // 원근
	else
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);   // 직각
	unsigned int projLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));

	// 카메라 조금 위에서 내려다봄
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 1.5f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	// 전체 이동 wasd + -
	glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(movex, movey, movez));

	// 중심 구
	glPolygonMode(GL_FRONT_AND_BACK, wireon ? GL_LINE : GL_FILL);
	drawwith(glm::scale(base, glm::vec3(sunsize)), GL_TRIANGLES, sunstart, suncount);

	float R = orbitr * radiusscale;   // 행성 궤도 반지름
	float mr = moonr * radiusscale;   // 달 궤도 반지름

	for (int i = 0; i < 3; i++) {
		// 경로 기울이기 z축 기준
		glm::mat4 path = glm::rotate(base, glm::radians(pathtilt[i]), glm::vec3(0.0f, 0.0f, 1.0f));

		// 행성 궤도
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawwith(glm::scale(path, glm::vec3(R)), GL_LINE_LOOP, circlestart, circlecount);

		// 행성 공전
		glm::mat4 planet = glm::rotate(path, glm::radians(planetangle[i]), glm::vec3(0.0f, 1.0f, 0.0f));
		planet = glm::translate(planet, glm::vec3(R, 0.0f, 0.0f));
		glPolygonMode(GL_FRONT_AND_BACK, wireon ? GL_LINE : GL_FILL);
		drawwith(glm::scale(planet, glm::vec3(0.45f)), GL_TRIANGLES, planetstart[i], spherecount);

		// 달 궤도 중심은 행성 위치 기울기는 따라가지 않음
		glm::vec4 p = planet * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::mat4 moonbase = glm::translate(glm::mat4(1.0f), glm::vec3(p));
		moonbase = glm::rotate(moonbase, glm::radians(moonorbitangle), glm::vec3(0.0f, 0.0f, 1.0f));   // 3번 z축 자전

		// 달 궤도
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawwith(glm::scale(moonbase, glm::vec3(mr)), GL_LINE_LOOP, circlestart, circlecount);

		// 달 공전
		glm::mat4 moon = glm::rotate(moonbase, glm::radians(moonangle[i]), glm::vec3(0.0f, 1.0f, 0.0f));
		moon = glm::translate(moon, glm::vec3(mr, 0.0f, 0.0f));
		glPolygonMode(GL_FRONT_AND_BACK, wireon ? GL_LINE : GL_FILL);
		drawwith(glm::scale(moon, glm::vec3(0.22f)), GL_TRIANGLES, moonstart[i], spherecount);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		bool shift = (mods & GLFW_MOD_SHIFT);
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_P:   // p 직각 P 원근
			perspectiveon = shift;
			break;
		case GLFW_KEY_M:   // m 솔리드 M 와이어
			wireon = shift;
			break;
		case GLFW_KEY_W:   // 위
			movey += 0.05f;
			break;
		case GLFW_KEY_S:   // 아래
			movey -= 0.05f;
			break;
		case GLFW_KEY_A:   // 왼쪽
			movex -= 0.05f;
			break;
		case GLFW_KEY_D:   // 오른쪽
			movex += 0.05f;
			break;
		case GLFW_KEY_EQUAL:      // + 앞으로
		case GLFW_KEY_KP_ADD:
			movez += 0.1f;
			break;
		case GLFW_KEY_MINUS:      // - 뒤로
		case GLFW_KEY_KP_SUBTRACT:
			movez -= 0.1f;
			break;
		case GLFW_KEY_1:   // 1 커짐 shift 1 작아짐
			if (shift) radiusscale -= 0.1f;
			else radiusscale += 0.1f;
			if (radiusscale < 0.5f) radiusscale = 0.5f;
			if (radiusscale > 1.6f) radiusscale = 1.6f;
			break;
		}
	}
	if (action == GLFW_PRESS)   // 켜고 끄는 키는 한 번만
	{
		switch (key)
		{
		case GLFW_KEY_2:   // 중심 구 확대 축소 시작 멈춤
			sunanim = !sunanim;
			break;
		case GLFW_KEY_3:   // 달 궤도 자전 시작 멈춤
			moonspin = !moonspin;
			break;
		}
	}
}
