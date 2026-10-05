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

// 필요한 변수
GLint width = 800, height = 600;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];

// 꼭짓점 공책
GLfloat position[1000][3];
GLfloat color[1000][3];
int n = 0;   // 공책에 적힌 꼭짓점 수
int hitstart = 0;     // 공책에서 히트박스 시작 줄
float dash = 0.02f;   // 점선 한 토막 길이
// 맞추는 구역 두 칸에 걸친 가로 띠
float zoneleft = -0.85f;
float zoneright = 0.15f;
float zonebottom = -0.2f;
float zonetop = 0.2f;
struct Shape {
	float r, g, b;
	float size;
	float x, y;
	int col, row;
	int type;
	int dir =1;
	float h;          // 세로 반 크기
	int state = 0;    // 0 위아래 1 날아가는 중 2 쌓임 3 탑 다 참
	float tx, ty;     // 쌓일 자리
};
Shape shape[50];
int shapecount = 0;
void makeshape(int idx)
{
	shape[idx].size = 0.1f;   // 가로 반 크기
	shape[idx].h = 0.05f;
	if (idx % 2 == 0)
	{
		shape[idx].x = -0.6f;
		shape[idx].y = 0.0f;
		shape[idx].dir = 1;
	}
	else
	{
		shape[idx].x = -0.1f;
		shape[idx].y = 0.0f;
		shape[idx].dir = 0;
	}
	shape[idx].state = 0;
	shape[idx].r = rand() / (float)RAND_MAX;
	shape[idx].g = rand() / (float)RAND_MAX;
	shape[idx].b = rand() / (float)RAND_MAX;
}
// 새 사각형 두 개
void makepair()
{
	if (shapecount + 2 > 48)
		return;
	makeshape(shapecount);
	makeshape(shapecount + 1);
	shapecount += 2;
}
// 쌓일 자리로 한 걸음
void flying(int i)
{
	float step = 0.005f;
	float ddx = shape[i].tx - shape[i].x;
	float ddy = shape[i].ty - shape[i].y;
	float dist = sqrt(ddx * ddx + ddy * ddy);
	if (dist <= step)
	{
		// 도착
		shape[i].x = shape[i].tx;
		shape[i].y = shape[i].ty;
		shape[i].state = 2;
		return;
	}
	shape[i].x += ddx / dist * step;
	shape[i].y += ddy / dist * step;
}
void moving()
{
	for (int i = 0; i < shapecount; ++i)
	{
		if (shape[i].state == 1)
		{
			flying(i);
			continue;
		}
		if (shape[i].state >= 2)
			continue;

		if (shape[i].y <= 0.75f && shape[i].dir == 0)
		{
			shape[i].y += 0.001f;
			if (shape[i].y + shape[i].h >= 0.75f)
			{
				shape[i].dir = 1;
			}
		}
		else if (shape[i].y >= -0.75f && shape[i].dir == 1)
		{
			shape[i].y -= 0.001f;
			if (shape[i].y - shape[i].h <= -0.75f)
			{
				shape[i].dir = 0;
			}
		}
	}

	// 둘 다 쌓이면 새 사각형
	if (shapecount >= 2 && shape[shapecount - 2].state == 2 && shape[shapecount - 1].state == 2)
	{
		if (shapecount + 2 > 48)
		{
			std::cout << "탑 다 참 r로 리셋" << std::endl;
			shape[shapecount - 1].state = 3;   // 한 번만 출력
		}
		else
		{
			makepair();
		}
	}
}
void makeLine()
{
	n = 0;

	position[n][0] = -0.75f; //첫번째 칸
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.45f;
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.45f;
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.45f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.45f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.75f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.75f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.75f;
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;


	position[n][0] = -0.25f; //두번째 칸
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = 0.05f;
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = 0.05f;
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = 0.05f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = 0.05f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.25f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.25f;
	position[n][1] = 0.75f;
	position[n][2] = 0.0f;
	n++;
	position[n][0] = -0.25f;
	position[n][1] = -0.75f;
	position[n][2] = 0.0f;
	n++;
}
void makevertex()
{
	n = 17;
	for (int i = 0; i < shapecount; ++i)
	{
		position[n][0] = shape[i].x - shape[i].size;;
		position[n][1] = shape[i].y + shape[i].h;
		position[n][2] = 0.0f;

		position[n + 1][0] = shape[i].x + shape[i].size;;
		position[n + 1][1] = shape[i].y + shape[i].h;
		position[n + 1][2] = 0.0f;

		position[n + 2][0] = shape[i].x - shape[i].size;;      // x
		position[n + 2][1] = shape[i].y - shape[i].h;;      // y
		position[n + 2][2] = 0.0f;      // z

		position[n + 3][0] = shape[i].x - shape[i].size;;      // x
		position[n + 3][1] = shape[i].y - shape[i].h;;      // y
		position[n + 3][2] = 0.0f;

		position[n + 4][0] = shape[i].x + shape[i].size;
		position[n + 4][1] = shape[i].y - shape[i].h;
		position[n + 4][2] = 0.0f;

		position[n + 5][0] = shape[i].x + shape[i].size;;
		position[n + 5][1] = shape[i].y + shape[i].h;
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
// 점선 한 줄
void makedash(float x1, float y1, float x2, float y2)
{
	float len = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	int count = (int)(len / dash);   // 토막 수

	// 짝수 토막만 그림
	for (int i = 0; i < count; i += 2)
	{
		// 0이면 시작점 1이면 끝점
		float t1 = (float)i / count;
		float t2 = (float)(i + 1) / count;

		position[n][0] = x1 + (x2 - x1) * t1;
		position[n][1] = y1 + (y2 - y1) * t1;
		position[n][2] = 0.0f;
		position[n + 1][0] = x1 + (x2 - x1) * t2;
		position[n + 1][1] = y1 + (y2 - y1) * t2;
		position[n + 1][2] = 0.0f;

		// 빨강
		color[n][0] = 1.0f;
		color[n][1] = 0.0f;
		color[n][2] = 0.0f;
		color[n + 1][0] = 1.0f;
		color[n + 1][1] = 0.0f;
		color[n + 1][2] = 0.0f;
		n += 2;
	}
}
// 맞추는 구역 점선 사각형
void makehitbox()
{
	hitstart = n;
	makedash(zoneleft, zonebottom, zoneright, zonebottom);   // 아래
	makedash(zoneright, zonebottom, zoneright, zonetop);     // 오른쪽
	makedash(zoneright, zonetop, zoneleft, zonetop);         // 위
	makedash(zoneleft, zonetop, zoneleft, zonebottom);       // 왼쪽
}
// 구역 안에 다 들어왔는지
bool inzone(int i)
{
	return shape[i].y - shape[i].h >= zonebottom && shape[i].y + shape[i].h <= zonetop;
}
// 엔터 맞추기
void hitcheck()
{
	if (shapecount < 2)
		return;
	int a = shapecount - 2;   // 왼쪽 칸
	int b = shapecount - 1;   // 오른쪽 칸
	if (shape[a].state != 0 || shape[b].state != 0)
		return;

	if (inzone(a) && inzone(b))
	{
		std::cout << "맞춤" << std::endl;

		// 몇 번째 쌍인지
		int pair = a / 2;
		int tower = pair / 8;   // 8쌍마다 옆 탑
		int floor = pair % 8;   // 탑 안 층

		// 왼쪽 칸 것이 아래 오른쪽 칸 것이 위
		float towerx = 0.4f + tower * 0.25f;
		float bottom = -0.75f + floor * (shape[a].h * 2.0f + shape[b].h * 2.0f);
		shape[a].tx = towerx;
		shape[a].ty = bottom + shape[a].h;
		shape[b].tx = towerx;
		shape[b].ty = bottom + shape[a].h * 2.0f + shape[b].h;

		shape[a].state = 1;
		shape[b].state = 1;
	}
	else
	{
		std::cout << "안 맞음" << std::endl;
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
	makepair();
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
	makeLine();
	moving();
	makevertex();
	makehitbox();
	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 16);
	glDrawArrays(GL_TRIANGLES, 17, hitstart - 17);
	glDrawArrays(GL_LINES, hitstart, n - hitstart);   // 히트박스
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
		case GLFW_KEY_ENTER:
			hitcheck();
			break;
		case GLFW_KEY_R:   // 리셋
			shapecount = 0;
			makepair();
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
