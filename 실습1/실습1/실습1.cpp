#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
//--- 함수 선언 (main보다 아래에 정의된 함수는 미리 선언해야 호출 가능)
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
float r = 1;
float g = 1;
float b = 1;
bool   timeron = false;  // 타이머 켜짐 여부
double lastChange = 0.0;    // 마지막으로 색 바꾼 시각(초)
double interval = 0.5;    // 몇 초마다 바꿀지

int main()
{
	// main 시작 부분에서 한 번만 호출
	srand((unsigned)time(NULL));

	
	//--- GLFW 초기화
	if (!glfwInit())
		return -1;
	//--- OpenGL 버전 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//--- 윈도우 생성
	GLFWwindow* window = glfwCreateWindow(800, 600, "Keyboard Input", nullptr, nullptr);
	//--- 컨텍스트 활성화
	glfwMakeContextCurrent(window);
	//--- GLEW 초기화
	glewExperimental = GL_TRUE;
	glewInit();
	//--- 뷰포트 설정
	glViewport(0, 0, 800, 600);
	//--- 키보드 입력 처리 함수
	glfwSetKeyCallback(window, KeyCallback); //--- 콜백 함수 등록
	//--- 메인 루프
	while (!glfwWindowShouldClose(window)) {
		if (timeron)
		{
			double now = glfwGetTime();
			if (now - lastChange >= interval)
			{
				r = rand() / (float)RAND_MAX;
				g = rand() / (float)RAND_MAX;
				b = rand() / (float)RAND_MAX;
				lastChange = now;
			}
		}


		//--- 화면 그리기
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
//--- End of Main function
//--- 키보드 입력 처리 함수
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) //--- 등록한 사용자 정의 콜백 함수
{
	if (action == GLFW_PRESS)
	{
		switch (key) {
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
		case GLFW_KEY_C: r = 0; g = 1; b = 1;

			break;
		case GLFW_KEY_M:;
			r = 1; g = 0; b = 1;
			break;
		case GLFW_KEY_Y:;
			r = 1; g = 1; b = 0;
			break;
		case GLFW_KEY_A:;
			r = rand() / (float)RAND_MAX; g = rand() / (float)RAND_MAX; b = rand() / (float)RAND_MAX;
			break;
		case GLFW_KEY_G:;
			r = 0.5; g = 0.5; b = 0.5;
			break;
		case GLFW_KEY_K:;
			r = 0; g = 0; b = 0;
			break;
		case GLFW_KEY_T:;
			timeron = true;
			lastChange = glfwGetTime();
			break;
		case GLFW_KEY_S:;
			timeron = false;
			break;
		}
	}
}
