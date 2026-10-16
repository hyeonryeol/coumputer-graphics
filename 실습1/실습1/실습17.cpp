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
#include <sstream>   // istringstream
#include <vector>    // vector



struct Vertex { float x, y, z; };   // 점 하나
struct Face { int a, b, c; };       // 삼각형 하나 = 점 번호 3개

std::vector<Vertex> leftv, rightv;   // 왼쪽 오른쪽 점 목록
std::vector<Face> leftf, rightf;     // 왼쪽 오른쪽 삼각형 목록
int leftstart, leftcount;            // 공책에서 왼쪽 도형 위치
int rightstart, rightcount;          // 공책에서 오른쪽 도형 위치


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
GLfloat position[6000][3];
GLfloat color[6000][3];
int n = 0;   // 공책에 적힌 꼭짓점 수
bool faceon[6] = { false, false, false, false, false, false };
bool faceonpy[5] = { false, false, false, false, false };
bool hpress = false;

float anglex[2] = { 0.0f, 0.0f };   // 도형별 x축 자전 각도
float angley[2] = { 0.0f, 0.0f };   // 도형별 y축 자전 각도
int spinx = 0;   // x 자전 명령 1 양 -1 음 0 없음
int spiny = 0;
int target = -1;                     // 0 왼쪽 1 오른쪽 2 둘 다
int gongjeon = 0;
float gongjeonangle[2] = { 0.0f, 0.0f };
float size[2] = { 1.0f, 1.0f };
int sizedir = 0;
float size2[2] = { 1.0f, 1.0f };
int sizedir2 = 0;
float objmovex[2] = { 0.0f,0.0f };
float objmovey[2] = { 0.0f,0.0f };

float jump[2] = { 0.0f,0.0f };
int movexdir = 0;
bool swap = false;
int swaping = -1;
bool swap2 = false;
int swaping2 = -1;
float movex = 0.0f;
float movey = 0.0f;
bool vmode = false;
int vdir = 1;
float vsize[2] = { 1.0f,1.0f };
bool cpress = false;

void makeline()
{
	n = 0;

	position[n][0] = -2.0f; //첫번째 칸
	position[n][1] = 0.0f;
	position[n][2] = 0.0f;
	color[n][0] = 1.0f;
	color[n][1] = 0.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 2.0f;
	position[n][1] = 0.0f;
	position[n][2] = 0.0f;
	color[n][0] = 1.0f;
	color[n][1] = 0.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = -2.0f;
	position[n][2] = 0.0f;
	color[n][0] = 0.0f;
	color[n][1] = 1.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = 2.0f;
	position[n][2] = 0.0f;
	color[n][0] = 0.0f;
	color[n][1] = 1.0f;
	color[n][2] = 0.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = 0.0f;
	position[n][2] = -2.0f;
	color[n][0] = 0.0f;
	color[n][1] = 0.0f;
	color[n][2] = 1.0f;
	n++;
	position[n][0] = 0.0f;
	position[n][1] = 0.0f;
	position[n][2] = 2.0f;
	color[n][0] = 0.0f;
	color[n][1] = 0.0f;
	color[n][2] = 1.0f;
	n++;
}

// obj 파일 읽기
void loadobj(const char* name, std::vector<Vertex>& vv, std::vector<Face>& ff)
{
	vv.clear();
	ff.clear();
	std::ifstream file(name);
	if (!file) std::cout << "열기 실패: " << name << std::endl;
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream ss(line);
		std::string type;
		ss >> type;
		if (type == "v") {
			Vertex p;
			ss >> p.x >> p.y >> p.z;
			vv.push_back(p);
		}
		else if (type == "f") {
			Face t;
			ss >> t.a >> t.b >> t.c;
			t.a--; t.b--; t.c--;
			ff.push_back(t);
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
	glEnable(GL_DEPTH_TEST);   //은면제거
	glDepthFunc(GL_GREATER);   // z 큰 쪽이 가까움
	glClearDepth(0.0);         // 지울 때 기준값을 0으로
	srand((unsigned int)time(NULL));

	loadobj("cone.obj", leftv, leftf);    // 왼쪽 원뿔
	loadobj("cube.obj", rightv, rightf);  // 오른쪽 육면체
	
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

// 도형 하나를 공책에 적기
void addobj(std::vector<Vertex>& vv, std::vector<Face>& ff)
{
	for (int k = 0; k < ff.size(); k++) {
		int idx[3] = { ff[k].a, ff[k].b, ff[k].c };
		for (int j = 0; j < 3; j++) {
			position[n][0] = vv[idx[j]].x;
			position[n][1] = vv[idx[j]].y;
			position[n][2] = vv[idx[j]].z;
			color[n][0] = 0.0f;
			color[n][1] = 0.0f;
			color[n][2] = 0.0f;
			n++;
		}
	}
}
// 그리기
void drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 여기서 공책 채우기
	makeline();

	leftstart = n;
	addobj(leftv, leftf);
	leftcount = n - leftstart;
	rightstart = n;
	addobj(rightv, rightf);
	rightcount = n - rightstart;

	for (int i = 0; i < 2; i++) {
		if (target == 2 || target == i) {   // 지금 타겟만 각도가 늘어남
			anglex[i] += 1.0f * spinx;
			angley[i] += 1.0f * spiny;
		}
	}

	for (int i = 0; i < 2; i++) {
		if (target == 2 || target == i) {   // 지금 타겟만 각도가 늘어남
			gongjeonangle[i] += 1.0f * gongjeon;

		}
	}
	for (int i = 0; i < 2; i++) {
		if (target == 2 || target == i) {   // 지금 타겟만 각도가 늘어남

			size[i] += 0.02f * sizedir;
			if (size[i] < 0.1f)
			{
				sizedir = 0;
				size[i] = 0.1f;
			}
			if (size[i] >= 1.0f)
			{
				sizedir = 0;
				size[i] = 1.0f;
			}
		}
	}
	for (int i = 0; i < 2; i++) {
		if (target == 2 || target == i) {   // 지금 타겟만 각도가 늘어남

			size2[i] += 0.02f * sizedir2;
			if (size2[i] < 0.1f)
			{
				sizedir2 = 0;
				size2[i] = 0.1f;
			}
			if (size2[i] >= 1.0f)
			{
				sizedir2 = 0;
				size2[i] = 1.0f;
			}
		}
	}
	if (swap == true)
	{
		if (swaping == 1)
		{
			if (objmovex[0] < 1.0f)
				objmovex[0] += 0.02f;
			if (objmovex[1] > -1.0f)
				objmovex[1] -= 0.02f;
		}
		else if (swaping == -1)
		{
			if (objmovex[0] > 0.0f)
				objmovex[0] -= 0.02f;
			if (objmovex[1] < 0.0f)
				objmovex[1] += 0.02f;
		}
	}

	if (swap2 == true)
	{
		if (swaping2 == 1)
		{
			if (objmovex[0] < 1.0f)
			{
				objmovex[0] += 0.02f;
				jump[0] = 2.0f * objmovex[0] * (1.0f - objmovex[0]);
			}
			if (objmovex[1] > -1.0f)
			{
				objmovex[1] -= 0.02f;
				jump[1] = -2.0f * objmovex[0] * (1.0f - objmovex[0]);
			}
		}
		else if (swaping2 == -1)
		{
			if (objmovex[0] > 0.0f)
				objmovex[0] -= 0.02f;
				jump[0] = -2.0f * objmovex[0] * (1.0f - objmovex[0]);
			if (objmovex[1] < 0.0f)
				objmovex[1] += 0.02f;
				jump[1] = 2.0f * objmovex[0] * (1.0f - objmovex[0]);
		}
	}
	if (vmode == true)
	{
		for (int i = 0; i < 2; ++i)
		{
			gongjeonangle[i] += 1.0f;
			angley[i] += 2.0f;
		}

		// 왼쪽 확대 오른쪽 축소
		vsize[0] += 0.01f * vdir;
		vsize[1] -= 0.01f * vdir;
		if (vsize[0] >= 1.5f)   // 왼쪽 최대
		{
			vsize[0] = 1.5f;
			vsize[1] = 0.5f;
			vdir = -1;
		}
		if (vsize[0] <= 0.5f)   // 왼쪽 최소
		{
			vsize[0] = 0.5f;
			vsize[1] = 1.5f;
			vdir = 1;
		}
	}
	// 공책을 창고에 올리기
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(position), position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);
	glUseProgram(shaderProgramID);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));   // x축으로 30도
	model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));   // y축으로 30도
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glBindVertexArray(vao);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glm::mat4 objmodel = glm::translate(model, glm::vec3(movex, movey, 0.0f));   // 이동

	glDrawArrays(GL_LINES, 0, 6);            // 축
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // 선(테두리만)
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(objmodel));


	// 왼쪽 도형
	glm::mat4 leftm = glm::rotate(objmodel, glm::radians(gongjeonangle[0]), glm::vec3(0.0f, 1.0f, 0.0f));
	leftm = glm::scale(leftm, glm::vec3(size2[0], size2[0], size2[0]));
	leftm = glm::translate(leftm, glm::vec3(-0.5f + objmovex[0], 0.0f + objmovey[0] + jump[0], 0.0f));
	leftm = glm::scale(leftm, glm::vec3(size[0] * vsize[0], size[0] * vsize[0], size[0] * vsize[0]));

	leftm = glm::rotate(leftm, glm::radians(anglex[0]), glm::vec3(1.0f, 0.0f, 0.0f));//자전
	leftm = glm::rotate(leftm, glm::radians(angley[0]), glm::vec3(0.0f, 1.0f, 0.0f));
	leftm = glm::rotate(leftm, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));   // x축으로 90도
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(leftm));
	glDrawArrays(GL_TRIANGLES, leftstart, leftcount);

	// 오른쪽 도형
	glm::mat4 rightm = glm::rotate(objmodel, glm::radians(gongjeonangle[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	rightm = glm::scale(rightm, glm::vec3(size2[1], size2[1], size2[1]));
	rightm = glm::translate(rightm, glm::vec3(0.5f + objmovex[1], 0.0f + objmovey[1] + jump[1], 0.0f));
	rightm = glm::scale(rightm, glm::vec3(size[1] * vsize[1], size[1] * vsize[1], size[1] * vsize[1]));

	rightm = glm::rotate(rightm, glm::radians(anglex[1]), glm::vec3(1.0f, 0.0f, 0.0f));
	rightm = glm::rotate(rightm, glm::radians(angley[1]), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(rightm));
	glDrawArrays(GL_TRIANGLES, rightstart, rightcount);
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
		case GLFW_KEY_1: target = 0; break;   // 왼쪽만
		case GLFW_KEY_2: target = 1; break;   // 오른쪽만
		case GLFW_KEY_3: target = 2; break;   // 둘 다
		case GLFW_KEY_X:
			spinx = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
			break;
		case GLFW_KEY_Y:
			spiny = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
			break;
		case GLFW_KEY_R:
			gongjeon = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
			break;
		case GLFW_KEY_A:
			sizedir = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
			break;
		case GLFW_KEY_B:
			sizedir2 = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
			break;
		case GLFW_KEY_D:
			movexdir = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
			if (target == 0)
				objmovex[0] += 0.02f * movexdir;
			if (target == 1)
				objmovex[1] += 0.02f * movexdir;
			if (target == 2)
			{
				objmovex[0] += 0.02f * movexdir;
				objmovex[1] += 0.02f * movexdir;
			}
			break;
		case GLFW_KEY_E:
			movexdir = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
			if (target == 0)
				objmovey[0] += 0.02f * movexdir;
			if (target == 1)
				objmovey[1] += 0.02f * movexdir;
			if (target == 2)
			{
				objmovey[0] += 0.02f * movexdir;
				objmovey[1] += 0.02f * movexdir;
			}
			break;
		case GLFW_KEY_T:
			swap = true;
			swaping = -swaping;
			break;
		case GLFW_KEY_U:
			swap2 = true;
			swaping2 = -swaping2;
			break;
		case GLFW_KEY_V:
			vmode = !vmode;
			break;
		case GLFW_KEY_C:
			cpress = !cpress;
			if (cpress == true)
			{
				loadobj("sphere.obj", leftv, leftf);    // 왼쪽 원뿔
				loadobj("pyramid.obj", rightv, rightf);  // 오른쪽 육면체
			}
			else if (cpress == false)
			{
				loadobj("cone.obj", leftv, leftf);    // 왼쪽 원뿔
				loadobj("cube.obj", rightv, rightf);  // 오른쪽 육면체
			}
			break;
		case GLFW_KEY_S:
			// 대상
			target = -1;
			// 자전
			anglex[0] = 0.0f; anglex[1] = 0.0f;
			angley[0] = 0.0f; angley[1] = 0.0f;
			spinx = 0; spiny = 0;
			// 공전
			gongjeon = 0;
			gongjeonangle[0] = 0.0f; gongjeonangle[1] = 0.0f;
			// a A 제자리 크기
			size[0] = 1.0f; size[1] = 1.0f;
			sizedir = 0;
			// b B 원점 기준 크기
			size2[0] = 1.0f; size2[1] = 1.0f;
			sizedir2 = 0;
			// d e 이동
			objmovex[0] = 0.0f; objmovex[1] = 0.0f;
			objmovey[0] = 0.0f; objmovey[1] = 0.0f;
			movexdir = 0;
			// t
			swap = false; swaping = -1;
			// u
			swap2 = false; swaping2 = -1;
			jump[0] = 0.0f; jump[1] = 0.0f;
			// v
			vmode = false; vdir = 1;
			vsize[0] = 1.0f; vsize[1] = 1.0f;
			// c 도형 원래대로
			cpress = false;
			loadobj("cone.obj", leftv, leftf);
			loadobj("cube.obj", rightv, rightf);
			// 화살표 이동
			movex = 0.0f; movey = 0.0f;
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
