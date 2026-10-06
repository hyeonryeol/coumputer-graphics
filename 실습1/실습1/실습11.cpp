#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

// 사용자 정의 함수
std::string filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
void drawScene();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
// 필요한 변수
GLint width = 800, height = 600;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];
bool move = false;
int dx = 1;    // 가로 방향 1 오른쪽 -1 왼쪽
int dy = -1;   // 세로 방향 -1 아래 1 위
float speed = 0.2f;   // 한 칸 가는 시간 초
double lasttime = 0.0;
// 충돌 효과
bool hit = false;
double hittime = 0.0;      // 부딪힌 시각
float hitx, hity;          // 부딪힌 칸 중심
float effecttime = 0.5f;   // 효과 보이는 시간 초
int effectstart = 0;       // 공책에서 효과 시작 줄
// 꼭짓점 공책
GLfloat position[300][3];
GLfloat color[300][3];
int n = 0;   // 공책에 적힌 꼭짓점 수
struct Shape {
	float r, g, b;
	float size;
	float x, y;
	int col, row;
	int type;
};
Shape shape[50];
int shapecount = 0;
void makeLine()
{
	// 가로선 왼쪽 끝, 오른쪽 끝, 세로선 아래 끝, 위 끝
	n = 0;

	for (float boardx = -1.0f; boardx <= 1.0f; boardx += 0.1f)
	{
		position[n][0] = boardx;
		position[n][1] = -1.0f;
		position[n][2] = 0.0f;
		n++;
		position[n][0] = boardx;
		position[n][1] = 1.0f;
		position[n][2] = 0.0f;
		n++;
	}

	for (float boardy = -1.0f; boardy <= 1.0f; boardy += 0.1f)
	{
		position[n][0] = -1.0f;
		position[n][1] = boardy;
		position[n][2] = 0.0f;
		n++;
		position[n][0] = 1.0f;
		position[n][1] = boardy;
		position[n][2] = 0.0f;
		n++;
	}
}
float cell = 0.1f;
void makeplayer()
{
	shape[0].type = 2;
	shape[0].col = 0;
	shape[0].row = 19;
	shape[0].x = -1.0f + (shape[0].col + 0.5f) * cell;
	shape[0].y = -1.0f + (shape[0].row + 0.5f) * cell;
	shape[0].r = rand() / (float)RAND_MAX;
	shape[0].g = rand() / (float)RAND_MAX;
	shape[0].b = rand() / (float)RAND_MAX;
	shape[0].size = 0.02f;
}
// (c, r) 칸이 이미 쓰였는지 (플레이어 칸 + 먼저 놓인 도형들)
bool taken(int idx, int c, int r)
{
	if (c == 0 && r == 19) return true;   // 플레이어 칸 예약
	for (int i = 0; i < idx; ++i)
		if (shape[i].col == c && shape[i].row == r) return true;
	return false;
}
void makeshape(int idx)
{
	shape[idx].type = rand() % 3;
	// 빈 칸이 나올 때까지 다시 뽑아 겹치지 않게
	do {
		shape[idx].col = rand() % 20;
		shape[idx].row = rand() % 20;
	} while (taken(idx, shape[idx].col, shape[idx].row));

	shape[idx].x = -1.0f + (shape[idx].col + 0.5f) * cell;
	shape[idx].y = -1.0f + (shape[idx].row + 0.5f) * cell;
	shape[idx].r = rand() / (float)RAND_MAX;
	shape[idx].g = rand() / (float)RAND_MAX;
	shape[idx].b = rand() / (float)RAND_MAX;
	shape[idx].size = 0.02f + rand() / (float)RAND_MAX * 0.025f;
}
// 충돌 검사
void checkhit()
{
	for (int i = 1; i < shapecount; ++i)
	{
		if (shape[0].col == shape[i].col && shape[0].row == shape[i].row)
		{
			std::cerr << "충돌" << std::endl;

			// 모양 교환
			int t = shape[0].type;
			shape[0].type = shape[i].type;
			shape[i].type = t;

			// 색 교환
			float r = shape[0].r, g = shape[0].g, b = shape[0].b;
			shape[0].r = shape[i].r;
			shape[0].g = shape[i].g;
			shape[0].b = shape[i].b;
			shape[i].r = r;
			shape[i].g = g;
			shape[i].b = b;

			// 효과 시작
			hit = true;
			hittime = glfwGetTime();
			hitx = shape[0].x;
			hity = shape[0].y;
			break;
		}
	}
}
// 충돌 효과 빨간 사각형
void makeeffect()
{
	effectstart = n;
	if (hit == false)
		return;

	// 시간 지나면 없어짐
	if (glfwGetTime() - hittime > effecttime)
	{
		hit = false;
		return;
	}

	float s = 0.05f;   // 칸 반 크기

	// 삼각형 1 왼위 오른위 왼아래
	position[n][0] = hitx - s;
	position[n][1] = hity + s;
	position[n][2] = 0.0f;
	position[n + 1][0] = hitx + s;
	position[n + 1][1] = hity + s;
	position[n + 1][2] = 0.0f;
	position[n + 2][0] = hitx - s;
	position[n + 2][1] = hity - s;
	position[n + 2][2] = 0.0f;

	// 삼각형 2 왼아래 오른아래 오른위
	position[n + 3][0] = hitx - s;
	position[n + 3][1] = hity - s;
	position[n + 3][2] = 0.0f;
	position[n + 4][0] = hitx + s;
	position[n + 4][1] = hity - s;
	position[n + 4][2] = 0.0f;
	position[n + 5][0] = hitx + s;
	position[n + 5][1] = hity + s;
	position[n + 5][2] = 0.0f;

	// 빨강
	for (int j = 0; j < 6; ++j)
	{
		color[n + j][0] = 1.0f;
		color[n + j][1] = 0.0f;
		color[n + j][2] = 0.0f;
	}
	n += 6;
}
// 지그재그 한 칸
void moving()
{
	if (move == false)
		return;

	// 속도 조절
	double now = glfwGetTime();
	if (now - lasttime < speed)
		return;
	lasttime = now;

	int nextcol = shape[0].col + dx;
	if (nextcol >= 0 && nextcol <= 19)
	{
		// 옆 칸으로
		shape[0].col = nextcol;
	}
	else
	{
		// 줄 끝이면 한 줄 내려가고 방향 반대
		int nextrow = shape[0].row + dy;
		if (nextrow < 0 || nextrow > 19)
		{
			// 판 끝이면 위아래 반대
			dy = -dy;
			nextrow = shape[0].row + dy;
		}
		shape[0].row = nextrow;
		dx = -dx;
	}

	shape[0].x = -1.0f + (shape[0].col + 0.5f) * cell;
	shape[0].y = -1.0f + (shape[0].row + 0.5f) * cell;
	checkhit();
}
void makevertex()
{
	n = 85;
	for (int i = 0; i < shapecount; ++i)
	{
		if (shape[i].type == 0) //삼각형
		{
			position[n][0] = shape[i].x - shape[i].size;;      // x
			position[n][1] = shape[i].y - shape[i].size;;      // y
			position[n][2] = 0.0f;      // z

			position[n + 1][0] = shape[i].x + shape[i].size;
			position[n + 1][1] = shape[i].y - shape[i].size;
			position[n + 1][2] = 0.0f;

			position[n + 2][0] = shape[i].x;
			position[n + 2][1] = shape[i].y + shape[i].size;
			position[n + 2][2] = 0.0f;

			color[n][0] = shape[i].r;         // r
			color[n][1] = shape[i].g;         // g
			color[n][2] = shape[i].b;         // b
			color[n + 1][0] = shape[i].r;
			color[n + 1][1] = shape[i].g;
			color[n + 1][2] = shape[i].b;
			color[n + 2][0] = shape[i].r;
			color[n + 2][1] = shape[i].g;
			color[n + 2][2] = shape[i].b;
			n += 3;
		}
		else if (shape[i].type == 1) // 역삼각형
		{
			position[n][0] = shape[i].x;
			position[n][1] = shape[i].y - shape[i].size;
			position[n][2] = 0.0f;
			position[n + 1][0] = shape[i].x - shape[i].size;;      // x
			position[n + 1][1] = shape[i].y + shape[i].size;;      // y
			position[n + 1][2] = 0.0f;      // z

			position[n + 2][0] = shape[i].x + shape[i].size;
			position[n + 2][1] = shape[i].y + shape[i].size;
			position[n + 2][2] = 0.0f;


			color[n][0] = shape[i].r;         // r
			color[n][1] = shape[i].g;         // g
			color[n][2] = shape[i].b;         // b
			color[n + 1][0] = shape[i].r;
			color[n + 1][1] = shape[i].g;
			color[n + 1][2] = shape[i].b;
			color[n + 2][0] = shape[i].r;
			color[n + 2][1] = shape[i].g;
			color[n + 2][2] = shape[i].b;
			n += 3;
		}
		else if (shape[i].type == 2) //사각형
		{
			position[n][0] = shape[i].x - shape[i].size;;
			position[n][1] = shape[i].y + shape[i].size;
			position[n][2] = 0.0f;

			position[n + 1][0] = shape[i].x + shape[i].size;;
			position[n + 1][1] = shape[i].y + shape[i].size;
			position[n + 1][2] = 0.0f;

			position[n + 2][0] = shape[i].x - shape[i].size;;      // x
			position[n + 2][1] = shape[i].y - shape[i].size;;      // y
			position[n + 2][2] = 0.0f;      // z

			position[n + 3][0] = shape[i].x - shape[i].size;;      // x
			position[n + 3][1] = shape[i].y - shape[i].size;;      // y
			position[n + 3][2] = 0.0f;

			position[n + 4][0] = shape[i].x + shape[i].size;
			position[n + 4][1] = shape[i].y - shape[i].size;
			position[n + 4][2] = 0.0f;

			position[n + 5][0] = shape[i].x + shape[i].size;;
			position[n + 5][1] = shape[i].y + shape[i].size;
			position[n + 5][2] = 0.0f;

			color[n][0] = shape[i].r;         // r
			color[n][1] = shape[i].g;         // g
			color[n][2] = shape[i].b;         // b
			color[n + 1][0] = shape[i].r;
			color[n + 1][1] = shape[i].g;
			color[n + 1][2] = shape[i].b;
			color[n + 2][0] = shape[i].r;
			color[n + 2][1] = shape[i].g;
			color[n + 2][2] = shape[i].b;
			color[n + 3][0] = shape[i].r;
			color[n + 3][1] = shape[i].g;
			color[n + 3][2] = shape[i].b;
			color[n + 4][0] = shape[i].r;
			color[n + 4][1] = shape[i].g;
			color[n + 4][2] = shape[i].b;
			color[n + 5][0] = shape[i].r;
			color[n + 5][1] = shape[i].g;
			color[n + 5][2] = shape[i].b;

			n += 6;
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
	shapecount = rand() % 30 + 20;
	for (int i = 0; i < shapecount; ++i)
	{
		makeshape(i);
	}
	makeplayer();

	// 세이더 읽어서 세이더 프로그램 만들기
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	// VAO, VBO 만들기
	InitBuffer();

	// 콜백 등록
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

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

// 그리기
void drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 여기서 공책 채우기
	moving();
	makeLine();
	makevertex();
	makeeffect();
	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 84);
	glDrawArrays(GL_TRIANGLES, effectstart, n - effectstart);   // 빨간 칸 먼저
	glDrawArrays(GL_TRIANGLES, 85, effectstart - 85);           // 도형은 그 위에

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
		case GLFW_KEY_M:
			move = !move;
			break;
		case GLFW_KEY_EQUAL:   // 빠르게
			if (speed > 0.06f)
				speed -= 0.05f;

			break;
		case GLFW_KEY_MINUS:   // 느리게
			if (speed < 0.96f)
				speed += 0.05f;

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
		// 화면 픽셀 -> GL 좌표
		float gx = (float)(x / width * 2.0 - 1.0);
		float gy = (float)(1.0 - y / height * 2.0);
	}
}
