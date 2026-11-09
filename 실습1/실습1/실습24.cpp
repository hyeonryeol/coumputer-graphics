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


// 공책 위치
int cubestart[10];  // 색깔별 육면체 위치
const int C_FLOOR = 0, C_BELT = 1, C_RAIL = 2, C_LEG = 3, C_STRIPE = 4, C_MARK = 5;
const int C_CAB = 6, C_GLASS = 7, C_WOOD = 8;
int boxstart[5];    // 상자 5개 위치

float boxcolor[5][3] = {
	{ 0.85f, 0.15f, 0.15f },   // 1번 빨강
	{ 0.15f, 0.35f, 0.85f },   // 2번 파랑
	{ 0.15f, 0.6f, 0.2f },     // 3번 초록
	{ 0.95f, 0.75f, 0.1f },    // 4번 노랑
	{ 0.8f, 0.45f, 0.1f },     // 5번 주황
};

// 벨트 경로 시작 S 코너 C 끝 E 높이 BELTY
const float BELTY = 0.6f;
const float SEG = 3.3f;          // 한 구간 길이
const float PATHLEN = SEG * 2;   // 전체 길이
float beltoffset = 0.0f;         // 벨트가 움직인 거리
bool beltmove = false;           // b 시작 정지

// 카메라
float camdist = 6.0f;            // + - 줌
float camangle = 45.0f;          // 좌우 화살표 회전
bool perspectiveon = true;       // p 원근 P 직각
bool firstperson = false;        // v 1인칭 3인칭
int viewbox = 4;                 // 1인칭으로 볼 상자 처음은 5번

// 트럭 벨트 앞 구간 왼쪽 +z를 바라봄
const float TRUCKX = -3.0f;
const float TRUCKHALF = 0.9f;    // 트럭 앞뒤 반 길이
const float FLOORHALF = 4.0f;    // 바닥 반 길이
float truckz = 0.0f;             // 트럭 위치
int truckdir = 1;                // 1 앞으로 -1 뒤로
bool truckmove = false;          // g 출발 정지

// 트럭에 실은 상자
bool loaded[5];                  // 실렸는지
int loadslot[5];                 // 짐칸 자리 번호
int loadcount = 0;               // 실은 수
float slotpos[6][2] = {          // 짐칸 자리 x z
	{ -0.2f, 0.05f }, { 0.2f, 0.05f },
	{ -0.2f, -0.3f }, { 0.2f, -0.3f },
	{ -0.2f, -0.65f }, { 0.2f, -0.65f },
};

void resetall()
{
	beltoffset = 0.0f;
	beltmove = false;
	camdist = 6.0f;
	camangle = 45.0f;
	perspectiveon = true;
	firstperson = false;
	viewbox = 4;
	truckz = 0.0f;
	truckdir = 1;
	truckmove = false;
	for (int i = 0; i < 5; i++)
		loaded[i] = false;
	loadcount = 0;
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

// 경로 위 거리 s의 위치 앞 구간은 -z로 가고 뒤 구간은 +x로 감
glm::vec3 pathpos(float s)
{
	if (s < SEG)
		return glm::vec3(-1.5f, BELTY, 1.8f - s);
	return glm::vec3(-1.5f + (s - SEG), BELTY, -1.5f);
}

// 경로 위 거리 s의 방향 코너 앞뒤 0.3 구간에서 0도에서 -90도로 천천히 돎
float pathyaw(float s)
{
	float a = SEG - 0.3f, b = SEG + 0.3f;
	if (s <= a) return 0.0f;
	if (s >= b) return -90.0f;
	return -90.0f * (s - a) / (b - a);
}

// 트럭 위치 행렬
glm::mat4 truckmatrix()
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(TRUCKX, 0.0f, truckz));
}

// 상자 i 위치 행렬 벨트 위 또는 짐칸
glm::mat4 boxmatrix(int i)
{
	if (loaded[i]) {
		int k = loadslot[i];
		return glm::translate(truckmatrix(), glm::vec3(slotpos[k][0], 0.57f, slotpos[k][1]));
	}
	float s = fmod(beltoffset + (4 - i) * 1.3f, PATHLEN);   // 1번이 맨 앞
	glm::mat4 box = glm::translate(glm::mat4(1.0f), pathpos(s) + glm::vec3(0.0f, 0.15f, 0.0f));
	return glm::rotate(box, glm::radians(pathyaw(s)), glm::vec3(0.0f, 1.0f, 0.0f));
}

// 상자 i 앞 방향 벨트 위는 -z 기준으로 돌고 짐칸은 트럭 앞
glm::vec3 boxforward(int i)
{
	if (loaded[i])
		return glm::vec3(0.0f, 0.0f, 1.0f);
	return glm::vec3(boxmatrix(i) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
}

// 트럭 이동 바닥 끝이면 멈추고 다음엔 반대로
void movetruck()
{
	if (!truckmove)
		return;
	truckz += 0.02f * truckdir;
	if (truckz + TRUCKHALF >= FLOORHALF || truckz - TRUCKHALF <= -FLOORHALF) {
		truckz = glm::clamp(truckz, -FLOORHALF + TRUCKHALF, FLOORHALF - TRUCKHALF);
		truckmove = false;
		truckdir = -truckdir;
	}
}

// 트럭 그리기 앞이 +z
void drawtruck()
{
	glm::mat4 t = truckmatrix();
	// 바닥 틀
	drawcube(glm::translate(t, glm::vec3(0.0f, 0.3f, 0.0f)), glm::vec3(1.0f, 0.12f, 1.8f), cubestart[C_LEG]);
	// 바퀴 4개
	float wheel[4][2] = { { -0.52f, 0.55f }, { 0.52f, 0.55f }, { -0.52f, -0.55f }, { 0.52f, -0.55f } };
	for (int k = 0; k < 4; k++)
		drawcube(glm::translate(t, glm::vec3(wheel[k][0], 0.15f, wheel[k][1])), glm::vec3(0.14f, 0.3f, 0.3f), cubestart[C_LEG]);
	// 운전석과 앞 유리
	drawcube(glm::translate(t, glm::vec3(0.0f, 0.66f, 0.6f)), glm::vec3(1.0f, 0.6f, 0.55f), cubestart[C_CAB]);
	drawcube(glm::translate(t, glm::vec3(0.0f, 0.75f, 0.88f)), glm::vec3(0.8f, 0.25f, 0.02f), cubestart[C_GLASS]);
	// 짐칸 바닥 양옆 뒤 벽
	drawcube(glm::translate(t, glm::vec3(0.0f, 0.39f, -0.3f)), glm::vec3(1.0f, 0.06f, 1.15f), cubestart[C_WOOD]);
	drawcube(glm::translate(t, glm::vec3(-0.48f, 0.57f, -0.3f)), glm::vec3(0.04f, 0.3f, 1.15f), cubestart[C_WOOD]);
	drawcube(glm::translate(t, glm::vec3(0.48f, 0.57f, -0.3f)), glm::vec3(0.04f, 0.3f, 1.15f), cubestart[C_WOOD]);
	drawcube(glm::translate(t, glm::vec3(0.0f, 0.57f, -0.86f)), glm::vec3(1.0f, 0.3f, 0.04f), cubestart[C_WOOD]);
}

// 그리기
void drawScene()
{
	if (beltmove)
		beltoffset += 0.01f;
	movetruck();

	glClearColor(0.55f, 0.75f, 0.95f, 1.0f);   // 하늘
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	n = 0;
	float cc[9][3] = {
		{ 0.3f, 0.35f, 0.3f },    // 바닥
		{ 0.45f, 0.45f, 0.48f },  // 벨트 판
		{ 0.85f, 0.7f, 0.1f },    // 난간 노랑
		{ 0.08f, 0.08f, 0.08f },  // 다리 검정
		{ 0.2f, 0.2f, 0.22f },    // 줄무늬
		{ 0.05f, 0.05f, 0.05f },  // 번호 점
		{ 0.62f, 0.62f, 0.66f },  // 운전석 회색
		{ 0.2f, 0.35f, 0.5f },    // 앞 유리
		{ 0.55f, 0.38f, 0.2f },   // 짐칸 나무
	};
	for (int i = 0; i < 9; i++) {
		cubestart[i] = n;
		addcube(cc[i][0], cc[i][1], cc[i][2]);
	}
	for (int i = 0; i < 5; i++) {
		boxstart[i] = n;
		addcube(boxcolor[i][0], boxcolor[i][1], boxcolor[i][2]);
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
	float aspect = (float)width / height;
	if (perspectiveon || firstperson)
		projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
	else
	{
		float h = camdist * 0.45f;   // 줌에 따라 직각 범위도 바뀜
		projection = glm::ortho(-h * aspect, h * aspect, -h, h, 0.1f, 100.0f);
	}
	unsigned int projLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));

	// 3인칭 쿼터뷰 카메라 중심을 비스듬히 위에서 봄
	float a = glm::radians(camangle);
	glm::vec3 eye = glm::vec3(camdist * sin(a), camdist * 0.8f, camdist * cos(a));
	glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	// 1인칭 상자 윗면 바로 위에서 상자 앞 방향을 봄 짐칸에서는 운전석에 가려서 지붕 위 높이로
	if (firstperson) {
		float eyeh = loaded[viewbox] ? 0.7f : 0.25f;
		glm::vec3 boxeye = glm::vec3(boxmatrix(viewbox) * glm::vec4(0.0f, eyeh, 0.0f, 1.0f));
		view = glm::lookAt(boxeye, boxeye + boxforward(viewbox), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 I = glm::mat4(1.0f);

	// 바닥
	drawcube(glm::translate(I, glm::vec3(0.0f, -0.01f, 0.0f)), glm::vec3(8.0f, 0.02f, 8.0f), cubestart[C_FLOOR]);

	// 벨트 판 앞 구간 뒤 구간
	drawcube(glm::translate(I, glm::vec3(-1.5f, BELTY - 0.05f, 0.0f)), glm::vec3(0.6f, 0.1f, 3.6f), cubestart[C_BELT]);
	drawcube(glm::translate(I, glm::vec3(0.3f, BELTY - 0.05f, -1.5f)), glm::vec3(3.0f, 0.1f, 0.6f), cubestart[C_BELT]);

	// 난간 코너 안쪽은 비움
	drawcube(glm::translate(I, glm::vec3(-1.82f, BELTY + 0.02f, 0.0f)), glm::vec3(0.04f, 0.08f, 3.68f), cubestart[C_RAIL]);
	drawcube(glm::translate(I, glm::vec3(-1.18f, BELTY + 0.02f, 0.3f)), glm::vec3(0.04f, 0.08f, 3.0f), cubestart[C_RAIL]);
	drawcube(glm::translate(I, glm::vec3(0.0f, BELTY + 0.02f, -1.82f)), glm::vec3(3.64f, 0.08f, 0.04f), cubestart[C_RAIL]);
	drawcube(glm::translate(I, glm::vec3(0.3f, BELTY + 0.02f, -1.18f)), glm::vec3(3.0f, 0.08f, 0.04f), cubestart[C_RAIL]);

	// 다리
	float legs[8][2] = { { -1.75f, 1.6f }, { -1.25f, 1.6f }, { -1.75f, 0.0f }, { -1.25f, 0.0f },
		{ -1.75f, -1.75f }, { 0.2f, -1.75f }, { 0.2f, -1.25f }, { 1.6f, -1.5f } };
	for (int i = 0; i < 8; i++)
		drawcube(glm::translate(I, glm::vec3(legs[i][0], (BELTY - 0.1f) / 2.0f, legs[i][1])), glm::vec3(0.06f, BELTY - 0.1f, 0.06f), cubestart[C_LEG]);

	// 코너 이음매 바깥 꼭짓점에서 안쪽 꼭짓점까지 사선
	glm::mat4 seam = glm::translate(I, glm::vec3(-1.5f, BELTY + 0.006f, -1.5f));
	seam = glm::rotate(seam, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	drawcube(seam, glm::vec3(0.82f, 0.012f, 0.03f), cubestart[C_STRIPE]);

	// 줄무늬 벨트와 같이 흘러감 0.3 간격
	// 줄무늬 경로 앞 구간 0 ~ 3  코너 3 ~ 3.6  뒤 구간 3.6 ~ 6.6
	float step = 0.3f;
	for (int k = 0; k < 22; k++) {
		float s = fmod(k * step + beltoffset, PATHLEN);
		if (s < 3.0f) {                        // 앞 구간 -z로
			drawcube(glm::translate(I, glm::vec3(-1.5f, BELTY + 0.005f, 1.8f - s)), glm::vec3(0.56f, 0.012f, 0.04f), cubestart[C_STRIPE]);
		}
		else if (s < 3.6f) {                   // 코너 안쪽 꼭짓점 기준 0도에서 90도로 부채꼴 회전 45도일 때 사선
			float th = 90.0f * (s - 3.0f) / 0.6f;
			float c = cos(glm::radians(th)), sn = sin(glm::radians(th));
			float len = 0.56f / glm::max(c, sn);   // 안쪽 꼭짓점에서 바깥 테두리까지
			glm::mat4 m = glm::translate(I, glm::vec3(-1.2f, BELTY + 0.005f, -1.2f));
			m = glm::rotate(m, glm::radians(-th), glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::translate(m, glm::vec3(-len / 2.0f, 0.0f, 0.0f));
			drawcube(m, glm::vec3(len, 0.012f, 0.04f), cubestart[C_STRIPE]);
		}
		else {                                 // 뒤 구간 +x로
			drawcube(glm::translate(I, glm::vec3(-1.2f + (s - 3.6f), BELTY + 0.005f, -1.5f)), glm::vec3(0.04f, 0.012f, 0.56f), cubestart[C_STRIPE]);
		}
	}

	drawtruck();

	// 택배 상자 5개 1.3 간격으로 경로를 돌고 끝나면 처음부터 실린 상자는 짐칸에
	for (int i = 0; i < 5; i++) {
		glm::mat4 box = boxmatrix(i);
		// 1인칭으로 보는 상자는 안 그림 자기 몸이 화면을 가려서
		if (firstperson && i == viewbox)
			continue;
		drawcube(box, glm::vec3(0.3f), boxstart[i]);

		// 번호 윗면에 점 i+1개
		for (int k = 0; k <= i; k++) {
			float x = (k - i / 2.0f) * 0.055f;
			drawcube(glm::translate(box, glm::vec3(x, 0.155f, 0.0f)), glm::vec3(0.035f, 0.01f, 0.035f), cubestart[C_MARK]);
		}
	}
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_EQUAL:      // + 줌 인 카메라 가까이
		case GLFW_KEY_KP_ADD:
			camdist -= 0.2f;
			break;
		case GLFW_KEY_MINUS:      // - 줌 아웃 카메라 멀리
		case GLFW_KEY_KP_SUBTRACT:
			camdist += 0.2f;
			break;
		case GLFW_KEY_LEFT:       // 위에서 볼 때 시계 방향
			camangle -= 3.0f;
			break;
		case GLFW_KEY_RIGHT:      // 반시계 방향
			camangle += 3.0f;
			break;
		}
		camdist = glm::clamp(camdist, 2.0f, 12.0f);
	}
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_B:          // 벨트 시작 정지
			beltmove = !beltmove;
			break;
		case GLFW_KEY_P:          // p 원근 P 직각
			perspectiveon = !shift;
			break;
		case GLFW_KEY_R:          // 리셋
			resetall();
			break;
		case GLFW_KEY_1:          // 1 ~ 5 그 상자 트럭에 싣기
		case GLFW_KEY_2:
		case GLFW_KEY_3:
		case GLFW_KEY_4:
		case GLFW_KEY_5:
		{
			int i = key - GLFW_KEY_1;
			if (!loaded[i]) {
				loaded[i] = true;
				loadslot[i] = loadcount;
				loadcount++;
			}
			break;
		}
		case GLFW_KEY_V:          // 1인칭 3인칭
			firstperson = !firstperson;
			break;
		case GLFW_KEY_N:          // n 다음 상자 N 이전 상자
			if (shift)
				viewbox = (viewbox + 4) % 5;
			else
				viewbox = (viewbox + 1) % 5;
			break;
		case GLFW_KEY_G:          // 트럭 출발 정지
			truckmove = !truckmove;
			break;
		}
	}
}
