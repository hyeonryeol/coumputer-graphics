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
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// 필요한 변수
GLint width = 600, height = 600;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao, vbo[2];

// 꼭짓점 공책
GLfloat position[300][3];
GLfloat color[300][3];
int n = 0;   // 공책에 적힌 꼭짓점 수
bool faceon[6] = { false, false, false, false, false, false };
bool faceonpy[5] = { false, false, false, false, false };
bool hpress = false;
float anglex = 0.0f;   // 지금x축 각도
float angley = 0.0f;
int spinx = 0;         //방향
int spiny = 0;
float movex = 0.0f;
float movey = 0.0f;
bool cube = false;
bool py = false;
float topangle = 0.0f;
bool topspin = false;
bool frontspin = false;
int frontdir = 1;
float frontangle = 0.0f;
bool sidespin = false;
float sideangle = 0.0f;
float backsize = 1.0f;
int backdir = 0;
float pairangle = 0.0f;
bool pairspin = false;
int pair = 0;
// 2번 자리 교체
float faceoffset[4] = { 0.0f, 0.0f, 0.0f, 0.0f };   // 면마다 옮겨간 각도
float swapangle = 0.0f;
bool swapspin = false;
int swapa = 0;   // +90 도는 면
int swapb = 1;   // -90 도는 면

// 면 k가 지금 있는 자리
int slotof(int k)
{
	int turn = (int)(faceoffset[k] / 90.0f);
	return ((k + turn) % 4 + 4) % 4;
}
bool gongjeon = false;
float gongjeonangle = 0.0f;
bool topgongjeon = false;
float topgongjeonangle = 0.0f;

void makeline()
{
	n = 0;

	position[n][0] = -1.0f; //첫번째 칸
	position[n][1] = 0.0f;
	position[n][2] = 0.0f;
	color[n][0] = 1.0f;
	color[n][1] = 0.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 1.0f;
	position[n][1] = 0.0f;
	position[n][2] = 0.0f;
	color[n][0] = 1.0f;
	color[n][1] = 0.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = -1.0f;
	position[n][2] = 0.0f;
	color[n][0] = 0.0f;
	color[n][1] = 1.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = 1.0f;
	position[n][2] = 0.0f;
	color[n][0] = 0.0f;
	color[n][1] = 1.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = 0.0f;
	position[n][2] = -1.0f;
	color[n][0] = 0.0f;
	color[n][1] = 0.0f;
	color[n][2] = 1.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = 0.0f;
	position[n][2] = 1.0f;
	color[n][0] = 0.0f;
	color[n][1] = 0.0f;
	color[n][2] = 1.0f;
	n++;
}
// 면 색
float faceangle[6] = { 0.0f, 90.0f, 180.0f, -90.0f, -90.0f, 90.0f };

// 면 색
float facecolor[6][3] = {
	{ 1.0f, 1.0f, 0.0f },   // 앞 노랑
	{ 1.0f, 0.0f, 1.0f },   // 오른쪽 자홍
	{ 0.0f, 1.0f, 1.0f },   // 뒤 청록
	{ 1.0f, 0.5f, 0.0f },   // 왼쪽 주황
	{ 0.5f, 0.5f, 0.5f },   // 위 회색
	{ 0.5f, 0.0f, 1.0f },   // 아래 보라
};
void makecube()
{

	for (int k = 0; k < 6; ++k)
	{
		position[n][0] = -0.3f;
		position[n][1] = -0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;
		position[n][0] = 0.3f;
		position[n][1] = -0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;
		position[n][0] = 0.3f;
		position[n][1] = 0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;

		position[n][0] = -0.3f;
		position[n][1] = -0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;

		position[n][0] = 0.3f;
		position[n][1] = 0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;

		position[n][0] = -0.3f;
		position[n][1] = 0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;
	}
}
void makepy()
{
	for (int k = 0; k < 4; ++k)
	{
		position[n][0] = -0.3f;
		position[n][1] = -0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;
		position[n][0] = 0.3f;
		position[n][1] = -0.3f;
		position[n][2] = 0.3f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;
		position[n][0] = 0.0f;
		position[n][1] = 0.3f;
		position[n][2] = 0.0f;
		color[n][0] = facecolor[k][0];
		color[n][1] = facecolor[k][1];
		color[n][2] = facecolor[k][2];
		n++;
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

	glDepthFunc(GL_GREATER);   // z 큰 쪽이 가까움
	glClearDepth(0.0);         // 지울 때 기준값을 0으로
	srand((unsigned int)time(NULL));

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
	std::string vertexSource = filetobuf("vertex13.glsl");
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	makeline();
	makecube();
	makepy();
	anglex += 1.0f * spinx;
	angley += 1.0f * spiny;
	if (topspin == true)
		topangle += 1.0f;
	if (frontspin == true)
	{
		if (frontdir == 1)
		{
			frontangle -= 1.0f;
			if (frontangle == -90.0f)
				frontdir = 0;
		}
		else if (frontdir == -1)
		{
			frontangle += 1.0f;
			if (frontangle == 0.0f)
				frontdir = 0;
		}
	}
	if (sidespin == true)
		sideangle += 1.0f;

	backsize += 0.01f * backdir;
	if (backsize <= 0.0f)
	{
		backdir = 0;
		backsize = 0.0f;
	}
	if (backsize >= 1.0f)
	{
		backdir = 0;
		backsize = 1.0f;
	}
	if (pairspin == true)
	{
		pairangle += 1.0f;
		if (pairangle == 360.0f)
		{
			pairangle = 0.0f;
			pairspin = false;
		}
	}
	if (swapspin == true)
	{
		swapangle += 1.0f;
		if (swapangle >= 90.0f)
		{
			
			faceoffset[swapa] += 90.0f;
			faceoffset[swapb] -= 90.0f;
			swapangle = 0.0f;
			swapspin = false;
		}
	}
	if (gongjeon == true)
	{
		gongjeonangle += 1.0f;
	}
	if (topgongjeon == true)
	{
		topgongjeonangle += 1.0f;
	}
		
	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);
	glUseProgram(shaderProgramID);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));   // x축으로 30도
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));   // y축으로 30도
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glBindVertexArray(vao);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glm::mat4 objmodel = glm::translate(model, glm::vec3(movex, movey, 0.0f));   // 이동
	objmodel = glm::rotate(objmodel, glm::radians(anglex), glm::vec3(1.0f, 0.0f, 0.0f));
	objmodel = glm::rotate(objmodel, glm::radians(angley), glm::vec3(0.0f, 1.0f, 0.0f));
	glDrawArrays(GL_LINES, 0, 6);            // 축
	for (int k = 0; k < 6; ++k)
	{
		if (faceon[k] == false)
			continue;

		glm::mat4 face = glm::mat4(1.0f);
		glm::vec3 axis;
		if (k < 4)
			axis = glm::vec3(0.0f, 1.0f, 0.0f);   // y축
		else
			axis = glm::vec3(1.0f, 0.0f, 0.0f);   // x축
		glm::mat4 spin = glm::mat4(1.0f);
		if (k == 0)
		{
			spin = glm::translate(spin, glm::vec3(0.0f, 0.3f, 0.3f));
			spin = glm::rotate(spin, glm::radians(frontangle), glm::vec3(1.0f, 0.0f, 0.0f));
			spin = glm::translate(spin, glm::vec3(0.0f, -0.3f, -0.3f));
		}
		if (k == 1)
		{
			spin = glm::rotate(spin, glm::radians(sideangle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (k == 2)
		{
			spin = glm::scale(spin, glm::vec3(backsize, backsize, 1.0f));
		}
		if (k == 3)
		{
			spin = glm::rotate(spin, glm::radians(sideangle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (k == 4)
		{
			spin = glm::translate(spin, glm::vec3(0.0f, 0.3f, 0.0f));
			spin = glm::rotate(spin, glm::radians(topangle), glm::vec3(0.0f, 0.0f, 1.0f));
			spin = glm::translate(spin, glm::vec3(0.0f, -0.3f, 0.0f));
		}
		if (pair == 0)
		{
			if (k == 0)
			{
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, 0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, -0.3f));
			}
			if (k == 1)
			{
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, 0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, -0.3f));
			}
		}
		if (pair == 1)
		{
			if (k == 1)
			{
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, -0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, 0.3f));
			}
			if (k == 2)
			{
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, -0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, 0.3f));
			}
		}
		if (pair == 2)
		{
			if (k == 2)
			{
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, -0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, 0.3f));
			}
			if (k == 3)
			{
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, -0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, 0.3f));
			}
		}
		if (pair == 3)
		{
			if (k == 0)
			{
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, 0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, -0.3f));
			}
			if (k == 3)
			{
				spin = glm::translate(spin, glm::vec3(-0.3f, 0.0f, 0.3f));
				spin = glm::rotate(spin, glm::radians(pairangle), glm::vec3(0.0f, 1.0f, 0.0f));
				spin = glm::translate(spin, glm::vec3(0.3f, 0.0f, -0.3f));
			}
		}
		if (k == 0)
		{
			spin = glm::rotate(spin, glm::radians(gongjeonangle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (k == 1)
		{
			spin = glm::rotate(spin, glm::radians(gongjeonangle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (k == 2)
		{
			spin = glm::rotate(spin, glm::radians(gongjeonangle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (k == 3)
		{
			spin = glm::rotate(spin, glm::radians(gongjeonangle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (k == 4)
		{
			spin = glm::rotate(spin, glm::radians(topgongjeonangle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (k == 5)
		{
			spin = glm::rotate(spin, glm::radians(topgongjeonangle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (k == 0)
		{
			spin = glm::rotate(spin, glm::radians(topgongjeonangle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (k == 2)
		{
			spin = glm::rotate(spin, glm::radians(topgongjeonangle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		face = glm::rotate(face, glm::radians(faceangle[k]), axis);
		glm::mat4 total = objmodel * face;

		// 가운데 y축 공전
		glm::mat4 orbit = glm::mat4(1.0f);
		if (k < 4)
		{
			float a = faceoffset[k];
			if (swapspin == true && k == swapa)
				a += swapangle;
			if (swapspin == true && k == swapb)
				a -= swapangle;
			orbit = glm::rotate(orbit, glm::radians(a), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		total = objmodel * orbit * spin * face;    // 면 회전 먼저, 기울이기 나중
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(total));
		glDrawArrays(GL_TRIANGLES, 6 + k * 6, 6);
	}
	for (int k = 0; k < 4; ++k)
	{
		if (faceonpy[k] == false)
			continue;
		glm::mat4 face = glm::mat4(1.0f);
		glm::vec3 axis;
		axis = glm::vec3(0.0f, 1.0f, 0.0f);   // y축
		face = glm::rotate(face, glm::radians(faceangle[k]), axis);
		glm::mat4 total = objmodel * face;     // 면 회전 먼저, 기울이기 나중
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(total));
		glDrawArrays(GL_TRIANGLES, 42 + k * 3, 3);
	}
	// 사각뿔 바닥

	if (faceonpy[4] == true)
	{
		glm::mat4 base = glm::mat4(1.0f);
		base = glm::rotate(base, glm::radians(faceangle[5]), glm::vec3(1.0f, 0.0f, 0.0f));   // x축 90도
		glm::mat4 total = objmodel * base;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(total));
		glDrawArrays(GL_TRIANGLES, 36, 6);   // 육면체 아랫면 6줄
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

		case GLFW_KEY_C:
		{
			faceonpy[0] = false;
			faceonpy[1] = false;
			faceonpy[2] = false;
			faceonpy[3] = false;
			faceonpy[4] = false;
			faceon[0] = true;
			faceon[1] = true;
			faceon[2] = true;
			faceon[3] = true;
			faceon[4] = true;
			faceon[5] = true;
			cube = true;
			py = false;
			break;
		}
		case GLFW_KEY_P:
		{
			cube = false;
			py = true;
			faceonpy[0] = true;
			faceonpy[1] = true;
			faceonpy[2] = true;
			faceonpy[3] = true;
			faceonpy[4] = true;
			faceon[0] = false;
			faceon[1] = false;
			faceon[2] = false;
			faceon[3] = false;
			faceon[4] = false;
			faceon[5] = false;
			break;
		}
		case GLFW_KEY_H:
			hpress = !hpress;
			if (hpress == true)
			{
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
			break;
		case GLFW_KEY_W:
			if (mods & GLFW_MOD_SHIFT)
				// W 대문자
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   // 면(속 채우기)
			else
				// w 소문자
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // 선(테두리만)
			break;
		case GLFW_KEY_R:
			cube = true;
			py = false;
			faceonpy[0] = false;
			faceonpy[1] = false;
			faceonpy[2] = false;
			faceonpy[3] = false;
			faceonpy[4] = false;
			faceon[0] = true;
			faceon[1] = true;
			faceon[2] = true;
			faceon[3] = true;
			faceon[4] = true;
			faceon[5] = true;
			spinx = 0;
			spiny = 0;
			anglex = 0.0f;
			angley = 0.0f;
			topspin = false;
			topangle = 0.0f;
			frontspin = false;
			frontangle = 0.0f;
			sidespin = false;
			sideangle = 0.0f;
			backsize = 1.0f;
			backdir = 0;
			pairspin = false;
			swapangle = 0.0f;
			swapspin = false;
			gongjeon = false;
			gongjeonangle = 0.0f;
			topgongjeon = false;
			topgongjeonangle = 0.0f;

			// 자리 교체 되돌리기
			for (int k = 0; k < 4; ++k)
				faceoffset[k] = 0.0f;
			swapangle = 0.0f;
			swapspin = false;
			break;
		case GLFW_KEY_Y:
			if (spiny == 0)
			{
				spiny = 1;
			}
			else
			{
				spiny = 0;
			}
			break;
		case GLFW_KEY_T:
			topspin = !topspin;
			break;
		case GLFW_KEY_F:
			frontspin = true;
			if (frontangle == 0.0f)
				frontdir = 1;
			if (frontangle == -90.0f)
				frontdir = -1;
			break;
		case GLFW_KEY_S:
			sidespin = !sidespin;
			break;
		case GLFW_KEY_B:
			if (backsize == 1.0f)
				backdir = -1;
			if (backsize == 0.0f)
				backdir = 1;
			break;
		case GLFW_KEY_1:
			
			if (pairspin == false)
			{
				pair = rand() % 4;
				pairspin = true;
			}
			break;
		case GLFW_KEY_2:
		{
			if (swapspin == false)
			{
				
				int p = rand() % 4;
				for (int k = 0; k < 4; ++k)
				{
					if (slotof(k) == p)
						swapa = k;
					if (slotof(k) == (p + 1) % 4)
						swapb = k;
				}
				swapspin = true;
			}
			break;
		}
		case GLFW_KEY_4:
			gongjeon = !gongjeon;
			break;
		case GLFW_KEY_5:
			topgongjeon = !topgongjeon;
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
