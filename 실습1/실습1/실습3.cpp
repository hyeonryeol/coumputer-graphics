#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
// 콜백 선언
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double x, double y);
float r1[20] = {};
float g1[20] = {};
float b1[20] = {};
float rx1[20] = {};
float ry1[20] = {};
float rx2[20] = {};
float ry2[20] = {};
int acount = 0;
int   dragIndex = -1;   // 잡은 사각형 번호
float lastX, lastY;     // 직전 마우스 위치

void removeRect(int idx)
{
    for (int i = idx; i < acount - 1; ++i)
    {
        rx1[i] = rx1[i + 1];  ry1[i] = ry1[i + 1];
        rx2[i] = rx2[i + 1];  ry2[i] = ry2[i + 1];
        r1[i] = r1[i + 1];   g1[i] = g1[i + 1];   b1[i] = b1[i + 1];
    }
    
}
void makeRandRect(int idx, float cx, float cy)
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
    srand((unsigned)time(NULL));
    for (int i = 0; i < 10;++i)
    {
        r1[i] = rand() / (float)RAND_MAX;
        g1[i] = rand() / (float)RAND_MAX;
        b1[i] = rand() / (float)RAND_MAX;
        float cx = rand() / (float)RAND_MAX * 2.0f - 1.0f;   // 중심
        float cy = rand() / (float)RAND_MAX * 2.0f - 1.0f;
        float hw = 0.05f + rand() / (float)RAND_MAX * 0.1f;  // 반폭 
        float hh = 0.05f + rand() / (float)RAND_MAX * 0.1f;

        rx1[i] = cx - hw;  ry1[i] = cy - hh;   // 좌하단 
        rx2[i] = cx + hw;  ry2[i] = cy + hh;   // 우상단 
    }
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
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // 메인 루프
    while (!glfwWindowShouldClose(window))
    {
        // 화면 지우기
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 여기에 그리기 코드
        for (int i = 0; i < acount; ++i)
        {
        glColor3f(r1[i], g1[i], b1[i]);
        glRectf(rx1[i], ry1[i], rx2[i], ry2[i]);

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
        case GLFW_KEY_A:
        {
            if (acount < 10)
            {
            acount++;

            }
            break;
        }
        }
    }
}
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    float gx = (float)(x / 800.0 * 2.0 - 1.0);
    float gy = (float)(1.0 - y / 600.0 * 2.0);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // 위에 있는 것부터 찾아 하나만 잡기
        for (int i = acount - 1; i >= 0; --i)
        {
            if (gx >= rx1[i] && gx <= rx2[i] && gy >= ry1[i] && gy <= ry2[i])
            {
                dragIndex = i;       // 이 사각형을 잡음
                lastX = gx;          // 시작 위치 기억
                lastY = gy;
                break;
            }
        }
    }
  
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        int d = dragIndex;      // 방금 놓은 사각형
        dragIndex = -1;

        if (d != -1)
        {
            // 다른 사각형과 겹치는지 검사
            for (int i = 0; i < acount; ++i)
            {
                if (i == d) continue;    // 자기 자신은 건너뜀

                bool overlap =
                    rx1[d] < rx2[i] && rx2[d] > rx1[i] &&
                    ry1[d] < ry2[i] && ry2[d] > ry1[i];

                if (overlap)
                {
                    // 두 사각형을 감싸는 큰 사각형 (i 자리에 저장)
                    rx1[i] = (rx1[d] < rx1[i]) ? rx1[d] : rx1[i];   // 더 작은 x1
                    ry1[i] = (ry1[d] < ry1[i]) ? ry1[d] : ry1[i];
                    rx2[i] = (rx2[d] > rx2[i]) ? rx2[d] : rx2[i];   // 더 큰 x2
                    ry2[i] = (ry2[d] > ry2[i]) ? ry2[d] : ry2[i];

                    // 색 랜덤
                    r1[i] = rand() / (float)RAND_MAX;
                    g1[i] = rand() / (float)RAND_MAX;
                    b1[i] = rand() / (float)RAND_MAX;

                    // d번 사각형 삭제 (배열에서 빼기)
                    removeRect(d);
                    acount--;
                    break;      // 하나 합쳤으면 끝
                }
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        if (acount >= 20) return;    // 20개 넘으면 분리 안 함

        // 위에 있는 것부터 찾아 하나 선택
        int hit = -1;
        for (int i = acount - 1; i >= 0; --i)
        {
            if (gx >= rx1[i] && gx <= rx2[i] && gy >= ry1[i] && gy <= ry2[i])
            {
                hit = i;
                break;
            }
        }

        if (hit != -1)
        {
            // 원본 사각형의 중심을 기준으로 두 개 생성
            float cx = (rx1[hit] + rx2[hit]) / 2.0f;
            float cy = (ry1[hit] + ry2[hit]) / 2.0f;

            // 첫 번째 (원본 자리에 덮어쓰기)
            makeRandRect(hit, cx, cy);

            // 두 번째 (배열 맨 뒤에 추가)
            makeRandRect(acount, cx, cy);
            acount++;
        }
    }
}
void CursorPosCallback(GLFWwindow* window, double x, double y)
{
    if (dragIndex == -1) return;    // 잡은 게 없으면 무시

    float gx = (float)(x / 800.0 * 2.0 - 1.0);
    float gy = (float)(1.0 - y / 600.0 * 2.0);

    float dx = gx - lastX;          // 얼마나 움직였나
    float dy = gy - lastY;

    // 잡은 사각형을 그만큼 이동
    rx1[dragIndex] += dx;  rx2[dragIndex] += dx;
    ry1[dragIndex] += dy;  ry2[dragIndex] += dy;

    lastX = gx;                     // 위치 갱신
    lastY = gy;
}