#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

//--- 함수 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
struct Rect {
	float x1, y1, x2, y2;   // 좌측하단, 우측상단
	float r, g, b;          // 색
	bool selected;
};
float r1[4], g1[4], b1[4] = {};

Rect rects[4][5];      // rects[사분면][몇 번째]
int  count[4] = {};    // 사분면별 개수
int minuspress = 0;

// 사분면 중심점
float centerX[4] = { -0.5f,  0.5f, -0.5f, 0.5f };
float centerY[4] = { 0.5f,  0.5f, -0.5f, -0.5f };

int main()
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < 4;++i)
	{

	r1[i] = rand() / (float)RAND_MAX;
	g1[i] = rand() / (float)RAND_MAX;
	b1[i] = rand() / (float)RAND_MAX;
	}
	
	//--- GLFW 초기화
	if (!glfwInit())
		return -1;

	//--- OpenGL 버전 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	//--- 윈도우 생성
	GLFWwindow* window = glfwCreateWindow(800, 600, "실습2", nullptr, nullptr);
	if (!window) {
		std::cerr << "윈도우 생성 실패!" << std::endl;
		glfwTerminate();
		return -1;
	}

	//--- 컨텍스트 활성화
	glfwMakeContextCurrent(window);

	//--- GLEW 초기화
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW 초기화 실패!" << std::endl;
		return -1;
	}

	//--- 뷰포트 설정
	glViewport(0, 0, 800, 600);

	//--- 콜백 함수 등록
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	//--- 메인 루프
	while (!glfwWindowShouldClose(window)) {
		//--- 화면 그리기
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glColor3f(r1[0], g1[0], b1[0]);
		glRectf(-1.0f, 1.0f, 0, 0);

		
		glColor3f(r1[1], g1[1], b1[1]);
		glRectf(0, 1.0f, 1.0f, 0);

		
		glColor3f(r1[2], g1[2], b1[2]);
		glRectf(-1.0f, 0, 0, -1.0f);

		
		glColor3f(r1[3], g1[3], b1[3]);
		glRectf(0, 0, 1.0f, -1.0f);

		for (int q = 0; q < 4; ++q)
		{
			for (int i = 0; i < count[q]; ++i)
			{
				Rect& rc = rects[q][i];
				glColor3f(rc.r, rc.g, rc.b);
				glRectf(rc.x1, rc.y1, rc.x2, rc.y2);

				if (rc.selected) {
					glColor3f(1, 1, 1);              // 흰 테두리
					glLineWidth(3);
					glBegin(GL_LINE_LOOP);
					glVertex2f(rc.x1, rc.y1);
					glVertex2f(rc.x2, rc.y1);
					glVertex2f(rc.x2, rc.y2);
					glVertex2f(rc.x1, rc.y2);
					glEnd();
				}
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

//--- 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key) {
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_1:   // 왼쪽 위
		{
			int q = 0;
			if (count[q] < 5)
			{
				Rect& rc = rects[q][count[q]];

				float hw = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반폭
				float hh = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반높이

				//--- 중심 흔들기
				float ox = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hw);
				float oy = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hh);
				float cx = centerX[q] + ox;
				float cy = centerY[q] + oy;

				rc.x1 = cx - hw;
				rc.y1 = cy - hh;
				rc.x2 = cx + hw;
				rc.y2 = cy + hh;

				rc.r = rand() / (float)RAND_MAX;
				rc.g = rand() / (float)RAND_MAX;
				rc.b = rand() / (float)RAND_MAX;

				count[q] += 1;
			}
			break;
		}
		case GLFW_KEY_2:   // 오른쪽 위
		{
			int q = 1;
			if (count[q] < 5)
			{
				Rect& rc = rects[q][count[q]];

				float hw = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반폭
				float hh = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반높이

				//--- 중심 흔들기
				float ox = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hw);
				float oy = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hh);
				float cx = centerX[q] + ox;
				float cy = centerY[q] + oy;

				rc.x1 = cx - hw;
				rc.y1 = cy - hh;
				rc.x2 = cx + hw;
				rc.y2 = cy + hh;

				rc.r = rand() / (float)RAND_MAX;
				rc.g = rand() / (float)RAND_MAX;
				rc.b = rand() / (float)RAND_MAX;

				count[q] += 1;
			}
			break;
		}
		case GLFW_KEY_3:   // 왼쪽 아래
		{
			int q = 2;
			if (count[q] < 5)
			{
				Rect& rc = rects[q][count[q]];

				float hw = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반폭
				float hh = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반높이

				//--- 중심 흔들기
				float ox = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hw);
				float oy = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hh);
				float cx = centerX[q] + ox;
				float cy = centerY[q] + oy;

				rc.x1 = cx - hw;
				rc.y1 = cy - hh;
				rc.x2 = cx + hw;
				rc.y2 = cy + hh;

				rc.r = rand() / (float)RAND_MAX;
				rc.g = rand() / (float)RAND_MAX;
				rc.b = rand() / (float)RAND_MAX;

				count[q] += 1;
			}
			break;
		}
		case GLFW_KEY_4:   // 오른쪽 아래
		{
			int q = 3;
			if (count[q] < 5)
			{
				Rect& rc = rects[q][count[q]];

				float hw = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반폭
				float hh = 0.08f + (rand() / (float)RAND_MAX) * 0.12f;  // 반높이

				//--- 중심 흔들기
				float ox = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hw);
				float oy = (rand() / (float)RAND_MAX * 2.0f - 1.0f) * (0.45f - hh);
				float cx = centerX[q] + ox;
				float cy = centerY[q] + oy;

				rc.x1 = cx - hw;
				rc.y1 = cy - hh;
				rc.x2 = cx + hw;
				rc.y2 = cy + hh;

				rc.r = rand() / (float)RAND_MAX;
				rc.g = rand() / (float)RAND_MAX;
				rc.b = rand() / (float)RAND_MAX;

				count[q] += 1;
			}
			break;
		}
		case GLFW_KEY_EQUAL:
		{
			float d = 0.02f;
			for (int q = 0; q < 4; ++q)
			{
				for (int i = 0; i < count[q]; ++i)
				{
					Rect& rc = rects[q][i];
					if (rc.selected)
					{
						rc.x1 -= d; rc.y1 -= d;
						rc.x2 += d; rc.y2 += d;
					}
				}
			}
			break;
		}
		case GLFW_KEY_MINUS:
		{
			float d = 0.02f;
			if (minuspress < 4)
			{
				for (int q = 0; q < 4; ++q)
				{
					for (int i = 0; i < count[q]; ++i)
					{
						Rect& rc = rects[q][i];
						if (rc.selected)
						{
							rc.x1 += d; rc.y1 += d;
							rc.x2 -= d; rc.y2 -= d;
						}
					}
				}
				minuspress++;
			}
			break;
		}
		case GLFW_KEY_C:
		{
			for (int q = 0; q < 4; ++q)
			{
				for (int i = 0; i < count[q]; ++i)
				{
					Rect& rc = rects[q][i];
					if (rc.selected)
					{
						rc.r = rand() / (float)RAND_MAX;
						rc.g = rand() / (float)RAND_MAX;
						rc.b = rand() / (float)RAND_MAX;
					}
				}
			}
			break;
		}
		case GLFW_KEY_R:
		{
			for (int q = 0; q < 4; ++q)
				for (int i = 0; i < count[q]; ++i)
					rects[q][i].selected = false;
			for (int q = 0; q < 4; ++q)
			{
				for (int i = 0; i < count[q]; ++i)
				{
					count[q] = 0;
					
				}
			}
			for (int i = 0; i < 4; ++i)
			{
				r1[i] = rand() / (float)RAND_MAX;
				g1[i] = rand() / (float)RAND_MAX;
				b1[i] = rand() / (float)RAND_MAX;
			}
			break;
		}
		}
	}
}
//--- 마우스 버튼 콜백
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		float gx = (float)(x / 800.0 * 2.0 - 1.0);
		float gy = (float)(1.0 - y / 600.0 * 2.0);

		// 먼저 전부 선택 해제
		for (int q = 0; q < 4; ++q)
			for (int i = 0; i < count[q]; ++i)
				rects[q][i].selected = false;

		// 클릭된 사각형 선택
		for (int q = 0; q < 4; ++q)
			for (int i = 0; i < count[q]; ++i)
			{
				Rect& rc = rects[q][i];
				if (gx >= rc.x1 && gx <= rc.x2 && gy >= rc.y1 && gy <= rc.y2)
					rc.selected = true;
			}
		minuspress = 0;
	}
}