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
GLfloat position[900][3];
GLfloat color[900][3];
int n = 0;   // 공책에 적힌 꼭짓점 수

// 삼각형이 지나간 자리저장
const int MAXTRAIL = 200;
float trailx[4][MAXTRAIL], traily[4][MAXTRAIL];
int trailN[4] = {};            // 각 삼각형 자취에 쌓인 점 개수
int trailstart[4], traillen[4];// position 배열에서 자취가 그려질 위치/개수
struct Triangle {
	float r, g, b;
	float size;
	float x, y;
	float dx, dy;   // 한 프레임에 움직이는 양
	float angle2;
	float angle4;
	float radius;   // 반지름
	float theta;    // 현재 각도
};
const float PI = 3.14159265f;
float rad(float deg) { return deg * PI / 180.0f; }   // 도 -> 라디안
bool rocate[4] = { false };
bool wasHwall[4] = {};   // 지난 프레임에 좌우 벽에 닿아 있었나
int rdir[4] = { 1, 1, 1, 1 };
int ycount[4] = {};
int mouseclik = -1;
float lastx, lasty;
Triangle tri[4]; // 0 오른쪽위 1 왼쪽위 2 왼쪽아래 3 오른쪽아래
int moving = 0;

float randomSpeed()
{
	float speed = 0.008f;

	speed = -speed;
	return speed;
}
int dir = 0;
void makeLine()
{
	// 가로선 왼쪽 끝, 오른쪽 끝, 세로선 아래 끝, 위 끝
	GLfloat line[4][2] = { { tri[0].x, tri[0].y}, {1.0f, 0.0f}, {0.0f, -1.0f}, {0.0f, 1.0f} };

	for (int i = 0; i < 4; ++i)
	{
		position[i][0] = line[i][0];
		position[i][1] = line[i][1];
		position[i][2] = 0.0f;


		color[i][0] = 0.0f;
		color[i][1] = 0.0f;
		color[i][2] = 1.0f;
	}
	n = 4;
}
void maketri(int idx)
{
	tri[idx].dx = randomSpeed();
	tri[idx].dy = randomSpeed();
	tri[idx].angle2 += rad(90.0f);
	tri[idx].angle4 += rad(0.0005f);

	if (idx == 0)
	{
		tri[idx].x = rand() / (float)RAND_MAX * 0.7f + 0.15f;
		tri[idx].y = rand() / (float)RAND_MAX * 0.7f + 0.15f;
		tri[idx].r = rand() / (float)RAND_MAX;
		tri[idx].g = rand() / (float)RAND_MAX;
		tri[idx].b = rand() / (float)RAND_MAX;
		tri[idx].size = 0.05f + rand() / (float)RAND_MAX * 0.1f;
	}
	else if (idx == 1)
	{
		tri[idx].x = rand() / (float)RAND_MAX * 0.7f - 0.85f;
		tri[idx].y = rand() / (float)RAND_MAX * 0.7f + 0.15f;
		tri[idx].r = rand() / (float)RAND_MAX;
		tri[idx].g = rand() / (float)RAND_MAX;
		tri[idx].b = rand() / (float)RAND_MAX;
		tri[idx].size = 0.05f + rand() / (float)RAND_MAX * 0.1f;
	}
	else if (idx == 2)
	{
		tri[idx].x = rand() / (float)RAND_MAX * 0.7f - 0.85f;
		tri[idx].y = rand() / (float)RAND_MAX * 0.7f - 0.85f;
		tri[idx].r = rand() / (float)RAND_MAX;
		tri[idx].g = rand() / (float)RAND_MAX;
		tri[idx].b = rand() / (float)RAND_MAX;
		tri[idx].size = 0.05f + rand() / (float)RAND_MAX * 0.1f;
	}
	else if (idx == 3)
	{
		tri[idx].x = rand() / (float)RAND_MAX * 0.7f + 0.15f;
		tri[idx].y = rand() / (float)RAND_MAX * 0.7f - 0.85f;
		tri[idx].r = rand() / (float)RAND_MAX;
		tri[idx].g = rand() / (float)RAND_MAX;
		tri[idx].b = rand() / (float)RAND_MAX;
		tri[idx].size = 0.05f + rand() / (float)RAND_MAX * 0.1f;
	}


}
// 공책 0번 줄부터 삼각형 4개 적기
void makevertax()
{
	n = 0;
	for (int i = 0; i < 4; ++i)
	{
		position[n][0] = tri[i].x - tri[i].size;;      // x
		position[n][1] = tri[i].y - tri[i].size;;      // y
		position[n][2] = 0.0f;      // z

		position[n + 1][0] = tri[i].x + tri[i].size;
		position[n + 1][1] = tri[i].y - tri[i].size;
		position[n + 1][2] = 0.0f;

		position[n + 2][0] = tri[i].x;
		position[n + 2][1] = tri[i].y + tri[i].size;
		position[n + 2][2] = 0.0f;

		color[n][0] = tri[i].r;         // r
		color[n][1] = tri[i].g;         // g
		color[n][2] = tri[i].b;         // b
		color[n + 1][0] = tri[i].r;
		color[n + 1][1] = tri[i].g;
		color[n + 1][2] = tri[i].b;
		color[n + 2][0] = tri[i].r;
		color[n + 2][1] = tri[i].g;
		color[n + 2][2] = tri[i].b;
		n += 3;
	}
	if (moving == 2)
	{
		n = 0;
		for (int i = 0; i < 4; ++i)
		{
			float s = tri[i].size;
			float a = tri[i].angle2;
			float c = cosf(a), sn = sinf(a);
	
			float ox[3] = { -s,  s, 0.0f };
			float oy[3] = { -s, -s, s };

			for (int k = 0; k < 3; ++k)
			{
				// angle 만큼 회전
				float rx = ox[k] * c - oy[k] * sn;
				float ry = ox[k] * sn + oy[k] * c;
				position[n + k][0] = tri[i].x + rx;
				position[n + k][1] = tri[i].y + ry;
				position[n + k][2] = 0.0f;
				color[n + k][0] = tri[i].r;
				color[n + k][1] = tri[i].g;
				color[n + k][2] = tri[i].b;
			}
			n += 3;
		}
	}
	if (moving == 4)
	{
		n = 0;
		for (int i = 0; i < 4; ++i)
		{
			float s = tri[i].size;
			float a = tri[i].theta;
			float c = cosf(a), sn = sinf(a);
	
			float ox[3] = { -s,  s, 0.0f };
			float oy[3] = { -s, -s, s };

			for (int k = 0; k < 3; ++k)
			{
				// angle 만큼 회전
				float rx = ox[k] * c - oy[k] * sn;
				float ry = ox[k] * sn + oy[k] * c;
				position[n + k][0] = tri[i].x + rx;
				position[n + k][1] = tri[i].y + ry;
				position[n + k][2] = 0.0f;
				color[n + k][0] = tri[i].r;
				color[n + k][1] = tri[i].g;
				color[n + k][2] = tri[i].b;
			}
			n += 3;
		}
	}

	// 삼각형이 지나온 선
	for (int i = 0; i < 4; ++i)
	{
		trailstart[i] = n;
		traillen[i] = trailN[i];
		for (int j = 0; j < trailN[i]; ++j)
		{
			position[n][0] = trailx[i][j];
			position[n][1] = traily[i][j];
			position[n][2] = 0.0f;
			color[n][0] = tri[i].r;
			color[n][1] = tri[i].g;
			color[n][2] = tri[i].b;
			n++;
		}
	}
}
// 삼각형마다 자기 속도만큼 이동
void move()
{
	if (moving == 1)
	{
		for (int i = 0; i < 4; ++i)
		{
			tri[i].x += tri[i].dx;
			tri[i].y += tri[i].dy;
			if (tri[i].x + tri[i].size >= 1 || tri[i].x - tri[i].size <= -1)
			{
				tri[i].dx = -tri[i].dx;

			}
			if (tri[i].y + tri[i].size >= 1 || tri[i].y - tri[i].size <= -1)
			{
				tri[i].dy = -tri[i].dy;
			}
		}
	}
	if (moving == 2)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (tri[i].x + tri[i].size < 1 && tri[i].x - tri[i].size > -1)
			{
				tri[i].x += tri[i].dx;
			}
			bool hwall = (tri[i].x + tri[i].size >= 1 || tri[i].x - tri[i].size <= -1);
			if (hwall)
			{
				tri[i].y += tri[i].dy;
				tri[i].dx = -tri[i].dx;
				if (!wasHwall[i]) tri[i].angle2 += 3.14159265f;   // 좌우 벽에 처음 닿은 순간만 180도
				ycount[i] += 1;
				if (ycount[i] == 7)
				{
					tri[i].x += tri[i].dx;
					ycount[i] = 0;
				}
			}
			bool vwall = (tri[i].y - tri[i].size <= -1 || tri[i].y + tri[i].size >= 1);
			if (vwall)
			{
				tri[i].dy = -tri[i].dy;

			}

			wasHwall[i] = hwall;

		}
	}
	if (moving == 3)
	{
		for (int i = 0; i < 4; ++i)
		{
			tri[i].x += tri[i].dx / 4;
			tri[i].y += tri[i].dy;
			if (tri[i].x + tri[i].size >= 1 || tri[i].x - tri[i].size <= -1)
			{
				tri[i].dx = -tri[i].dx;

			}
			if (tri[i].y + tri[i].size >= 1 || tri[i].y - tri[i].size <= -1)
			{
				tri[i].dy = -tri[i].dy;
			}
		}
	}
	if (moving == 4)
	{
		
		for (int i = 0; i < 4; ++i)
		{
			tri[i].theta += 0.05f;                  // 각도 회전
			tri[i].radius += rdir[i] * 0.002f;       // 방향대로 반지름 

			if (tri[i].radius >= 1.0f) rdir[i] = -1; // 최대 도달
			if (tri[i].radius <= 0.1f) rdir[i] = 1; // 최소 도달 
			 
			tri[i].x = tri[i].radius * cosf(tri[i].theta);
			tri[i].y = tri[i].radius * sinf(tri[i].theta);
		}
	}


}
// 지금 삼각형 중심 위치를 자취에 기록 
void recordTrail()
{
	for (int i = 0; i < 4; ++i)
	{
		if (trailN[i] < MAXTRAIL)
		{
			trailx[i][trailN[i]] = tri[i].x;
			traily[i][trailN[i]] = tri[i].y;
			trailN[i]++;
		}
		else
		{
			for (int j = 1; j < MAXTRAIL; ++j)
			{
				trailx[i][j - 1] = trailx[i][j];
				traily[i][j - 1] = traily[i][j];
			}
			trailx[i][MAXTRAIL - 1] = tri[i].x;
			traily[i][MAXTRAIL - 1] = tri[i].y;
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
	GLFWwindow* window = glfwCreateWindow(width, height, "실습9", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "윈도우 생성 실패" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);   // 모니터 주사율에 맞춰 그려서 속도를 일정하게

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
	maketri(0);
	maketri(1);
	maketri(2);
	maketri(3);
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

	// 움직이고 나서 공책 채우기
	if (moving != 0)
	{
		move();
		recordTrail();   // 움직인 자리를 자취에 기록
	}
	makeLine();
	makevertax();

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 삼각형은 0번 줄부터 12줄
	//glDrawArrays(GL_LINES, 0, 4);
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// 삼각형마다 지나온 자취를 선으로 그리기
	for (int i = 0; i < 4; ++i)
		if (traillen[i] >= 2)
			glDrawArrays(GL_LINE_STRIP, trailstart[i], traillen[i]);

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
		case GLFW_KEY_1:
			moving = 1;
			break;
		case GLFW_KEY_2:
			moving = 2;
			break;
		case GLFW_KEY_3:
			moving = 3;
			break;
		case GLFW_KEY_4:
			moving = 4;
			for (int i = 0; i < 4; ++i)
			{
				// 원점에서 현재 위치까지 거리 반지름
				tri[i].radius = sqrtf(tri[i].x * tri[i].x + tri[i].y * tri[i].y);
				// 현재 위치의 각도
				tri[i].theta = atan2f(tri[i].y, tri[i].x);
			}
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
		lastx = gx;
		lasty = gy;
		if (gx >= -1.0f && gx <= 0.0f && gy >= 0.0f && gy <= 1.0f)
		{
			mouseclik = 1;
			std::cout << "1click" << std::endl;
		}
		else if (gx <= 1.0f && gx >= 0.0f && gy >= 0.0f && gy <= 1.0f)
		{
			mouseclik = 0;
			std::cout << "0click" << std::endl;
		}
		else if (gx >= -1.0f && gx <= 0.0f && gy <= 0.0f && gy >= -1.0f)
		{
			mouseclik = 2;
			std::cout << "2click" << std::endl;
		}
		else if (gx <= 1.0f && gx >= 0.0f && gy <= 0.0f && gy >= -1.0f)
		{
			mouseclik = 3;
			std::cout << "3click" << std::endl;
		}
		tri[mouseclik].size = 0.05f + rand() / (float)RAND_MAX * 0.1f;
		tri[mouseclik].r = rand() / (float)RAND_MAX;
		tri[mouseclik].g = rand() / (float)RAND_MAX;
		tri[mouseclik].b = rand() / (float)RAND_MAX;
		tri[mouseclik].x = lastx;
		tri[mouseclik].y = lasty;
		tri[mouseclik].dx = randomSpeed();
		tri[mouseclik].dy = randomSpeed();
	}
}
