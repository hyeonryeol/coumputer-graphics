#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
GLint width = 700, height = 700;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];

// 꼭짓점 공책 위치와 텍스처 좌표
GLfloat position[100][3];
GLfloat texcoord[100][2];
int n = 0;          // 공책에 적힌 꼭짓점 수
int cubestart;      // 육면체 위치 면마다 6개씩
int pyramidstart;   // 사각뿔 위치 옆면 3개씩 4면 + 밑면 6개
int axisstart;      // 좌표축 선 6개

// 텍스처 A ~ F
GLuint texture[6];
const char* texfile[6] = { "texA.bmp", "texB.bmp", "texC.bmp", "texD.bmp", "texE.bmp", "texF.bmp" };

// 상태
bool showcube = true;     // c 육면체 p 사각뿔
float anglex = 30.0f;     // x축 회전 각도
float angley = -30.0f;    // y축 회전 각도
int spinx = 0;            // x 양 X 음 0 멈춤
int spiny = 0;            // y 양 Y 음 0 멈춤

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
	std::string vertexSource = filetobuf("vertex30.glsl");
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
	std::string fragmentSource = filetobuf("fragment30.glsl");
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

// bmp 파일 읽어서 텍스처 만들기 24비트 bmp만
GLuint loadbmp(const char* file)
{
	std::ifstream f(file, std::ios::binary);
	if (!f.is_open())
	{
		std::cerr << "텍스처 열기 실패: " << file << std::endl;
		return 0;
	}
	unsigned char header[54];
	f.read((char*)header, 54);
	int offset = *(int*)&header[10];   // 픽셀 시작 위치
	int w = *(int*)&header[18];        // 가로
	int h = *(int*)&header[22];        // 세로
	int rowsize = (w * 3 + 3) / 4 * 4; // 한 줄은 4바이트 배수로 채워짐
	std::vector<unsigned char> data(rowsize * h);
	f.seekg(offset);
	f.read((char*)data.data(), data.size());

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// bmp는 아래 줄부터 저장되고 색 순서가 BGR
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, data.data());
	return id;
}

// VAO, VBO 만들고 텍스처 읽기
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

	// 1번 VBO는 텍스처 좌표, attribute 1번 한 점에 2개
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	for (int i = 0; i < 6; i++)
		texture[i] = loadbmp(texfile[i]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "outTexture"), 0);   // 0번 텍스처 유닛
}

// 점 하나 공책에 적기
void addvertex(glm::vec3 p, float u, float v)
{
	position[n][0] = p.x;
	position[n][1] = p.y;
	position[n][2] = p.z;
	texcoord[n][0] = u;
	texcoord[n][1] = v;
	n++;
}

// 크기 1 육면체 면마다 텍스처 좌표 0 ~ 1
void addcube()
{
	glm::vec3 p[8] = {
		{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, 0.5f,  0.5f }, { -0.5f, 0.5f,  0.5f },
		{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
	};
	// 앞 뒤 오른 왼 위 아래 바깥에서 보면 왼아래부터 반시계
	int face[6][4] = { { 0, 1, 2, 3 }, { 5, 4, 7, 6 }, { 1, 5, 6, 2 }, { 4, 0, 3, 7 }, { 3, 2, 6, 7 }, { 4, 5, 1, 0 } };
	float uv[4][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
	int order[6] = { 0, 1, 2, 0, 2, 3 };
	for (int k = 0; k < 6; k++)
		for (int j = 0; j < 6; j++) {
			int c = order[j];
			addvertex(p[face[k][c]], uv[c][0], uv[c][1]);
		}
}

// 사각뿔 밑면 가로세로 1 높이 1 가운데가 원점
void addpyramid()
{
	glm::vec3 top(0.0f, 0.5f, 0.0f);
	glm::vec3 b[4] = { { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f } };
	for (int k = 0; k < 4; k++) {   // 옆면 앞 오른 뒤 왼
		addvertex(b[k], 0.0f, 0.0f);
		addvertex(b[(k + 1) % 4], 1.0f, 0.0f);
		addvertex(top, 0.5f, 1.0f);
	}
	// 밑면
	addvertex(b[3], 0.0f, 0.0f); addvertex(b[2], 1.0f, 0.0f); addvertex(b[1], 1.0f, 1.0f);
	addvertex(b[3], 0.0f, 0.0f); addvertex(b[1], 1.0f, 1.0f); addvertex(b[0], 0.0f, 1.0f);
}

// 좌표축 선 3개
void addaxis()
{
	float L = 1.2f;
	addvertex(glm::vec3(-L, 0, 0), 0, 0); addvertex(glm::vec3(L, 0, 0), 0, 0);
	addvertex(glm::vec3(0, -L, 0), 0, 0); addvertex(glm::vec3(0, L, 0), 0, 0);
	addvertex(glm::vec3(0, 0, -L), 0, 0); addvertex(glm::vec3(0, 0, L), 0, 0);
}

// 모델 행렬 보내기
void setmodel(glm::mat4 m)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(m));
}

// 처음 상태 s 키
void resetall()
{
	showcube = true;
	anglex = 30.0f;
	angley = -30.0f;
	spinx = 0;
	spiny = 0;
}

// 그리기
void drawScene()
{
	anglex += 1.0f * spinx;
	angley += 1.0f * spiny;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	n = 0;
	cubestart = n;
	addcube();
	pyramidstart = n;
	addpyramid();
	axisstart = n;
	addaxis();

	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 3 * sizeof(GLfloat), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, n * 2 * sizeof(GLfloat), texcoord);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	// 원근 투영과 카메라
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniform1f(glGetUniformLocation(shaderProgramID, "alpha"), 1.0f);

	// 좌표축 회색 단색
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.75f, 0.75f, 0.75f);
	setmodel(glm::mat4(1.0f));
	glDrawArrays(GL_LINES, axisstart, 6);

	// 도형 x축 y축 회전
	glm::mat4 m = glm::rotate(glm::mat4(1.0f), glm::radians(anglex), glm::vec3(1.0f, 0.0f, 0.0f));
	m = glm::rotate(m, glm::radians(angley), glm::vec3(0.0f, 1.0f, 0.0f));
	setmodel(m);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);
	glActiveTexture(GL_TEXTURE0);

	if (showcube) {
		for (int k = 0; k < 6; k++) {   // 면마다 다른 텍스처
			glBindTexture(GL_TEXTURE_2D, texture[k]);
			glDrawArrays(GL_TRIANGLES, cubestart + 6 * k, 6);
		}
	}
	else {
		for (int k = 0; k < 4; k++) {   // 옆면 4개
			glBindTexture(GL_TEXTURE_2D, texture[k]);
			glDrawArrays(GL_TRIANGLES, pyramidstart + 3 * k, 3);
		}
		glBindTexture(GL_TEXTURE_2D, texture[4]);   // 밑면
		glDrawArrays(GL_TRIANGLES, pyramidstart + 12, 6);
	}
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);
	if (action != GLFW_PRESS)
		return;
	switch (key)
	{
	case GLFW_KEY_Q:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_C:       // 육면체
		showcube = true;
		break;
	case GLFW_KEY_P:       // 사각뿔
		showcube = false;
		break;
	case GLFW_KEY_X:       // x 양 X 음 회전 같은 키 다시 누르면 멈춤
	{
		int want = shift ? -1 : 1;
		spinx = (spinx == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_Y:       // y 양 Y 음 회전
	{
		int want = shift ? -1 : 1;
		spiny = (spiny == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_S:       // 초기화
		resetall();
		break;
	}
}
