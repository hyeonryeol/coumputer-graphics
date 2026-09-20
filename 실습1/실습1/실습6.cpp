#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

struct Rect {
	float x1, y1, x2, y2;   // 좌하단, 우상단
	float r, g, b;          // 현재 색
	float dx, dy;           // 프레임당 이동량 (방향)
	bool moving;            // 쪼개져서 움직이는 조각인가
	bool alive;             // 살아있나 (작아지면 false)
};

Rect rects[200];
int count = 0;

// 콜백 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void makerect(int idx)
{
	float hw = 0.05f + rand() / (float)RAND_MAX * 0.1f;
	float hh = 0.05f + rand() / (float)RAND_MAX * 0.1f;

	float cx = rand() / (float)RAND_MAX * 2.0f - 1.0f;   // -1 ~ 1
	float cy = rand() / (float)RAND_MAX * 2.0f - 1.0f;

	rects[idx].x1 = cx - hw;  rects[idx].y1 = cy - hh;
	rects[idx].x2 = cx + hw;  rects[idx].y2 = cy + hh;

	rects[idx].r = rand() / (float)RAND_MAX;
	rects[idx].g = rand() / (float)RAND_MAX;
	rects[idx].b = rand() / (float)RAND_MAX;

	rects[idx].dx = 0.0f;  rects[idx].dy = 0.0f;
	rects[idx].moving = false;
	rects[idx].alive = true;
}

void split4(int i)
{
	float midx = (rects[i].x1 + rects[i].x2) / 2.0f;
	float midy = (rects[i].y1 + rects[i].y2) / 2.0f;
	float hw = (rects[i].x2 - rects[i].x1) / 2.0f;   // 조각 반너비
	float hh = (rects[i].y2 - rects[i].y1) / 2.0f;

	Rect base = rects[i];        // 색 등 복사용
	float d = 0.003f;            // 이동 속도

	// 8방향 (좌 우 하 상 + 대각선 4개)
	float dirx[8] = { -d, d, 0, 0, -d, d, -d, d };
	float diry[8] = { 0, 0, -d, d, -d, -d, d, d };

	int mode = rand() % 4;   // 0=좌우상하 1=대각선 2=한방향 3=8방향

	if (mode == 3)   // 4번: 8방향 -> 8조각 (가운데에 작은 사각형 8개)
	{
		if (count + 8 > 200) return;
		for (int k = 0; k < 8; ++k)
		{
			Rect p = base;
			p.x1 = midx - hw / 2.0f;  p.x2 = midx + hw / 2.0f;
			p.y1 = midy - hh / 2.0f;  p.y2 = midy + hh / 2.0f;
			p.dx = dirx[k];  p.dy = diry[k];
			p.moving = true; p.alive = true;
			rects[count] = p; count++;
		}
		rects[i].alive = false;
		return;
	}

	// 0,1,2번: 사등분 -> 4조각
	if (count + 4 > 200) return;

	// 4조각 좌표 (좌하, 우하, 좌상, 우상)
	float qx1[4] = { base.x1, midx,     base.x1, midx };
	float qy1[4] = { base.y1, base.y1,  midy,    midy };
	float qx2[4] = { midx,    base.x2,  midx,    base.x2 };
	float qy2[4] = { midy,    midy,     base.y2, base.y2 };

	// 조각별 이동 방향 정하기
	float ddx[4], ddy[4];
	if (mode == 0)          // 1번: 좌우상하 (좌 우 하 상)
	{
		float ax[4] = { -d, d, 0, 0 };
		float ay[4] = { 0, 0, -d, d };
		for (int k = 0; k < 4; ++k) { ddx[k] = ax[k]; ddy[k] = ay[k]; }
	}
	else if (mode == 1)     // 2번: 대각선
	{
		float ax[4] = { -d, d, -d, d };
		float ay[4] = { -d, -d, d, d };
		for (int k = 0; k < 4; ++k) { ddx[k] = ax[k]; ddy[k] = ay[k]; }
	}
	else                    // 3번: 넷이 한 방향으로 같이 (랜덤 한 방향)
	{
		int dir = rand() % 8;
		for (int k = 0; k < 4; ++k) { ddx[k] = dirx[dir]; ddy[k] = diry[dir]; }
	}

	for (int k = 0; k < 4; ++k)
	{
		Rect p = base;                 // 색 복사
		p.x1 = qx1[k]; p.y1 = qy1[k];
		p.x2 = qx2[k]; p.y2 = qy2[k];
		p.dx = ddx[k]; p.dy = ddy[k];
		p.moving = true; p.alive = true;
		rects[count] = p; count++;
	}
	rects[i].alive = false;            // 원래 건 사라짐
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
	count = rand() % 5 + 5;    // 5 ~ 9개
	for (int i = 0; i < count; ++i)
	{
		makerect(i);
	}
	// 콜백 등록
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	// 메인 루프
	while (!glfwWindowShouldClose(window))
	{
		// 화면 지우기
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float s = 0.0008f;   // 축소 속도
		for (int i = 0; i < count; ++i)
		{
			if (rects[i].alive == false) continue;

			if (rects[i].moving)
			{
				// 이동
				rects[i].x1 += rects[i].dx; rects[i].x2 += rects[i].dx;
				rects[i].y1 += rects[i].dy; rects[i].y2 += rects[i].dy;
				// 축소 (네 변을 안으로)
				rects[i].x1 += s; rects[i].x2 -= s;
				rects[i].y1 += s; rects[i].y2 -= s;
				// 특정 크기 이하 -> 사라짐
				if (rects[i].x2 - rects[i].x1 <= 0.0f) rects[i].alive = false;
				// 색 어둡게 (밝게 하려면 += 로)
				rects[i].r -= 0.003f; if (rects[i].r < 0) rects[i].r = 0;
				rects[i].g -= 0.003f; if (rects[i].g < 0) rects[i].g = 0;
				rects[i].b -= 0.003f; if (rects[i].b < 0) rects[i].b = 0;
			}

			glColor3f(rects[i].r, rects[i].g, rects[i].b);
			glRectf(rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
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

		for (int i = 0; i < count; ++i)
		{
			if (rects[i].alive == false) continue;

			if (gx <= rects[i].x2 && gx >= rects[i].x1 && gy <= rects[i].y2 && gy >= rects[i].y1)
			{
				split4(i);
				break;    // 하나만 나누고 끝
			}
		}
	}
}
