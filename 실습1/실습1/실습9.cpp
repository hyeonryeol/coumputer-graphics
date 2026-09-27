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
struct Triangle {
	float r, g, b;
	float size;
	float x, y;
	float dx, dy;   // 한 프레임에 움직이는 양
};
int mouseclik = -1;
float lastx, lasty;
Triangle tri[4]; // 0 오른쪽위 1 왼쪽위 2 왼쪽아래 3 오른쪽아래
int moving = 0;
// 크기 0.002 ~ 0.008, 부호는 반반 확률로
float randomSpeed()
{
	float speed = 0.002f + rand() / (float)RAND_MAX * 0.006f;
	if (rand() % 2 == 0)
		speed = -speed;
	return speed;
}
int dir = 0;
void maketri(int idx)
{
	tri[idx].dx = randomSpeed();
	tri[idx].dy = randomSpeed();


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

}
// 삼각형마다 자기 속도만큼 이동
void move()
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
	if (moving == 1)
	{
		move();
	}
	makevertax();

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 삼각형은 0번 줄부터 12줄
	glDrawArrays(GL_TRIANGLES, 0, 12);

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
