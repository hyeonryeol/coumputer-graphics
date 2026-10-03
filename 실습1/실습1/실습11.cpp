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

void makeshape(int idx)
{
	shape[idx].type = rand() % 3;
	shape[idx].col = rand() % 20;
	shape[idx].row = rand() % 20;

	shape[idx].x = -1.0f + (shape[idx].col + 0.5f) * cell;
	shape[idx].y = -1.0f + (shape[idx].row + 0.5f) * cell;
	shape[idx].r = rand() / (float)RAND_MAX;
	shape[idx].g = rand() / (float)RAND_MAX;
	shape[idx].b = rand() / (float)RAND_MAX;
	shape[idx].size = 0.02f + rand() / (float)RAND_MAX * 0.025f;
}
void makevertex()
{
	n = 85;
	for (int i = 0; i < shapecount; ++i)
	{
		if (shape[i].type == 0)
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
		else if (shape[i].type == 1)
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
		else if (shape[i].type == 2)
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
	shapecount = rand() % 20 + 5;
	for (int i = 0; i < shapecount; ++i)
		makeshape(i);
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
	makevertex();
	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 84);
	glDrawArrays(GL_TRIANGLES, 85, n - 85);
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
