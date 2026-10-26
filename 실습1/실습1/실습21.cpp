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
GLfloat position[5000][3];
GLfloat color[5000][3];
int n = 0;   // 공책에 적힌 꼭짓점 수

#include <sstream>   // istringstream
#include <vector>    // vector

struct Vertex { float x, y, z; };   // 점 하나
struct Face { int a, b, c; };       // 삼각형 하나 = 점 번호 3개
std::vector<Vertex> spherev;        // 구 점 목록
std::vector<Face> spheref;          // 구 삼각형 목록

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double x, double y);

// 방 크기 x y는 -1 ~ 1 z는 -1.5 ~ 1.5 앞면은 뚫림
const float RX = 1.0f, RY = 1.0f, RZ = 1.5f;

// 공책 위치
int roomstart;      // 방 육면체 면마다 색 다름
int boxstart;       // 바닥 육면체
int ballstart;      // 공
int spherecount;

// 공 최대 5개
struct Ball {
	glm::vec3 p;    // 방 안 위치
	glm::vec3 v;    // 한 프레임 이동량
	bool alive;
};
Ball ball[5];
int ballcount = 0;
const float BALLR = 0.12f;   // 공 반지름 구 obj가 0.3이라 0.4배

// 바닥 육면체 3개 방 안 x y 위치만 움직임 z는 고정
struct Box {
	float x, y, z;
	float vx, vy;
	float size;
	bool alive;
};
Box box[3];

// 장면 변환
float scenez = 0.0f;       // z Z 앞뒤 이동
float sceneangle = 0.0f;   // y Y 화면 y축 회전
float roomangle = 0.0f;    // 마우스 드래그 z축 회전
bool dragging = false;
double lastmousex = 0.0;

// 바닥 열기 o
bool flooropen = false;
float floorangle = 0.0f;

float randf()
{
	return rand() / (float)RAND_MAX;
}

// 공 하나 만들기 방 가운데서 랜덤 방향
void addball()
{
	if (ballcount >= 5) return;
	Ball& b = ball[ballcount];
	b.p = glm::vec3(0.0f, 0.2f, 0.0f);
	b.v = glm::vec3(randf() - 0.5f, randf() - 0.5f, randf() - 0.5f);
	b.v = glm::normalize(b.v) * 0.02f;
	b.alive = true;
	ballcount++;
}

void resetall()
{
	scenez = 0.0f;
	sceneangle = 0.0f;
	roomangle = 0.0f;
	flooropen = false;
	floorangle = 0.0f;
	ballcount = 0;
	addball();
	float sizes[3] = { 0.25f, 0.35f, 0.45f };
	float zs[3] = { -0.9f, 0.0f, 0.9f };
	for (int i = 0; i < 3; i++) {
		box[i].size = sizes[i];
		box[i].x = 0.0f;
		box[i].y = -RY + sizes[i] / 2.0f;   // 바닥에 붙음
		box[i].z = zs[i];
		box[i].vx = box[i].vy = 0.0f;
		box[i].alive = true;
	}
}

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
	srand((unsigned int)time(NULL));
	resetall();
	loadobj("sphere.obj", spherev, spheref);   // 반지름 0.3 구

	// 세이더 읽어서 세이더 프로그램 만들기
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	// VAO, VBO 만들기
	InitBuffer();

	// 콜백 등록
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);

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

// 구 하나 공책에 적기
void addsphere(float r, float g, float b)
{
	for (int k = 0; k < spheref.size(); k++) {
		int idx[3] = { spheref[k].a, spheref[k].b, spheref[k].c };
		for (int j = 0; j < 3; j++)
			addvertex(spherev[idx[j]].x, spherev[idx[j]].y, spherev[idx[j]].z, r, g, b);
	}
}

// 움직임 위치는 모두 방 기준 좌표
void update()
{
	// 바닥 열고 닫기 뒤쪽 모서리 기준으로 90도
	if (flooropen && floorangle < 90.0f) floorangle += 2.0f;
	if (!flooropen && floorangle > 0.0f) floorangle -= 2.0f;
	bool floorgone = floorangle > 30.0f;   // 이만큼 열리면 바닥이 없는 셈

	// 공 벽에 닿으면 그 축 방향만 반대로
	for (int i = 0; i < ballcount; i++) {
		Ball& b = ball[i];
		if (!b.alive) continue;
		b.p += b.v;
		if (b.p.x > RX - BALLR || b.p.x < -RX + BALLR) b.v.x = -b.v.x;
		if (b.p.y > RY - BALLR) b.v.y = -b.v.y;
		if (b.p.y < -RY + BALLR && !floorgone) b.v.y = -b.v.y;
		if (b.p.z > RZ - BALLR || b.p.z < -RZ + BALLR) b.v.z = -b.v.z;
		b.p.x = glm::clamp(b.p.x, -RX + BALLR, RX - BALLR);
		b.p.z = glm::clamp(b.p.z, -RZ + BALLR, RZ - BALLR);
		if (!floorgone) b.p.y = glm::clamp(b.p.y, -RY + BALLR, RY - BALLR);
		if (b.p.y < -4.0f) b.alive = false;   // 떨어져서 사라짐
	}

	// 바닥 육면체 화면 아래쪽 중력을 방 좌표로 바꿔서 미끄러짐
	float a = glm::radians(roomangle);
	float gx = -sin(a) * 0.003f;
	float gy = -cos(a) * 0.003f;
	for (int i = 0; i < 3; i++) {
		Box& c = box[i];
		if (!c.alive) continue;
		c.vx += gx;
		c.vy += gy;
		c.x += c.vx;
		c.y += c.vy;
		float h = c.size / 2.0f;
		if (c.x > RX - h) { c.x = RX - h; c.vx = 0.0f; }   // 벽에 닿으면 멈춤
		if (c.x < -RX + h) { c.x = -RX + h; c.vx = 0.0f; }
		if (c.y > RY - h) { c.y = RY - h; c.vy = 0.0f; }
		if (c.y < -RY + h && !floorgone) { c.y = -RY + h; c.vy = 0.0f; }
		if (c.y < -4.0f) c.alive = false;
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
	float fc[6][3] = {
		{ 0.0f, 0.0f, 0.0f },     // 앞 안 그림
		{ 0.75f, 0.75f, 0.75f },  // 뒤
		{ 0.45f, 0.45f, 0.45f },  // 오른쪽
		{ 0.55f, 0.55f, 0.55f },  // 왼쪽
		{ 0.35f, 0.35f, 0.35f },  // 위
		{ 0.62f, 0.62f, 0.62f },  // 아래
	};
	roomstart = n;
	addcubefaces(fc);
	boxstart = n;
	addcube(0.9f, 0.1f, 0.1f);
	ballstart = n;
	addsphere(0.2f, 0.3f, 1.0f);
	spherecount = n - ballstart;

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

	// 카메라 앞에서 방 안을 봄
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	// 장면 z 이동 y축 회전 그 안에서 방이 z축 회전
	glm::mat4 scene = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, scenez));
	scene = glm::rotate(scene, glm::radians(sceneangle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 room = glm::rotate(scene, glm::radians(roomangle), glm::vec3(0.0f, 0.0f, 1.0f));

	// 방은 앞면을 컬링해서 안쪽만 보임
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glm::mat4 roomscale = glm::scale(room, glm::vec3(RX * 2, RY * 2, RZ * 2));
	for (int k = 1; k <= 4; k++)   // 뒤 오른 왼 위 앞면 0은 안 그림
		drawwith(roomscale, GL_TRIANGLES, roomstart + k * 6, 6);

	// 바닥 뒤쪽 모서리 기준으로 아래로 열림
	glm::mat4 fl = glm::translate(room, glm::vec3(0.0f, -RY, -RZ));
	fl = glm::rotate(fl, glm::radians(floorangle), glm::vec3(1.0f, 0.0f, 0.0f));
	fl = glm::translate(fl, glm::vec3(0.0f, RY, RZ));
	drawwith(glm::scale(fl, glm::vec3(RX * 2, RY * 2, RZ * 2)), GL_TRIANGLES, roomstart + 5 * 6, 6);

	// 안에 있는 물체는 보통처럼 뒷면 컬링
	glCullFace(GL_BACK);
	for (int i = 0; i < 3; i++) {
		if (!box[i].alive) continue;
		glm::mat4 m = glm::translate(room, glm::vec3(box[i].x, box[i].y, box[i].z));
		drawcube(m, glm::vec3(box[i].size), boxstart);
	}
	for (int i = 0; i < ballcount; i++) {
		if (!ball[i].alive) continue;
		glm::mat4 m = glm::translate(room, ball[i].p);
		drawwith(glm::scale(m, glm::vec3(BALLR / 0.3f)), GL_TRIANGLES, ballstart, spherecount);
	}
	glDisable(GL_CULL_FACE);
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_Z:   // z 양 Z 음 방향 이동
			scenez += shift ? -0.1f : 0.1f;
			break;
		case GLFW_KEY_Y:   // y 시계 Y 반시계 화면 y축 회전
			sceneangle += shift ? 3.0f : -3.0f;
			break;
		}
	}
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_B:   // 공 하나 추가 최대 5개
			addball();
			break;
		case GLFW_KEY_O:   // 바닥 열기 닫기
			flooropen = !flooropen;
			break;
		case GLFW_KEY_R:   // 처음 상태로
			resetall();
			break;
		}
	}
}

// 마우스 버튼 콜백 누르는 동안만 드래그
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			double y;
			glfwGetCursorPos(window, &lastmousex, &y);
			dragging = true;
		}
		else if (action == GLFW_RELEASE)
			dragging = false;
	}
}

// 마우스 이동 콜백 왼쪽으로 끌면 왼쪽 반시계로 회전
void CursorPosCallback(GLFWwindow* window, double x, double y)
{
	if (!dragging) return;
	roomangle += (float)(lastmousex - x) * 0.3f;
	lastmousex = x;
}
