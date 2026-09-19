#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
float lastx, lasty;
float plusx, plusy;
float r1[30] = {};
float g1[30] = {};
float b1[30] = {};

float rx1[30] = {};
float ry1[30] = {};
float rx2[30] = {};
float ry2[30] = {};
float er;
float eg;
float eb;
int rightcount = 0;
float mx1 = 0;
float mx2 = 0;
float my1 = 0;
float my2 = 0;
int acount = 0;
float orirx1[30] = {};
float oriry1[30] = {};
float orirx2[30] = {};
float oriry2[30] = {};
bool origin = false;
int mousecount = 0;
bool mouseclick = false;
float mouseRclick = 0;
float overlap = 0;
bool drag = false;
// 콜백 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double x, double y);
void makerect(int idx, float cx, float cy)
{
	float hw = 0.05f;
	float hh = 0.05f;

	cx = rand() / (float)RAND_MAX * 2.0f - 1.0f;   // -1 ~ 1
	cy = rand() / (float)RAND_MAX * 2.0f - 1.0f;


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
void plusrect(int idx, float gx, float gy)
{
    float hw = 0.05f;
    float hh = 0.05f;

    rx1[idx] = gx - hw;  ry1[idx] = gy - hh;   
    rx2[idx] = gx + hw;  ry2[idx] = gy + hh;

    orirx1[idx] = rx1[idx];  orirx2[idx] = rx2[idx];   
    oriry1[idx] = ry1[idx];  oriry2[idx] = ry2[idx];

    r1[idx] = rand() / (float)RAND_MAX;
    g1[idx] = rand() / (float)RAND_MAX;
    b1[idx] = rand() / (float)RAND_MAX;
}
void eraser(int idx, float hx, float hy)
{
	float hw = 0.1f - mouseRclick + overlap;
	float hh = 0.1f - mouseRclick + overlap;
	
	mx1 = hx - hw; my1 = hy - hh;
	mx2 = hx + hw; my2 = hy + hh;

}
void removeRect(int idx)
{
	for (int i = idx; i < mousecount - 1; ++i)
	{
		rx1[i] = rx1[i + 1];  ry1[i] = ry1[i + 1];
		rx2[i] = rx2[i + 1];  ry2[i] = ry2[i + 1];
		r1[i] = r1[i + 1];   g1[i] = g1[i + 1];   b1[i] = b1[i + 1];
	}

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


	srand(time(NULL));
	for (int i = 0; i < 20; ++i)
	{
		makerect(i, 0, 0);
		mousecount++;
	}

	// 콜백 등록
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	// 메인 루프
	while (!glfwWindowShouldClose(window))
	{
		// 화면 지우기
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
		if (mouseclick == true)
		{

			glColor3f(er, eg, eb);
			glRectf(mx1, my1, mx2, my2);
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
		case GLFW_KEY_R:
		{
			mousecount = 0;
			overlap = 0;
			mouseRclick = 0;
			for (int i = 0; i < 20; ++i)
			{
				makerect(i, 0, 0);
				mousecount++;
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
		float hx = (float)(x / 800.0 * 2.0 - 1.0);
		float hy = (float)(1.0 - y / 600.0 * 2.0);
		lastx = hx;
		lasty = hy;
		eraser(1, hx, hy);
		mouseclick = true;
		if (mouseclick == true)
		{
			for (int i = 0; i < mousecount; ++i)
			{
				if (mx1 <= rx2[i] && mx2 >= rx1[i] && my1 <= ry2[i] && my2 >= ry1[i])
				{
					
					rx1[i] = rx2[i] = ry1[i] = ry2[i] = 999.0f;
					er = r1[i]; eg = g1[i]; eb = b1[i];

				}
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseclick = false;
		overlap = 0;
		for (int i = 0; i < mousecount; ++i)
		{
			rx1[i] = orirx1[i];  ry1[i] = oriry1[i];
			rx2[i] = orirx2[i];  ry2[i] = oriry2[i];
		}
		er = 0; eg = 0; eb = 0;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mouseRclick += 0.005f;
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		float gx = (float)(x / 800.0 * 2.0 - 1.0);
		float gy = (float)(1.0 - y / 600.0 * 2.0);
		
		if (mousecount < 30)
		{
			plusrect(mousecount, gx, gy);
			mousecount++;
			
		}
	}
}
void CursorPosCallback(GLFWwindow* window, double x, double y)
{
	if (mouseclick == false) return;   //  드래그

	float hx = (float)(x / 800.0 * 2.0 - 1.0);
	float hy = (float)(1.0 - y / 600.0 * 2.0);
	for (int i = 0; i < mousecount; ++i)
	{
		if (mx1 <= rx2[i] && mx2 >= rx1[i] && my1 <= ry2[i] && my2 >= ry1[i])
		{
			rx1[i] = rx2[i] = ry1[i] = ry2[i] = 999.0f;
			er = r1[i]; eg = g1[i]; eb = b1[i];
			overlap += 0.005f;
			eraser(1, hx, hy);
		}
	}
	eraser(1, hx, hy);                  // 지우개를 커서 위치
}