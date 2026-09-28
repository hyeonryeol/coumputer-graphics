#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

// 사용자 정의 함수
std::string filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
void drawScene();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double x, double y);
// 필요한 변수
GLint width = 700, height = 700;   // 정사각형 창
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];

// 꼭짓점 공책
GLfloat position[500][3];
GLfloat color[500][3];
int n = 0;   // 공책에 적힌 꼭짓점 수

// 도형 종류
const int square = 0;   // 작은 사각형
const int triangle = 1;     // 정삼각형
const int righttri = 2;    // 직각삼각형

// 도형 크기
const float squaresize = 0.12f;            // 사각형 한 변
const float trisize = 0.16f;            // 정삼각형 한 변
const float triheight = trisize * 0.866f;    // 정삼각형 높이
const float rightw = 0.14f;               // 직각삼각형 밑변
const float righth = 0.34f;               // 직각삼각형 높이

// 가운데 나누는 선 위치
const float linex = 0.2f;

struct Shape {
	int type;          // 도형 종류
	int turn;          // 돌린 횟수
	float x, y;       // 중심
	float r, g, b;
	int slot;         // 들어간 칸 번호
	int group;        // 모양판 번호
	bool done;        // 완성됨
};

Shape board[20];      // 모양판 칸
int boardcount = 0;
Shape shape[30];     // 왼쪽 조각
int shapecount = 0;
int select = -1;
// 칸마다 공책 시작 줄과 줄 수
int boardstart[20], boardnum[20];
int shapestart = 0, shapenum = 0;

float lastx, lasty;
float offsetx = 0.0f, offsety = 0.0f;   // 잡은 곳과 중심 차이
bool mouseclick = false;
int nowgroup = 0;   // 지금 만드는 모양판 번호

// 빈 칸인지 확인
bool isempty(int j)
{
	for (int i = 0; i < shapecount; ++i)
	{
		if (shape[i].slot == j)
			return false;
	}
	return true;
}

// 모양판 완성 확인
bool iscomplete(int g)
{
	for (int j = 0; j < boardcount; ++j)
	{
		if (board[j].group == g && isempty(j))
			return false;
	}
	return true;
}

// 완성된 모양판 표시
void markdone(int g)
{
	for (int i = 0; i < shapecount; ++i)
	{
		if (shape[i].slot != -1 && board[shape[i].slot].group == g)
			shape[i].done = true;
	}
	// 완성된 테두리는 초록
	for (int j = 0; j < boardcount; ++j)
	{
		if (board[j].group == g)
		{
			board[j].r = 0.1f; board[j].g = 0.7f; board[j].b = 0.2f;
		}
	}
}

// 맞는 칸에 붙이기
void snaptoboard(int i)
{
	for (int j = 0; j < boardcount; ++j)
	{
		if (shape[i].type != board[j].type) continue;
		if (shape[i].turn != board[j].turn) continue;
		if (fabs(shape[i].x - board[j].x) > 0.05f) continue;
		if (fabs(shape[i].y - board[j].y) > 0.05f) continue;
		if (!isempty(j)) continue;

		shape[i].x = board[j].x;
		shape[i].y = board[j].y;
		shape[i].slot = j;
		std::cout << "철커덕" << std::endl;

		// 다 찼으면 완성
		if (iscomplete(board[j].group))
			markdone(board[j].group);
		break;
	}
}

// 꼭짓점 구하기
int getpoints(const Shape& p, float px[4], float py[4])
{
	int count = 0;
	if (p.type == square)
	{
		float h = squaresize / 2;
		px[0] = -h; py[0] = -h;
		px[1] = h;  py[1] = -h;
		px[2] = h;  py[2] = h;
		px[3] = -h; py[3] = h;
		count = 4;
	}
	else if (p.type == triangle)
	{
		// 꼭지가 위
		px[0] = -trisize / 2; py[0] = -triheight / 2;
		px[1] = trisize / 2;  py[1] = -triheight / 2;
		px[2] = 0.0f;         py[2] = triheight / 2;
		count = 3;
	}
	else
	{
		// 직각이 왼쪽 아래
		px[0] = -rightw / 2; py[0] = -righth / 2;
		px[1] = rightw / 2;  py[1] = -righth / 2;
		px[2] = -rightw / 2; py[2] = righth / 2;
		count = 3;
	}

	// 90도씩 돌리기
	for (int t = 0; t < p.turn; ++t)
	{
		for (int k = 0; k < count; ++k)
		{
			float ox = px[k];
			px[k] = -py[k];
			py[k] = ox;
		}
	}

	// 중심으로 옮기기
	for (int k = 0; k < count; ++k)
	{
		px[k] += p.x;
		py[k] += p.y;
	}
	return count;
}

// 가로 세로 절반 구하기
void gethalfsize(const Shape& p, float& halfw, float& halfh)
{
	Shape zero = p;
	zero.x = 0.0f;
	zero.y = 0.0f;
	float px[4], py[4];
	int count = getpoints(zero, px, py);
	halfw = 0.0f;
	halfh = 0.0f;
	for (int k = 0; k < count; ++k)
	{
		if (fabs(px[k]) > halfw) halfw = fabs(px[k]);
		if (fabs(py[k]) > halfh) halfh = fabs(py[k]);
	}
}

float random01()
{
	return rand() / (float)RAND_MAX;
}

void addboard(int type, int turn, float x, float y)
{
	Shape& s = board[boardcount];
	s.type = type;
	s.turn = turn;
	s.x = x;
	s.y = y;
	s.r = 0.2f; s.g = 0.3f; s.b = 0.6f;
	s.slot = -1;
	s.group = nowgroup;
	s.done = false;
	boardcount++;
}

// 모양판 만들기
void makeboard()
{
	boardcount = 0;

	// 사각형 4개
	nowgroup = 0;
	float bx = 0.45f, by = 0.68f, h = squaresize / 2;
	addboard(square, 0, bx - h, by + h);
	addboard(square, 0, bx + h, by + h);
	addboard(square, 0, bx - h, by - h);
	addboard(square, 0, bx + h, by - h);

	// 나비 모양
	nowgroup = 1;
	bx = 0.80f; by = 0.68f;
	addboard(triangle, 2, bx, by + triheight / 2);   // 위쪽
	addboard(triangle, 0, bx, by - triheight / 2);   // 아래쪽
	addboard(triangle, 3, bx - triheight / 2, by);   // 왼쪽
	addboard(triangle, 1, bx + triheight / 2, by);   // 오른쪽

	// 대각선 직사각형
	nowgroup = 2;
	bx = 0.45f; by = 0.05f;
	addboard(righttri, 0, bx, by);   // 직각이 왼쪽 아래
	addboard(righttri, 2, bx, by);   // 직각이 오른쪽 위

	// 집 모양
	nowgroup = 3;
	bx = 0.80f; by = 0.02f;
	addboard(square, 0, bx, by);
	addboard(triangle, 0, bx, by + squaresize / 2 + triheight / 2);

	// 마름모
	nowgroup = 4;
	bx = 0.62f; by = -0.60f;
	addboard(triangle, 0, bx, by + triheight / 2);
	addboard(triangle, 2, bx, by - triheight / 2);
}

// 안 겹치게 랜덤 배치
void setrandompos(Shape& p)
{
	float halfw, halfh;
	gethalfsize(p, halfw, halfh);
	float left = -0.95f + halfw, right = linex - 0.05f - halfw;
	float bottom = -0.95f + halfh, top = 0.95f - halfh;

	for (int trycount = 0; trycount < 200; ++trycount)
	{
		p.x = left + random01() * (right - left);
		p.y = bottom + random01() * (top - bottom);

		bool overlap = false;
		for (int i = 0; i < shapecount; ++i)
		{
			float otherw, otherh;
			gethalfsize(shape[i], otherw, otherh);
			if (fabs(p.x - shape[i].x) < halfw + otherw + 0.02f && fabs(p.y - shape[i].y) < halfh + otherh + 0.02f)
			{
				overlap = true;
				break;
			}
		}
		if (!overlap)
			return;
	}
}

// 왼쪽 조각 만들기
void makeshapes()
{
	shapecount = 0;
	int extracount = 2 + rand() % 4;   // 추가 조각 수

	for (int i = 0; i < boardcount + extracount; ++i)
	{
		Shape p;
		if (i < boardcount)
		{
			p.type = board[i].type;
			p.turn = board[i].turn;
		}
		else
		{
			p.type = rand() % 3;
			p.turn = rand() % 4;
		}
		p.r = 0.15f + random01() * 0.7f;
		p.g = 0.15f + random01() * 0.7f;
		p.b = 0.15f + random01() * 0.7f;
		p.slot = -1;
		p.group = -1;
		p.done = false;
		setrandompos(p);
		shape[shapecount] = p;
		shapecount++;
	}

	// 순서 섞기
	for (int i = shapecount - 1; i > 0; --i)
	{
		int j = rand() % (i + 1);
		Shape t = shape[i];
		shape[i] = shape[j];
		shape[j] = t;
	}
}

// 공책 한 줄 적기
void addvertex(float x, float y, float r, float g, float b)
{
	position[n][0] = x;
	position[n][1] = y;
	position[n][2] = 0.0f;
	color[n][0] = r;
	color[n][1] = g;
	color[n][2] = b;
	n++;
}

// 공책 채우기
void makevertax()
{
	n = 0;

	// 나누는 선
	addvertex(linex, -1.0f, 0.6f, 0.6f, 0.6f);
	addvertex(linex, 1.0f, 0.6f, 0.6f, 0.6f);

	// 모양판 테두리
	for (int i = 0; i < boardcount; ++i)
	{
		float px[4], py[4];
		int count = getpoints(board[i], px, py);
		boardstart[i] = n;
		for (int k = 0; k < count; ++k)
			addvertex(px[k], py[k], board[i].r, board[i].g, board[i].b);
		boardnum[i] = count;
	}

	// 조각 사각형은 삼각형 2개
	shapestart = n;
	for (int i = 0; i < shapecount; ++i)
	{
		Shape& p = shape[i];
		float px[4], py[4];
		int count = getpoints(p, px, py);
		addvertex(px[0], py[0], p.r, p.g, p.b);
		addvertex(px[1], py[1], p.r, p.g, p.b);
		addvertex(px[2], py[2], p.r, p.g, p.b);
		if (count == 4)
		{
			addvertex(px[0], py[0], p.r, p.g, p.b);
			addvertex(px[2], py[2], p.r, p.g, p.b);
			addvertex(px[3], py[3], p.r, p.g, p.b);
		}
	}
	shapenum = n - shapestart;
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
	GLFWwindow* window = glfwCreateWindow(width, height, "실습10", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "윈도우 생성 실패" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

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
	srand((unsigned int)time(NULL));
	makeboard();
	makeshapes();

	// 세이더 읽어서 세이더 프로그램 만들기
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	// VAO VBO 만들기
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
	std::string vertexSource = filetobuf("vertex.glsl");
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

// VAO VBO 만들기
void InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo);

	// 0번 VBO 좌표
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 1번 VBO 색
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

// 그리기
void drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 공책 채우기
	makevertax();

	// 공책 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 나누는 선
	glDrawArrays(GL_LINES, 0, 2);

	// 모양판 테두리
	for (int i = 0; i < boardcount; ++i)
		glDrawArrays(GL_LINE_LOOP, boardstart[i], boardnum[i]);

	// 조각
	glDrawArrays(GL_TRIANGLES, shapestart, shapenum);
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_R:
			makeboard();
			makeshapes();
			select = -1;
			mouseclick = false;
			break;
		}
	}
}

// 마우스 버튼 콜백
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		// GL 좌표로 변환
		float gx = (float)(x / width * 2.0 - 1.0);
		float gy = (float)(1.0 - y / height * 2.0);


		for (int i = 0; i < shapecount; ++i)
		{
			// 완성된 도형은 못 잡음
			if (shape[i].done) continue;

			float halfw, halfh;
			gethalfsize(shape[i], halfw, halfh);   // 가로 세로 절반
			if (gx >= shape[i].x - halfw && gx <= shape[i].x + halfw &&
				gy >= shape[i].y - halfh && gy <= shape[i].y + halfh)
			{
				select = i;
				std::cout << i << std::endl;
				mouseclick = true;
				offsetx = gx - shape[i].x;
				offsety = gy - shape[i].y;
			}
		}
		// 잡으면 칸 비우기
		if (select != -1)
			shape[select].slot = -1;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// 놓으면 칸에 붙이기
		if (select != -1)
			snaptoboard(select);
		select = -1;
		mouseclick = false;
	}
}

void CursorPosCallback(GLFWwindow* window, double x, double y)
{
	if (mouseclick == false) return;   // 드래그 중에만

	float hx = (float)(x / width * 2.0 - 1.0);
	float hy = (float)(1.0 - y / height * 2.0);
	lastx = hx;
	lasty = hy;

	
	shape[select].x = hx - offsetx;
	shape[select].y = hy - offsety;
}
