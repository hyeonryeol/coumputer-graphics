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
int justcount = 0;
bool moveyang[5] = {};
bool moveum[5] = { -1 };
void makerect(int idx, float cx, float cy)
{
	float hw = 0.05f + rand() / (float)RAND_MAX * 0.1f;
	float hh = 0.05f + rand() / (float)RAND_MAX * 0.1f;

	rx1[idx] = cx - hw;  ry1[idx] = cy - hh;
	rx2[idx] = cx + hw;  ry2[idx] = cy + hh;

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
			glColor3f(r1[i], g1[i], b1[i]);
			glRectf(rx1[i], ry1[i], rx2[i], ry2[i]);

		}
		if (move1 == true)
		{
			for (int i = 0; i < mousecount; ++i)
			{
				if (rx2[i] < 1.0 && ry2[i] < 1.0 && moveyang[i] == 0)
				{

					rx1[i] += 0.001f;
					ry1[i] += 0.001f;
					rx2[i] += 0.001f;
					ry2[i] += 0.001f;
					if (rx2[i] >= 1.0 || ry2[i] >= 1.0)
					{
						moveyang[i] = -1;
						moveum[i] = 0;
					}
				}
				else if (rx1[i] > -1 && ry1[i] > -1 && moveum[i] == 0)
				{
					rx1[i] -= 0.001f;
					ry1[i] -= 0.001f;
					rx2[i] -= 0.001f;
					ry2[i] -= 0.001f;
					if (rx1[i] <= -1.0 || ry1[i] <= -1.0)
					{
						moveyang[i] = 0;
						moveum[i] = -1;
					}
				}

			}
		}
		if (move2 == true)
		{
			for (int i = 0; i < mousecount; ++i)
			{
				if (rx2[i] <= 1.0 && moveyang[i] == 0)
				{
					rx1[i] += 0.001f;
					rx2[i] += 0.001f;
					if (rx2[i] >= 1.0)
					{
						moveyang[i] = -1;
						moveum[i] = 0;
						move3 = true;
					}
				}
				else if (rx1[i] >= -1.0 && moveum[i] == 0)
				{
					rx1[i] -= 0.001f;
					rx2[i] -= 0.001f;
					if (rx1[i] <= -1.0)
					{
						moveyang[i] = 0;
						moveum[i] = -1;
						move3 = true;
					}
				}
			}
		}
		if (move3 == true)
		{
			for (int k = 0; k < 5; ++k)
			{
				for (int i = 0; i < mousecount; ++i)

				{
					ry1[i] -= 0.01f;
					ry2[i] -= 0.01f;
				}
				justcount++;
			}
			if (justcount == 5)
			{

				move3 = false;
				move2 = true;
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