#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>

// 콜백 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
float lastx, lasty;
float r1[5] = {};
float g1[5] = {};
float b1[5] = {};

float rx1[5] = {};
float ry1[5] = {};
float rx2[5] = {};
float ry2[5] = {};
int mousecount = 0;



bool move1 = false;
bool move2 = false;
bool move3 = false;
bool move4 = false;
bool move5 = false;

int phase3[5] = {};
int   state1[5] = {};
int   state4[5] = {};     // move4 전용 0=커지는중 1=작아지는중
int   state[5] = {};
int   nextDir[5] = {};
float dropLeft[5] = {};

float orirx1[5] = {};
float oriry1[5] = {};
float orirx2[5] = {};
float oriry2[5] = {};
bool origin = false;

void makerect(int idx, float cx, float cy)
{
	float hw = 0.05f + rand() / (float)RAND_MAX * 0.1f;
	float hh = 0.05f + rand() / (float)RAND_MAX * 0.1f;

	rx1[idx] = cx - hw;  ry1[idx] = cy - hh;
	rx2[idx] = cx + hw;  ry2[idx] = cy + hh;

	orirx1[idx] = rx1[idx];
	orirx2[idx] = rx2[idx];
	oriry1[idx] = ry1[idx];
	oriry2[idx] = ry2[idx];

	r1[idx] = rand() / (float)RAND_MAX;
	g1[idx] = rand() / (float)RAND_MAX;
	b1[idx] = rand() / (float)RAND_MAX;

}



int main()
{
	// GLFW 초기화
	if (!glfwInit())
		return -1;

	// OpenGL 버전과 프로파일 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// 윈도우 생성
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "윈도우 생성 실패" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 컨텍스트 활성화
	glfwMakeContextCurrent(window);

	// GLEW 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW 초기화 실패" << std::endl;
		return -1;
	}

	// 뷰포트 설정
	glViewport(0, 0, 800, 600);

	// 콜백 등록
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	// 메인 루프
	while (!glfwWindowShouldClose(window))
	{
		// 화면 지우기
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 여기에 그리기 코드
		for (int i = 0; i < mousecount; ++i)
		{
			if (origin == false)
			{

				glColor3f(r1[i], g1[i], b1[i]);
				glRectf(rx1[i], ry1[i], rx2[i], ry2[i]);
			}
			else
			{
				glColor3f(r1[i], g1[i], b1[i]);
				glRectf(orirx1[i], oriry1[i], orirx2[i], oriry2[i]);
			}

		}
		if (move1 == true)
		{
			for (int i = 0; i < mousecount; ++i)
			{
				if (state1[i] == 0)          // 오른쪽 위로
				{
					rx1[i] += 0.001f;
					ry1[i] += 0.001f;
					rx2[i] += 0.001f;
					ry2[i] += 0.001f;
					if (rx2[i] >= 1.0f || ry2[i] >= 1.0f)
						state1[i] = 1;
				}
				else                          // 왼쪽 아래로
				{
					rx1[i] -= 0.001f;
					ry1[i] -= 0.001f;
					rx2[i] -= 0.001f;
					ry2[i] -= 0.001f;
					if (rx1[i] <= -1.0f || ry1[i] <= -1.0f)
						state1[i] = 0;
				}
			}
		}
		if (move2 == true)
		{
			for (int i = 0; i < mousecount; ++i)
			{
				if (state[i] == 0)
				{
					rx1[i] += 0.001f;  rx2[i] += 0.001f;
					if (rx2[i] >= 1.0f)
					{
						state[i] = 2;
						nextDir[i] = 1;
						dropLeft[i] = 0.1f;
					}
				}
				else if (state[i] == 1)
				{
					rx1[i] -= 0.001f;  rx2[i] -= 0.001f;
					if (rx1[i] <= -1.0f)
					{
						state[i] = 2;
						nextDir[i] = 0;
						dropLeft[i] = 0.1f;
					}
				}
				else if (state[i] == 2)
				{
					float step = 0.001f;
					ry1[i] -= step;  ry2[i] -= step;
					dropLeft[i] -= step;
					if (dropLeft[i] <= 0.0f)
						state[i] = nextDir[i];
				}
			}
		}
		if (move3 == true)
		{
			float s = 0.002f;
			for (int i = 0; i < mousecount; ++i)
			{
				if (phase3[i] == 0)              // 오른쪽으로
				{
					rx1[i] += s;  rx2[i] += s;
					if (rx2[i] >= 1.0f) phase3[i] = 1;   // 오른쪽 벽 닿음
				}
				else if (phase3[i] == 1)         // 아래로
				{
					ry1[i] -= s;  ry2[i] -= s;
					if (ry1[i] <= -1.0f) phase3[i] = 2;  // 바닥 닿음
				}
				else if (phase3[i] == 2)         // 왼쪽으로
				{
					rx1[i] -= s;  rx2[i] -= s;
					if (rx1[i] <= -1.0f) phase3[i] = 3;  // 왼쪽 벽 닿음
				}
				else if (phase3[i] == 3)         // 위로
				{
					ry1[i] += s;  ry2[i] += s;
					if (ry2[i] >= 1.0f) phase3[i] = 0;   // 천장 닿음
				}
			}
		}
		if (move4 == true)
		{
			float s = 0.001f;
			for (int i = 0; i < mousecount; ++i)
			{
				float w = rx2[i] - rx1[i];             // 지금 가로 크기
				float ow = orirx2[i] - orirx1[i];      // 원래 가로 크기
				if (state4[i] == 0)
				{
					rx1[i] -= s;  ry1[i] -= s;
					rx2[i] += s;  ry2[i] += s;
					if (w >= ow * 2.0f) state4[i] = 1;
				}
				else
				{
					rx1[i] += s;  ry1[i] += s;
					rx2[i] -= s;  ry2[i] -= s;
					if (w <= ow) state4[i] = 0;
				}
			}
		}
		if (move5 == true)
		{
			for (int i = 0; i < mousecount; ++i)
			{
				r1[i] = rand() / (float)RAND_MAX;
				g1[i] = rand() / (float)RAND_MAX;
				b1[i] = rand() / (float)RAND_MAX;
			}
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
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
		{
			if (move1 == false)
				move1 = true;
			else
				move1 = false;
			break;
		}
		case GLFW_KEY_2:
		{
			if (move2 == false)
				move2 = true;
			else
				move2 = false;
			break;
		}
		case GLFW_KEY_3:
		{
			move3 = !move3;
			break;
		}
		case GLFW_KEY_4:
		{
			move4 = !move4;
			break;
		}
		case GLFW_KEY_5:
		{
			move5 = !move5;
			break;
		}
		case GLFW_KEY_M:
		{
			origin = !origin;
			break;
		}
		case GLFW_KEY_S:
		{
			move1 = false;
			move2 = false;
			move3 = false;
			move4 = false;
			move5 = false;
			break;
		}
		case GLFW_KEY_R:
		{
		
			mousecount = 0;
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
		float gx = (float)(x / 800.0 * 2.0 - 1.0);
		float gy = (float)(1.0 - y / 600.0 * 2.0);

		if (mousecount < 5)
		{
			lastx = gx;
			lasty = gy;
			if (mousecount < 5) {
				makerect(mousecount, gx, gy);
				mousecount++;
			}

		}

	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{

	}
}