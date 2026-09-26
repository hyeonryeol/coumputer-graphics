#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include<ctime>
#include<cstdlib>


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
struct Shape {

	float r, g, b;          // 현재 색
	float size;
	float x, y;
	int kind; //1- 점 2- 선 3-삼각형 4- 사각형
	int start;
	int num;
};
int select = -1;

Shape shapes[50];
int count;
GLfloat position[300][3];
GLfloat color[300][3];
int n = 0;
float lastx, lasty;
void makevertax()
{
	n = 0;
	for (int i = 0; i < count; ++i)
	{
		if (1 == shapes[i].kind)
		{
			shapes[i].start = n;   // 적기 직전 n
			shapes[i].num = 1;   // 점은 몇 줄

			position[n][0] = shapes[i].x;      // x
			position[n][1] = shapes[i].y;      // y
			position[n][2] = 0.0f;      // z

			color[n][0] = shapes[i].r;         // r
			color[n][1] = shapes[i].g;         // g
			color[n][2] = shapes[i].b;         // b

			n++;
		}
		else if (2 == shapes[i].kind)
		{
			shapes[i].start = n;   // 적기 직전 n
			shapes[i].num = 2;   // 점은 몇 줄

			position[n][0] = shapes[i].x;      // x
			position[n][1] = shapes[i].y;      // y
			position[n][2] = 0.0f;      // z

			position[n + 1][0] = shapes[i].x + shapes[i].size;
			position[n + 1][1] = shapes[i].y + shapes[i].size;
			position[n + 1][2] = 0.0f;

			color[n][0] = shapes[i].r;         // r
			color[n][1] = shapes[i].g;         // g
			color[n][2] = shapes[i].b;         // b
			color[n + 1][0] = shapes[i].r;         // r
			color[n + 1][1] = shapes[i].g;         // g
			color[n + 1][2] = shapes[i].b;         // b
			n += 2;
		}
		else if (3 == shapes[i].kind)
		{
			shapes[i].start = n;   // 적기 직전 n
			shapes[i].num = 3;   // 점은 몇 줄

			position[n][0] = shapes[i].x - shapes[i].size;;      // x
			position[n][1] = shapes[i].y - shapes[i].size;;      // y
			position[n][2] = 0.0f;      // z

			position[n + 1][0] = shapes[i].x + shapes[i].size;
			position[n + 1][1] = shapes[i].y - shapes[i].size;
			position[n + 1][2] = 0.0f;

			position[n + 2][0] = shapes[i].x;
			position[n + 2][1] = shapes[i].y + shapes[i].size;
			position[n + 2][2] = 0.0f;

			color[n][0] = shapes[i].r;         // r
			color[n][1] = shapes[i].g;         // g
			color[n][2] = shapes[i].b;         // b
			color[n + 1][0] = shapes[i].r;
			color[n + 1][1] = shapes[i].g;
			color[n + 1][2] = shapes[i].b;
			color[n + 2][0] = shapes[i].r;
			color[n + 2][1] = shapes[i].g;
			color[n + 2][2] = shapes[i].b;

			n += 3;
		}
		else if (4 == shapes[i].kind)
		{
			shapes[i].start = n;   // 적기 직전 n
			shapes[i].num = 4;   // 점은 몇 줄

			position[n][0] = shapes[i].x - shapes[i].size;;
			position[n][1] = shapes[i].y + shapes[i].size;
			position[n][2] = 0.0f;

			position[n + 1][0] = shapes[i].x + shapes[i].size;;
			position[n + 1][1] = shapes[i].y + shapes[i].size;
			position[n + 1][2] = 0.0f;

			position[n + 2][0] = shapes[i].x - shapes[i].size;;      // x
			position[n + 2][1] = shapes[i].y - shapes[i].size;;      // y
			position[n + 2][2] = 0.0f;      // z

			position[n + 3][0] = shapes[i].x + shapes[i].size;
			position[n + 3][1] = shapes[i].y - shapes[i].size;
			position[n + 3][2] = 0.0f;

			color[n][0] = shapes[i].r;         // r
			color[n][1] = shapes[i].g;         // g
			color[n][2] = shapes[i].b;         // b
			color[n + 1][0] = shapes[i].r;
			color[n + 1][1] = shapes[i].g;
			color[n + 1][2] = shapes[i].b;
			color[n + 2][0] = shapes[i].r;
			color[n + 2][1] = shapes[i].g;
			color[n + 2][2] = shapes[i].b;
			color[n + 3][0] = shapes[i].r;
			color[n + 3][1] = shapes[i].g;
			color[n + 3][2] = shapes[i].b;

			n += 4;
		}
	}
}
void makeshape(int idx)
{

	shapes[idx].size = 0.05f + rand() / (float)RAND_MAX * 0.1f;

	float cx = rand() / (float)RAND_MAX * 1.6f - 0.8f;
	float cy = rand() / (float)RAND_MAX * 1.6f - 0.8f;

	shapes[idx].x = cx;  shapes[idx].y = cy;


	shapes[idx].r = rand() / (float)RAND_MAX;
	shapes[idx].g = rand() / (float)RAND_MAX;
	shapes[idx].b = rand() / (float)RAND_MAX;

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
	GLFWwindow* window = glfwCreateWindow(width, height, "실습7", nullptr, nullptr);
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
	// 어떤 GPU로 돌고 있는지 확인용
	std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL : " << glGetString(GL_VERSION) << std::endl;

	glViewport(0, 0, width, height);
	srand(time(NULL));

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

// VAO, VBO 만들고 데이터 넣기
void InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo);

	// 0번 VBO에 좌표, attribute 0번
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 1번 VBO에 색상, attribute 1번
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
	makevertax();



	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);                              // 좌표 창고 가리키기
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);    // 거기에 좌표 넣기

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);                              // 색 창고 가리키기
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);          // 거기에 색 넣기


	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	glPointSize(8.0f);
	for (int i = 0; i < count; ++i)
	{
		if (shapes[i].kind == 1)
		{
			glDrawArrays(GL_POINTS, shapes[i].start, shapes[i].num);
		}
		else if (shapes[i].kind == 2)
		{
			glDrawArrays(GL_LINES, shapes[i].start, shapes[i].num);
		}
		else if (shapes[i].kind == 3)
		{
			glDrawArrays(GL_TRIANGLES, shapes[i].start, shapes[i].num);
		}
		else if (shapes[i].kind == 4)
		{
			glDrawArrays(GL_TRIANGLE_STRIP, shapes[i].start, 4);
		}
	}
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
		case GLFW_KEY_P:
		{
			if (count < 50)
			{
				makeshape(count);
				shapes[count].kind = 1;
				count++;
			}
			break;
		}
		case GLFW_KEY_E:
		{
			if (count < 50)
			{
				makeshape(count);
				shapes[count].kind = 2;
				count++;
			}
			break;
		}
		case GLFW_KEY_T:
		{
			if (count < 50)
			{
				makeshape(count);
				shapes[count].kind = 3;
				count++;
			}
			break;
		}
		case GLFW_KEY_R:
		{
			if (count < 50)
			{
				makeshape(count);
				shapes[count].kind = 4;
				count++;
			}
			break;
		}
		case GLFW_KEY_C:
		{
			count = 0;
			select = -1;
			break;
		}
		case GLFW_KEY_W:
		{
			if (select != -1)
				shapes[select].y += 0.05f;
			break;
		}
		case GLFW_KEY_A:
		{
			if (select != -1)
				shapes[select].x -= 0.05f;
			break;
		}
		case GLFW_KEY_S:
		{
			if (select != -1)
				shapes[select].y -= 0.05f;
			break;
		}
		case GLFW_KEY_D:
		{
			if (select != -1)
				shapes[select].x += 0.05f;
			break;
		}
		case GLFW_KEY_I:
		{
			if (select != -1)
			{
				shapes[select].x -= 0.05f;
				shapes[select].y += 0.05f;
			}
			break;
		}
		case GLFW_KEY_J:
		{
			if (select != -1)
			{
				shapes[select].x += 0.05f;
				shapes[select].y += 0.05f;
			}
			break;
		}
		case GLFW_KEY_K:
		{
			if (select != -1)
			{
				shapes[select].x -= 0.05f;
				shapes[select].y -= 0.05f;
			}
			break;
		}
		case GLFW_KEY_L:
		{
			if (select != -1)
			{
				shapes[select].x += 0.05f;
				shapes[select].y -= 0.05f;
			}
			break;
		}
		case GLFW_KEY_1:
		{
			for (int i = 0; i < count; ++i)
			{
				shapes[i].x -= 0.05f;
			}
			break;
		}
		case GLFW_KEY_2:
		{
			for (int i = 0; i < count; ++i)
			{
				shapes[i].x += 0.05f;
			}
			break;
		}
		case GLFW_KEY_3:
		{
			for (int i = 0; i < count; ++i)
			{
				shapes[i].y += 0.05f;
			}
			break;
		}
		case GLFW_KEY_4:
		{
			for (int i = 0; i < count; ++i)
			{
				shapes[i].y -= 0.05f;
			}
			break;
		}
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
		for (int i = 0; i < count; ++i)
		{
			if (gx <= shapes[i].x + shapes[i].size && gx >= shapes[i].x - shapes[i].size && gy <= shapes[i].y + shapes[i].size && gy >= shapes[i].y - shapes[i].size)
			{
				select = i;
				std::cout << select;
			}
		}
	}
}
