// =====================================================================
// 시험장 라이브러리 설치  틀 붙이기 전에 먼저
// =====================================================================
// 받을 것
//   GLEW  glew.sourceforge.net  Windows 32-bit and 64-bit Binaries zip
//   GLFW  glfw.org Download  64-bit Windows binaries zip
//   GLM   github.com/g-truc/glm  Releases zip
//
// 기준 폴더  C:\Program Files (x86)\Windows Kits\10 폴더
//   Include 와 Lib 아래 버전 폴더 하나 있음  이 컴퓨터는 10.0.26100.0  시험장은 숫자 다를 수 있음 있는 것 쓰기
//
// 넣을 곳
//   glew.h wglew.h 등       glew include\GL       →  Include\버전\um\GL 폴더
//   glfw3.h glfw3native.h   glfw include\GLFW     →  Include\버전\um\GL 폴더   GLFW 폴더 말고 GL 폴더
//   glm 폴더 통째로         glm 압축 안 glm 폴더  →  Include\버전\um\GL\glm
//   glew32.lib              glew lib\Release\x64  →  Lib\버전\um\x64 폴더
//   glfw3.lib               glfw lib-vc2022       →  Lib\버전\um\x64 폴더
//   glew32.dll              glew bin\Release\x64  →  C:\Windows\System32 폴더
//   glfw3.h 를 GL 폴더에 넣는 이유  틀이 #include <GL/glfw3.h> 로 부름
//
// 확인  새 프로젝트에 코테틀 붙이고 cube.obj 같은 폴더  Ctrl F5  파란 육면체 나오면 성공
//   틀 맨 위 #pragma comment 세 줄이 링커 설정 대신이라 프로젝트 설정 안 건드려도 됨
//
// 잘 나는 문제
//   glew.h 를 열 수 없음      헤더를 다른 버전 폴더나 GL 폴더 밖에 넣음
//   LNK2019                   lib 를 x86 폴더에 넣음 x64 로  또는 VS 위쪽 플랫폼 x86 을 x64 로
//   glew32.dll 없다고 뜸      System32 에 dll 없음  권한 없으면 dll 을 cpp 있는 프로젝트 폴더에
//   복사할 때 권한 거부       Program Files System32 는 관리자 권한 필요  계속 누르기


// =====================================================================
// 컴그 코테 창고 obj 버전
// =====================================================================
// 기능 복붙용 창고 이 파일 혼자서는 실행 안 됨
// 코테틀.cpp 를 먼저 붙이고 여기서 필요한 함수만 골라 틀의 [창고 붙이는 자리] 에 붙여넣기
// 3D 도형은 전부 obj 파일  cube sphere pyramid cylinder cone  전부 크기 0.6 가운데가 원점
//
// 쓰는 법
//   1. 새 cpp 에 코테틀.cpp 통째로 붙여넣기 obj 파일들 같은 폴더에 넣고 육면체 나오는지 확인
//   2. 프로젝트의 다른 cpp는 빌드에서 제외
//      솔루션 탐색기 파일 오른쪽 클릭 속성 빌드에서 제외 예
//   3. 여기서 필요한 함수만 골라 [창고 붙이는 자리] 에 붙여넣기
//      그 함수가 부르는 함수도 같이  예 drawrobot 은 hinge drawobj 필요
//   4. 틀의 [문제 풀이] 에서 initobj drawScene KeyCallback 고치기 [18] 예제 참고
//   5. 3D 문제면 glEnable GL_DEPTH_TEST 그리고 glClear 에 GL_DEPTH_BUFFER_BIT 확인
//   6. 키 배정은 항상 문제 PDF 와 다시 맞추기 창고 주석의 키는 예전 실습 기준
//
// 틀에 이미 있어서 여기 없는 것 또 붙이면 재정의 에러
//   헤더 전역 변수 공책 셰이더 main  compileshader make_shaderProgram InitBuffer upload
//   setmodel setview setproj set2d setcolor drawwith drawpart
//   addvertex addline addtri addrect mousetogl  Vertex Face loadobj addobj
//
// 목차 Ctrl F로 [번호] 검색  [1] ~ [5] 는 틀
//   [6]  랜덤 좌표 변환 시간
//   [7]  2D 도형 공책에 적기 사각형 원 다각형 별 점선
//   [8]  2D 도형 구조체 선택 충돌 합치기 삭제
//   [9]  2D 애니메이션 튕기기 지그재그 스파이럴 테두리 따라가기 크기 색
//   [10] 2D 손 회전 손 크기 변환
//   [11] 보드판 칸 계산
//   [12] 3D 도형 obj 로 그리기 drawobj 면마다 색 꼭짓점 색 밝기 축 궤도 원 바닥
//   [13] obj 읽기 쓰는 법
//   [14] 3D 변환 조각 자전 공전 제자리 크기 원점 크기 경첩 열기 계층
//   [15] 카메라 투영 뷰포트 여러 개
//   [16] 키보드 마우스 패턴
//   [17] 윈도우 뷰포트 변환 클리핑 계산
//   [18] 예제 장면 틀의 [문제 풀이] 를 통째로 바꾸면 실행됨
//   [19] 자주 나는 실수 체크
// =====================================================================


// =====================================================================
// [6] 랜덤 좌표 변환 시간
// =====================================================================
// 0 ~ 1 실수
float randf()
{
	return rand() / (float)RAND_MAX;
}

// a ~ b 실수
float randrange(float a, float b)
{
	return a + (b - a) * randf();
}

// a ~ b 정수 둘 다 포함
int randint(int a, int b)
{
	return a + rand() % (b - a + 1);
}

// 랜덤 색 세 칸 배열에 넣기 너무 어두우면 안 보이니 최소 0.2
void randcolor(float c[3])
{
	c[0] = 0.2f + 0.8f * randf();
	c[1] = 0.2f + 0.8f * randf();
	c[2] = 0.2f + 0.8f * randf();
}

// mousetogl 은 틀에 있음

// GL 좌표를 픽셀로 거꾸로
void gltomouse(float gx, float gy, double& mx, double& my)
{
	mx = (gx + 1.0) / 2.0 * width;
	my = (1.0 - gy) / 2.0 * height;
}

// 타이머 흉내 일정 시간마다 true
// 사용법
//   static double last = 0;
//   if (every(last, 0.5)) { 0.5초마다 할 일 }
bool every(double& last, double sec)
{
	double now = glfwGetTime();
	if (now - last >= sec)
	{
		last = now;
		return true;
	}
	return false;
}

// 프레임 사이 시간 속도를 초당 값으로 쓰고 싶을 때
// 사용법 float dt = deltatime(); x += speed * dt;
float deltatime()
{
	static double last = glfwGetTime();
	double now = glfwGetTime();
	float dt = (float)(now - last);
	last = now;
	return dt;
}


// =====================================================================
// [7] 2D 도형 공책에 적기
// =====================================================================
// 모든 add 함수는 공책 n 뒤에 이어 적음
// 그리기 모드
//   add 결과가 삼각형 묶음이면 GL_TRIANGLES
//   선 두 점이면 GL_LINES  이어진 테두리면 GL_LINE_LOOP

// addvertex addline addtri addrect 는 틀에 있음

// 사각형 가운데 cx cy 반폭 hw 반높이 hh 로 주기
void addrectc(float cx, float cy, float hw, float hh, float r, float g, float b)
{
	addrect(cx - hw, cy - hh, cx + hw, cy + hh, r, g, b);
}

// 사각형 테두리 GL_LINE_LOOP로 그림 꼭짓점 4개
void addrectline(float x1, float y1, float x2, float y2, float r, float g, float b)
{
	addvertex(x1, y1, 0.0f, r, g, b);
	addvertex(x2, y1, 0.0f, r, g, b);
	addvertex(x2, y2, 0.0f, r, g, b);
	addvertex(x1, y2, 0.0f, r, g, b);
}

// 점 문제에서 점 대신 작은 사각형
void addpoint(float x, float y, float r, float g, float b)
{
	addrectc(x, y, 0.01f, 0.01f, r, g, b);
}

// 이등변 삼각형 가운데 cx cy 크기 s 꼭짓점이 위 실습8 9 모양
void addisotri(float cx, float cy, float s, float r, float g, float b)
{
	addtri(cx - s, cy - s, cx + s, cy - s, cx, cy + s, r, g, b);
}

// 이등변 삼각형 angle 도 만큼 돌린 것 꼭짓점이 진행 방향 볼 때
void addisotrirot(float cx, float cy, float s, float angle, float r, float g, float b)
{
	float ox[3] = { -s, s, 0.0f };
	float oy[3] = { -s, -s, s };
	float c = cos(glm::radians(angle)), sn = sin(glm::radians(angle));
	for (int k = 0; k < 3; k++)
		addvertex(cx + ox[k] * c - oy[k] * sn, cy + ox[k] * sn + oy[k] * c, 0.0f, r, g, b);
}

// 원 채우기 조각 seg개 삼각형 seg개
// 창이 정사각형이 아니면 찌그러짐 그럴 땐 rx 에 r * height / width 넣기
void addcircle(float cx, float cy, float rx, float ry, int seg, float r, float g, float b)
{
	for (int k = 0; k < seg; k++) {
		float a1 = 2.0f * PI * k / seg;
		float a2 = 2.0f * PI * (k + 1) / seg;
		addtri(cx, cy, cx + rx * cos(a1), cy + ry * sin(a1), cx + rx * cos(a2), cy + ry * sin(a2), r, g, b);
	}
}

// 원 테두리 GL_LINE_LOOP 꼭짓점 seg개
void addcircleline(float cx, float cy, float rad, int seg, float r, float g, float b)
{
	for (int k = 0; k < seg; k++) {
		float a = 2.0f * PI * k / seg;
		addvertex(cx + rad * cos(a), cy + rad * sin(a), 0.0f, r, g, b);
	}
}

// 정 k 각형 채우기 첫 꼭짓점이 위쪽 오각형 육각형 등
void addngon(float cx, float cy, float rad, int k, float r, float g, float b)
{
	for (int i = 0; i < k; i++) {
		float a1 = PI / 2 + 2.0f * PI * i / k;
		float a2 = PI / 2 + 2.0f * PI * (i + 1) / k;
		addtri(cx, cy, cx + rad * cos(a1), cy + rad * sin(a1), cx + rad * cos(a2), cy + rad * sin(a2), r, g, b);
	}
}

// 별 꼭짓점 5개 바깥 반지름 ro 안쪽 반지름 ri
void addstar(float cx, float cy, float ro, float ri, float r, float g, float b)
{
	for (int i = 0; i < 10; i++) {
		float a1 = PI / 2 + PI * i / 5;
		float a2 = PI / 2 + PI * (i + 1) / 5;
		float r1 = (i % 2 == 0) ? ro : ri;
		float r2 = (i % 2 == 0) ? ri : ro;
		addtri(cx, cy, cx + r1 * cos(a1), cy + r1 * sin(a1), cx + r2 * cos(a2), cy + r2 * sin(a2), r, g, b);
	}
}

// 화면 가운데 십자선 사분면 나누기 GL_LINES 꼭짓점 4개
void addcross(float r, float g, float b)
{
	addline(-1.0f, 0.0f, 1.0f, 0.0f, r, g, b);
	addline(0.0f, -1.0f, 0.0f, 1.0f, r, g, b);
}

// 점선 x1 y1 에서 x2 y2 까지 dash 길이마다 끊어서 GL_LINES
void adddash(float x1, float y1, float x2, float y2, float dash, float r, float g, float b)
{
	float len = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	int cnt = (int)(len / dash);
	for (int i = 0; i < cnt; i += 2) {
		float t1 = (float)i / cnt, t2 = (float)(i + 1) / cnt;
		addline(x1 + (x2 - x1) * t1, y1 + (y2 - y1) * t1, x1 + (x2 - x1) * t2, y1 + (y2 - y1) * t2, r, g, b);
	}
}

// 사분면 번호 1 오른쪽 위 2 왼쪽 위 3 왼쪽 아래 4 오른쪽 아래
int quadrant(float x, float y)
{
	if (x >= 0 && y >= 0) return 1;
	if (x < 0 && y >= 0) return 2;
	if (x < 0 && y < 0) return 3;
	return 4;
}

// 사분면 가운데 좌표
void quadcenter(int q, float& cx, float& cy)
{
	cx = (q == 1 || q == 4) ? 0.5f : -0.5f;
	cy = (q == 1 || q == 2) ? 0.5f : -0.5f;
}


// =====================================================================
// [8] 2D 도형 구조체 선택 충돌 합치기 삭제
// =====================================================================
// 도형 하나 정보 문제마다 필요한 칸만 추가
struct Shape {
	int type;             // 0 점 1 선 2 삼각형 3 사각형 4 원 등 마음대로
	float x, y;           // 가운데
	float hw, hh;         // 반폭 반높이 삼각형이면 크기 s 로 hw 사용
	float r, g, b;
	float dx, dy;         // 한 프레임 이동량
	float angle;          // 회전 각도 도
	float scale;          // 크기 배율
	float ox, oy;         // 처음 위치 원래 자리로 돌아가기 용
	bool alive;
};
const int MAXS = 50;
Shape shapes[MAXS];
int scount = 0;           // 도형 수
int selected = -1;        // 선택된 도형 번호 없으면 -1

// 도형 하나 만들기 랜덤 크기 색
void makeshape(Shape& s, int type, float x, float y)
{
	s.type = type;
	s.x = s.ox = x;
	s.y = s.oy = y;
	s.hw = randrange(0.05f, 0.12f);
	s.hh = randrange(0.05f, 0.12f);
	s.r = randrange(0.2f, 1.0f);
	s.g = randrange(0.2f, 1.0f);
	s.b = randrange(0.2f, 1.0f);
	s.dx = (randf() < 0.5f ? -1 : 1) * randrange(0.005f, 0.015f);
	s.dy = (randf() < 0.5f ? -1 : 1) * randrange(0.005f, 0.015f);
	s.angle = 0.0f;
	s.scale = 1.0f;
	s.alive = true;
}

// 도형 추가 최대 넘으면 무시 성공하면 번호
int pushshape(int type, float x, float y)
{
	if (scount >= MAXS) return -1;
	makeshape(shapes[scount], type, x, y);
	return scount++;
}

// i번 도형 삭제 뒤에 것들 한 칸씩 당김
void removeshape(int i)
{
	for (int k = i; k < scount - 1; k++)
		shapes[k] = shapes[k + 1];
	scount--;
	if (selected == i) selected = -1;
	else if (selected > i) selected--;
}

// 점이 사각형 안에 있나
bool inrect(float px, float py, float x1, float y1, float x2, float y2)
{
	return px >= x1 && px <= x2 && py >= y1 && py <= y2;
}

// 점이 가운데 기준 사각형 안에 있나
bool inrectc(float px, float py, float cx, float cy, float hw, float hh)
{
	return fabs(px - cx) <= hw && fabs(py - cy) <= hh;
}

// 두 사각형이 겹치나 왼쪽 아래 오른쪽 위 좌표
bool rectoverlap(float ax1, float ay1, float ax2, float ay2, float bx1, float by1, float bx2, float by2)
{
	return ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1;
}

// 두 도형 겹치나 가운데와 반폭으로
bool shapeoverlap(const Shape& a, const Shape& b)
{
	return fabs(a.x - b.x) < a.hw + b.hw && fabs(a.y - b.y) < a.hh + b.hh;
}

// 점이 원 안에 있나
bool incircle(float px, float py, float cx, float cy, float rad)
{
	return (px - cx) * (px - cx) + (py - cy) * (py - cy) <= rad * rad;
}

// 두 원이 부딪치나
bool circlehit(float x1, float y1, float r1, float x2, float y2, float r2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= (r1 + r2) * (r1 + r2);
}

// 점이 삼각형 안에 있나 세 변 기준 부호가 모두 같으면 안
float edgesign(float px, float py, float ax, float ay, float bx, float by)
{
	return (px - bx) * (ay - by) - (ax - bx) * (py - by);
}
bool intri(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3)
{
	float d1 = edgesign(px, py, x1, y1, x2, y2);
	float d2 = edgesign(px, py, x2, y2, x3, y3);
	float d3 = edgesign(px, py, x3, y3, x1, y1);
	bool neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
	return !(neg && pos);
}

// 점에서 선분까지 거리 선 클릭 선택용 가까우면 선택
float distseg(float px, float py, float x1, float y1, float x2, float y2)
{
	float vx = x2 - x1, vy = y2 - y1;
	float t = ((px - x1) * vx + (py - y1) * vy) / (vx * vx + vy * vy + 1e-6f);
	t = glm::clamp(t, 0.0f, 1.0f);
	float qx = x1 + vx * t, qy = y1 + vy * t;
	return sqrt((px - qx) * (px - qx) + (py - qy) * (py - qy));
}

// 클릭한 곳의 맨 위 도형 찾기 나중에 그린 게 위라서 뒤에서부터 검사 없으면 -1
int pickshape(float gx, float gy)
{
	for (int i = scount - 1; i >= 0; i--) {
		Shape& s = shapes[i];
		float hw = s.hw * s.scale, hh = s.hh * s.scale;
		if (inrectc(gx, gy, s.x, s.y, hw, hh))
			return i;
	}
	return -1;
}

// 선택한 도형 맨 위로 올리기 배열 맨 뒤로 보내기
int bringtofront(int i)
{
	Shape t = shapes[i];
	for (int k = i; k < scount - 1; k++)
		shapes[k] = shapes[k + 1];
	shapes[scount - 1] = t;
	return scount - 1;
}

// 두 사각형 감싸는 큰 사각형으로 합치기 a 자리에 저장 b는 지우기 실습3
void mergeshape(int a, int b)
{
	float x1 = glm::min(shapes[a].x - shapes[a].hw, shapes[b].x - shapes[b].hw);
	float y1 = glm::min(shapes[a].y - shapes[a].hh, shapes[b].y - shapes[b].hh);
	float x2 = glm::max(shapes[a].x + shapes[a].hw, shapes[b].x + shapes[b].hw);
	float y2 = glm::max(shapes[a].y + shapes[a].hh, shapes[b].y + shapes[b].hh);
	shapes[a].x = (x1 + x2) / 2;  shapes[a].y = (y1 + y2) / 2;
	shapes[a].hw = (x2 - x1) / 2; shapes[a].hh = (y2 - y1) / 2;
	shapes[a].r = randf(); shapes[a].g = randf(); shapes[a].b = randf();
	removeshape(b);
}

// 사각형 하나를 네 조각으로 쪼개기 실습6 조각마다 날아갈 방향 dirs
// 사용법 splitfour(i, 0) 좌우상하  splitfour(i, 1) 대각선
void splitfour(int i, int mode)
{
	Shape base = shapes[i];
	removeshape(i);
	float qx[4] = { -1, 1, -1, 1 }, qy[4] = { 1, 1, -1, -1 };     // 네 조각 자리
	float sx[4] = { 0, 0, -1, 1 }, sy[4] = { 1, -1, 0, 0 };       // 상하좌우
	for (int k = 0; k < 4; k++) {
		int j = pushshape(base.type, base.x + qx[k] * base.hw / 2, base.y + qy[k] * base.hh / 2);
		if (j < 0) return;
		shapes[j].hw = base.hw / 2;  shapes[j].hh = base.hh / 2;
		shapes[j].r = base.r; shapes[j].g = base.g; shapes[j].b = base.b;
		float ddx = (mode == 0) ? sx[k] : qx[k];
		float ddy = (mode == 0) ? sy[k] : qy[k];
		shapes[j].dx = ddx * 0.01f;
		shapes[j].dy = ddy * 0.01f;
	}
}

// 도형 전부 공책에 적기 type별로 모양 다르게
// 사각형 type 3 삼각형 type 2 원 type 4 나머지는 점
void addshapes()
{
	for (int i = 0; i < scount; i++) {
		Shape& s = shapes[i];
		if (!s.alive) continue;
		float hw = s.hw * s.scale, hh = s.hh * s.scale;
		if (s.type == 3) addrectc(s.x, s.y, hw, hh, s.r, s.g, s.b);
		else if (s.type == 2) addisotrirot(s.x, s.y, hw, s.angle, s.r, s.g, s.b);
		else if (s.type == 4) addcircle(s.x, s.y, hw, hw, 24, s.r, s.g, s.b);
		else addpoint(s.x, s.y, s.r, s.g, s.b);
	}
}


// =====================================================================
// [9] 2D 애니메이션
// =====================================================================
// 매 프레임 drawScene 안에서 한 번 불러서 위치 바꾸기

// 튕기기 대각선 이동 벽에 닿으면 그 축 방향만 반대로
void movebounce(Shape& s)
{
	s.x += s.dx;
	s.y += s.dy;
	if (s.x + s.hw >= 1.0f || s.x - s.hw <= -1.0f) s.dx = -s.dx;
	if (s.y + s.hh >= 1.0f || s.y - s.hh <= -1.0f) s.dy = -s.dy;
	s.x = glm::clamp(s.x, -1.0f + s.hw, 1.0f - s.hw);   // 벽 안으로 넣어서 끼임 방지
	s.y = glm::clamp(s.y, -1.0f + s.hh, 1.0f - s.hh);
}

// 가로 지그재그 좌우로 가다가 벽에 닿으면 한 줄 아래로 내려가고 방향 반대
// 아래 끝에 닿으면 위로 올라가기 시작
void movezigzag(Shape& s, float rowstep)
{
	s.x += s.dx;
	if (s.x + s.hw >= 1.0f || s.x - s.hw <= -1.0f)
	{
		s.dx = -s.dx;
		s.x = glm::clamp(s.x, -1.0f + s.hw, 1.0f - s.hw);
		s.y += (s.dy > 0 ? rowstep : -rowstep);
		if (s.y + s.hh >= 1.0f || s.y - s.hh <= -1.0f) s.dy = -s.dy;
	}
}

// 상하 뾰족 지그재그 위아래는 빠르게 옆으로는 느리게 실습9 3번
void movesharpzigzag(Shape& s)
{
	s.x += s.dx / 4;
	s.y += s.dy;
	if (s.x + s.hw >= 1.0f || s.x - s.hw <= -1.0f) s.dx = -s.dx;
	if (s.y + s.hh >= 1.0f || s.y - s.hh <= -1.0f) s.dy = -s.dy;
}

// 원 스파이럴 각도 돌면서 반지름이 커졌다 작아졌다
// theta radius rdir 는 도형마다 따로 들고 있어야 함
void movespiral(float& x, float& y, float& theta, float& radius, int& rdir, float cx, float cy)
{
	theta += 0.05f;
	radius += rdir * 0.002f;
	if (radius >= 0.9f) rdir = -1;
	if (radius <= 0.05f) rdir = 1;
	x = cx + radius * cos(theta);
	y = cy + radius * sin(theta);
}

// 원 궤도 그대로 돌기
void movecircle(float& x, float& y, float& theta, float cx, float cy, float radius, float speed)
{
	theta += speed;
	x = cx + radius * cos(theta);
	y = cy + radius * sin(theta);
}

// 창 테두리 따라 시계 방향으로 돌기 실습4 3번
// 위 변은 오른쪽 오른 변은 아래 아래 변은 왼쪽 왼 변은 위로
void moveedge(Shape& s, float speed)
{
	float L = -1.0f + s.hw, R = 1.0f - s.hw, B = -1.0f + s.hh, T = 1.0f - s.hh;
	if (s.y >= T && s.x < R) { s.y = T; s.x = glm::min(s.x + speed, R); }
	else if (s.x >= R && s.y > B) { s.x = R; s.y = glm::max(s.y - speed, B); }
	else if (s.y <= B && s.x > L) { s.y = B; s.x = glm::max(s.x - speed, L); }
	else if (s.x <= L && s.y < T) { s.x = L; s.y = glm::min(s.y + speed, T); }
	else { s.y = glm::min(s.y + speed, T); }     // 테두리에 아직 없으면 위로 붙기
}

// 목표 지점으로 조금씩 다가가기 도착하면 true 원래 자리로 m 키 실습4
bool moveto(float& x, float& y, float tx, float ty, float speed)
{
	float dx = tx - x, dy = ty - y;
	float d = sqrt(dx * dx + dy * dy);
	if (d <= speed) { x = tx; y = ty; return true; }
	x += dx / d * speed;
	y += dy / d * speed;
	return false;
}

// 크기 커졌다 작아졌다 반복 dir 는 1 또는 -1 저장해 두기
// 사용법 전역  float lowscale = 1.0f; int lowdir = 1; bool lowanim = false;
//       drawScene 에서  if (lowanim) pulse(lowscale, lowdir, 0.5f, 1.5f, 0.02f);
void pulse(float& sc, int& dir, float lo, float hi, float step)
{
	sc += step * dir;
	if (sc >= hi) { sc = hi; dir = -1; }
	if (sc <= lo) { sc = lo; dir = 1; }
}

// 색 점점 어둡게 0 이 되면 멈춤 실습6
void darken(float& r, float& g, float& b, float step)
{
	r = glm::max(r - step, 0.0f);
	g = glm::max(g - step, 0.0f);
	b = glm::max(b - step, 0.0f);
}

// 색 점점 밝게 흰색 쪽으로
void brighten(float& r, float& g, float& b, float step)
{
	r = glm::min(r + step, 1.0f);
	g = glm::min(g + step, 1.0f);
	b = glm::min(b + step, 1.0f);
}

// 두 값 사이 t 만큼 t 는 0 ~ 1 애니메이션 진행도
float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

// 자취 남기기 지나온 점 저장 GL_LINE_STRIP으로 그림 실습9
const int MAXTRAIL = 2000;
float trailx[MAXTRAIL], traily[MAXTRAIL];
int trailn = 0;
void recordtrail(float x, float y)
{
	if (trailn < MAXTRAIL) { trailx[trailn] = x; traily[trailn] = y; trailn++; }
}
void addtrail(float r, float g, float b)
{
	for (int i = 0; i < trailn; i++)
		addvertex(trailx[i], traily[i], 0.0f, r, g, b);
}


// =====================================================================
// [10] 2D 손 회전 손 크기 변환
// =====================================================================
// 셰이더 행렬 안 쓰고 좌표를 직접 바꾸는 방법

// 점 px py 를 가운데 cx cy 기준으로 angle 도 돌리기
void rotatepoint(float& px, float& py, float cx, float cy, float angle)
{
	float a = glm::radians(angle);
	float x = px - cx, y = py - cy;
	px = cx + x * cos(a) - y * sin(a);
	py = cy + x * sin(a) + y * cos(a);
}

// 점을 가운데 기준으로 s 배
void scalepoint(float& px, float& py, float cx, float cy, float s)
{
	px = cx + (px - cx) * s;
	py = cy + (py - cy) * s;
}

// 셰이더 행렬로 2D 도형 하나 제자리에서 돌리고 키우기
// 도형을 원점 기준으로 공책에 적고 이 행렬로 위치를 줌
glm::mat4 model2d(float x, float y, float angle, float s)
{
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
	m = glm::rotate(m, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	m = glm::scale(m, glm::vec3(s, s, 1.0f));
	return m;
}


// =====================================================================
// [11] 보드판 칸 계산
// =====================================================================
// 화면을 cols x rows 칸으로 나눈 보드 실습11 20 x 20
const int COLS = 20, ROWS = 20;

// 칸 가운데 좌표 col 0 이 왼쪽 row 0 이 위
void cellcenter(int col, int row, float& x, float& y)
{
	float cw = 2.0f / COLS, ch = 2.0f / ROWS;
	x = -1.0f + cw * (col + 0.5f);
	y = 1.0f - ch * (row + 0.5f);
}

// 좌표가 몇 번째 칸인가
void cellof(float x, float y, int& col, int& row)
{
	col = (int)((x + 1.0f) / 2.0f * COLS);
	row = (int)((1.0f - y) / 2.0f * ROWS);
	col = glm::clamp(col, 0, COLS - 1);
	row = glm::clamp(row, 0, ROWS - 1);
}

// 보드 격자선 GL_LINES
void addgrid(float r, float g, float b)
{
	for (int i = 0; i <= COLS; i++) {
		float x = -1.0f + 2.0f * i / COLS;
		addline(x, -1.0f, x, 1.0f, r, g, b);
	}
	for (int j = 0; j <= ROWS; j++) {
		float y = -1.0f + 2.0f * j / ROWS;
		addline(-1.0f, y, 1.0f, y, r, g, b);
	}
}

// 가까운 칸 가운데로 딱 붙이기 모양판 맞추기 실습10
void snaptocell(float& x, float& y)
{
	int c, r;
	cellof(x, y, c, r);
	cellcenter(c, r, x, y);
}


// =====================================================================
// [12] 3D 도형 obj 로 그리기
// =====================================================================
// obj 파일 cube sphere pyramid cylinder cone 전부 크기 0.6  -0.3 ~ 0.3  가운데가 원점
// 읽기는 틀 initobj 에서 loadobj  공책 적기는 아래 add 함수  그리기는 drawobj
// 면은 바깥에서 볼 때 반시계 순서 컬링 써도 제대로 보임

// obj 한 변 길이
const float OBJSIZE = 0.6f;

// 원하는 실제 크기 s 로 그리기
// 사용법 drawobj(m, glm::vec3(1.0f, 0.2f, 1.0f), start, count)  가로 1 높이 0.2 깊이 1
//       drawobj(m, glm::vec3(0.5f), start, count)  지름 0.5 구
// 칸 순서  drawobj( 행렬 위치 회전,  실제 크기 가로 높이 깊이,  공책 시작 번호,  꼭짓점 개수 )
void drawobj(glm::mat4 m, glm::vec3 s, int start, int count)
{
	drawwith(glm::scale(m, s / OBJSIZE), GL_TRIANGLES, start, count);
}

// obj 삼각형 일부만 한 색으로 적기 first 번째 삼각형부터 cnt 개
// 사용법 공책 적기  int s = n; addobjpart(cubev, cubef, 0, 2, 1, 0, 0);   앞면 삼각형 2개만 빨강
void addobjpart(const std::vector<Vertex>& vv, const std::vector<Face>& ff, int first, int cnt, float r, float g, float b)
{
	for (int i = first; i < first + cnt && i < (int)ff.size(); i++) {
		int id[3] = { ff[i].a, ff[i].b, ff[i].c };
		for (int j = 0; j < 3; j++)
			addvertex(vv[id[j]].x, vv[id[j]].y, vv[id[j]].z, r, g, b);
	}
}

// 한 색인데 면 방향마다 밝기 다르게 구 원기둥 육면체가 입체로 보임
// addobj 대신 이것 쓰면 한 색 obj 도 모서리가 보임
// 사용법 공책 적기  int sp = n; addobjshade(spherev, spheref, 0, 0, 1); int spc = n - sp;
//       그리기     drawobj(m, glm::vec3(0.5f), sp, spc);
// 칸 순서  addobjshade( obj 꼭짓점,  obj 면,  r,  g,  b )
void addobjshade(const std::vector<Vertex>& vv, const std::vector<Face>& ff, float r, float g, float b)
{
	glm::vec3 light = glm::normalize(glm::vec3(0.4f, 0.8f, 0.6f));
	for (const Face& f : ff) {
		glm::vec3 p0(vv[f.a].x, vv[f.a].y, vv[f.a].z);
		glm::vec3 p1(vv[f.b].x, vv[f.b].y, vv[f.b].z);
		glm::vec3 p2(vv[f.c].x, vv[f.c].y, vv[f.c].z);
		glm::vec3 nrm = glm::normalize(glm::cross(p1 - p0, p2 - p0));
		float s = 0.55f + 0.45f * glm::max(glm::dot(nrm, light), 0.0f);
		float cr = glm::min(r * s, 1.0f), cg = glm::min(g * s, 1.0f), cb = glm::min(b * s, 1.0f);
		addvertex(p0.x, p0.y, p0.z, cr, cg, cb);
		addvertex(p1.x, p1.y, p1.z, cr, cg, cb);
		addvertex(p2.x, p2.y, p2.z, cr, cg, cb);
	}
}

// cube.obj 면 순서 앞 뒤 오른 왼 위 아래 면마다 삼각형 2개 꼭짓점 6개
const int F_FRONT = 0, F_BACK = 1, F_RIGHT = 2, F_LEFT = 3, F_TOP = 4, F_BOTTOM = 5;

// 면마다 색 다른 육면체 fc 순서 앞 뒤 오른 왼 위 아래 꼭짓점 36개
// 면 하나만 그리기 drawobj(m, 크기, start + 면번호 * 6, 6)
// 사용법 공책 적기  float fc[6][3] = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {0,1,1}, {1,0,1} };
//                 int cs = n; addcubefaces(cubev, cubef, fc);
//       그리기     drawobj(m, glm::vec3(0.5f), cs, 36);
void addcubefaces(const std::vector<Vertex>& vv, const std::vector<Face>& ff, float fc[6][3])
{
	for (int k = 0; k < 6; k++)
		addobjpart(vv, ff, k * 2, 2, fc[k][0], fc[k][1], fc[k][2]);
}

// pyramid.obj 면 순서 앞 오른 뒤 왼 삼각형 1개씩 그다음 바닥 삼각형 2개 꼭짓점 18개
// 옆면 하나 drawobj(m, 크기, start + k * 3, 3)  바닥 start + 12, 6
// 사용법 공책 적기  float pc[5][3] = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {0.5f,0.5f,0.5f} };
//                 int ps = n; addpyramidfaces(pyv, pyf, pc);
//       그리기     drawobj(m, glm::vec3(0.5f), ps, 18);
void addpyramidfaces(const std::vector<Vertex>& vv, const std::vector<Face>& ff, float fc[5][3])
{
	for (int k = 0; k < 4; k++)
		addobjpart(vv, ff, k, 1, fc[k][0], fc[k][1], fc[k][2]);
	addobjpart(vv, ff, 4, 2, fc[4][0], fc[4][1], fc[4][2]);
}

// 꼭짓점마다 색 다른 obj 실습14 색이 면 안에서 섞여 보임 vc 는 obj 꼭짓점 번호 순서
// cube.obj 꼭짓점 번호 x y z 부호
//   0 - - -   1 - - +   2 - + -   3 - + +   4 + - -   5 + - +   6 + + -   7 + + +
// pyramid.obj 꼭짓점 번호 0 앞왼 1 앞오른 2 뒤오른 3 뒤왼 바닥  4 꼭대기
// 사용법 공책 적기  float vc[8][3] = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {0,1,1}, {1,0,1}, {1,1,1}, {0,0,0} };
//                 int cs = n; addobjvertexcolor(cubev, cubef, vc);
//       그리기     drawobj(m, glm::vec3(0.5f), cs, 36);
void addobjvertexcolor(const std::vector<Vertex>& vv, const std::vector<Face>& ff, float vc[][3])
{
	for (const Face& f : ff) {
		int id[3] = { f.a, f.b, f.c };
		for (int j = 0; j < 3; j++)
			addvertex(vv[id[j]].x, vv[id[j]].y, vv[id[j]].z, vc[id[j]][0], vc[id[j]][1], vc[id[j]][2]);
	}
}

// 좌표축 x 빨강 y 초록 z 파랑 GL_LINES 꼭짓점 6개 길이 len
// 사용법 공책 적기  int ax = n; addaxes(1.5f);
//       그리기     drawwith(world, GL_LINES, ax, 6);
void addaxes(float len)
{
	addvertex(-len, 0, 0, 1, 0, 0); addvertex(len, 0, 0, 1, 0, 0);
	addvertex(0, -len, 0, 0, 1, 0); addvertex(0, len, 0, 0, 1, 0);
	addvertex(0, 0, -len, 0, 0, 1); addvertex(0, 0, len, 0, 0, 1);
}

// xz 평면 원 반지름 1 궤도 그리기 GL_LINE_LOOP 꼭짓점 seg개 scale로 반지름 조절
// 사용법 공책 적기  int orb = n; addorbit(72, 0.5f, 0.5f, 0.5f);
//       그리기     drawwith(glm::scale(기준 행렬, glm::vec3(반지름)), GL_LINE_LOOP, orb, 72);
//                 기준 행렬과 반지름은 그 궤도를 도는 물체의 orbity 와 똑같이
// 칸 순서  addorbit( 원 점 개수,  r,  g,  b )  반지름 1 짜리
void addorbit(int seg, float r, float g, float b)
{
	for (int k = 0; k < seg; k++) {
		float t = 2.0f * PI * k / seg;
		addvertex(cos(t), 0.0f, sin(t), r, g, b);
	}
}

// xz 바닥 판 크기 1 y = 0 삼각형 2개 scale로 크기 조절
// 사용법 공책 적기  int gr = n; addfloor(0.5f, 0.5f, 0.5f);
//       그리기     drawwith(glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 4.0f)), GL_TRIANGLES, gr, 6);   4 x 4 바닥
// 칸 순서  addfloor( r,  g,  b )  크기 1 짜리
void addfloor(float r, float g, float b)
{
	addvertex(-0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, -0.5f, r, g, b);
	addvertex(-0.5f, 0, 0.5f, r, g, b); addvertex(0.5f, 0, -0.5f, r, g, b); addvertex(-0.5f, 0, -0.5f, r, g, b);
}

// 두 색 체크무늬 바닥  칸 하나 크기 1 을 칸 수 x 칸 수 만큼 옮겨 그림  가운데가 원점
// 사용법 공책 적기  int blue = n; addfloor(0.2f, 0.2f, 1.0f);
//                  int white = n; addfloor(0.9f, 0.9f, 0.9f);
//       그리기     drawchecker(glm::mat4(1.0f), 4, blue, white);   4 x 4 바닥
//       y 회전 따라가면 안 되니 기준은 glm::mat4(1.0f)
// 칸 순서  drawchecker( 기준 행렬,  한 줄 칸 수,  첫 색 시작 번호,  둘째 색 시작 번호 )
// 원리  칸 위치는 i - 1.5  1.5 는 칸 수 빼기 1 나누기 2   i + j 가 짝수면 첫 색 홀수면 둘째 색
//   i=0 j=0  위치 -1.5 -1.5  0 짝수 첫 색
//   i=1 j=0  위치 -0.5 -1.5  1 홀수 둘째 색
void drawchecker(glm::mat4 base, int count, int first, int second)
{
	float half = (count - 1) / 2.0f;
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < count; j++)
		{
			glm::mat4 cell = glm::translate(base, glm::vec3(i - half, 0.0f, j - half));
			int start = ((i + j) % 2 == 0) ? first : second;
			drawwith(cell, GL_TRIANGLES, start, 6);
		}
	}
}


// =====================================================================
// [13] obj 읽기 쓰는 법
// =====================================================================
// Vertex Face loadobj addobj 는 틀에 있음 여기는 쓰는 법만
//   1. 틀 [문제 풀이] 위쪽에 담는 곳 추가     std::vector<Vertex> spherev; std::vector<Face> spheref;
//   2. initobj 안에 읽기                     loadobj("sphere.obj", spherev, spheref);
//   3. drawScene 공책 적기 upload 전에        int sp = n; addobjshade(spherev, spheref, 0, 0, 1); int spc = n - sp;
//   4. upload 다음 그리기                    drawobj(m, glm::vec3(0.5f), sp, spc);   지름 0.5
// 같은 obj 를 색만 바꿔 여러 번 적어도 되고 한 번 적고 setcolor(true, r, g, b) 로 덮어 칠해도 됨
// obj 파일은 cpp 와 같은 프로젝트 폴더 VS 에서 Ctrl F5 로 실행 못 찾으면 콘솔에 열기 실패


// =====================================================================
// [14] 3D 변환 조각
// =====================================================================
// 핵심 규칙
//   m = translate(m, ...) 는 m * T  그래서 코드에 나중에 쓴 변환이 정점에 먼저 적용
//   제자리 회전 크기   먼저 translate 로 자리 잡고 그다음 rotate scale
//   공전              먼저 rotate 그다음 translate 거리
//   자식 부품은 부모 행렬에서 이어서 곱함
//   앞 변환에 이어 붙이기  m = translate(m, ...)   glm::mat4(1.0f) 에서 시작하면 앞 변환 버려짐
//     glm::mat4 m = orbity(glm::mat4(1.0f), a2, 1.0f);
//     m = glm::translate(m, glm::vec3(0.0f, 0.5f, 0.0f));     공전 자리에서 위로 0.5
//   같은 기준에서 부품 여러 개  기준을 변수로 두고 부품마다 기준에서 새로 시작
//     glm::mat4 base = orbity(glm::mat4(1.0f), a2, 1.0f);
//     glm::mat4 m1 = glm::translate(base, glm::vec3(-1.0f, 0.5f, 0.0f));   drawobj(m1, ...);
//     glm::mat4 m2 = glm::translate(base, glm::vec3( 1.0f, 0.5f, 0.0f));   drawobj(m2, ...);
//     m 하나로 translate 를 두 번 하면 이동이 쌓임  -1 다음 +1 이면 두 번째는 0 자리

const glm::vec3 AX(1.0f, 0.0f, 0.0f), AY(0.0f, 1.0f, 0.0f), AZ(0.0f, 0.0f, 1.0f);

// 제자리 자전 위치 p 에서 각도만큼 돌기
// 사용법 glm::mat4 mid = spinat(tank, glm::vec3(0, 0.25f, 0), 0.0f, midangle);   y 축만 돌면 ax 는 0
//       drawobj(mid, glm::vec3(0.4f, 0.1f, 0.5f), cs, 36);
//       mid 를 변수로 두고 위에 붙는 부품은 mid 에서 이어서 translate
// 칸 순서  spinat( 기준 행렬,  자리 위치,  x축 각도,  y축 각도 )
glm::mat4 spinat(glm::mat4 base, glm::vec3 p, float ax, float ay)
{
	glm::mat4 m = glm::translate(base, p);
	m = glm::rotate(m, glm::radians(ax), AX);
	m = glm::rotate(m, glm::radians(ay), AY);
	return m;
}

// 원점 중심 y축 공전 반지름 r 각도 a  자기 방향도 같이 돎
// 사용법 glm::mat4 p1 = orbity(glm::mat4(1.0f), a1, 1.0f);   반지름 1 궤도에서 a1 도
//       drawobj(p1, glm::vec3(0.2f), sp, spc);
//       기울어진 궤도  orbity(glm::rotate(world, glm::radians(45.0f), AZ), a2, 1.3f)
//       달 화면에 평행  orbity(glm::rotate(행성 행렬, glm::radians(90.0f), AX), am, 0.3f)
// 칸 순서  orbity( 기준 행렬,  공전 각도,  공전 반지름 )
glm::mat4 orbity(glm::mat4 base, float a, float r)
{
	glm::mat4 m = glm::rotate(base, glm::radians(a), AY);
	return glm::translate(m, glm::vec3(r, 0.0f, 0.0f));
}

// 공전은 하되 자기 방향은 그대로 두기 돌아가는 만큼 다시 거꾸로 돌림 실습19 상부 몸체
// 사용법 glm::mat4 m = orbitkeep(world, a1, glm::vec3(1.0f, 0, 0));   반지름 1 공전 방향은 그대로 //위치
//       drawobj(m, glm::vec3(0.2f), sp, spc); //크기
// 칸 순서  orbitkeep( 기준 행렬 공전 중심,  공전 각도,  중심에서 떨어진 위치 )
glm::mat4 orbitkeep(glm::mat4 base, float a, glm::vec3 offset)
{
	glm::mat4 m = glm::rotate(base, glm::radians(a), AY);
	m = glm::translate(m, offset);
	return glm::rotate(m, glm::radians(-a), AY);
}

// 제자리 크기 a/A 위치 그대로 도형만 커짐
// 사용법 drawobj(scaleinplace(world, glm::vec3(0.5f, 0, 0), s), glm::vec3(0.3f), cs, 36);   s 는 상태 변수 cs는 cubecount
// 칸 순서  scaleinplace( 기준 행렬,  자리 위치,  배율 )  가로 높이 깊이 전부 같은 배율
glm::mat4 scaleinplace(glm::mat4 base, glm::vec3 p, float s)
{
	glm::mat4 m = glm::translate(base, p);
	return glm::scale(m, glm::vec3(s));
}

// 원점 기준 크기 b/B 위치까지 원점에서 멀어지거나 가까워짐
// 사용법 drawobj(scaleorigin(world, glm::vec3(0.5f, 0, 0), s), glm::vec3(0.3f), cs, 36);
// 칸 순서  scaleorigin( 기준 행렬,  자리 위치,  배율 )
glm::mat4 scaleorigin(glm::mat4 base, glm::vec3 p, float s)
{
	glm::mat4 m = glm::scale(base, glm::vec3(s));
	return glm::translate(m, p);
}

// 경첩 기준 회전 pivot 점을 지나는 axis 축으로 angle 도
// 문 열기 면 열기 팔다리 흔들기 포신 깃대 전부 이것
// 칸 순서  hinge( 기준 행렬,  경첩 위치,  회전 축,  각도 )
//   기준 행렬  부품이 붙어 있는 몸체의 행렬  그 몸체 가운데가 0 0 0
//   경첩 위치  붙은 곳이 기준 가운데에서 얼마나 떨어졌나  실제 길이
//              앞면 윗변  0 높이/2 깊이/2     앞면 가운데  0 0 깊이/2     윗면 가운데  0 높이/2 0
//   회전 축    AY 좌우로 흔들기   AX 앞뒤로 까딱   AZ 화면에서 시계처럼
//   각도       상태 변수  반대 방향은 앞에 마이너스
//   결과 행렬은 기준 가운데로 돌아와 있음  기준 가운데에서 부품 가운데까지 translate 한 번 더 하고 drawobj
// 사용법 실습19 포신 깊이 0.5 상부 몸체 up1 앞면에 붙음 좌우로 흔들기
//   glm::mat4 b1 = hinge(up1, glm::vec3(0, 0, 0.25f), AY, barrelangle);
//   drawobj(glm::translate(b1, glm::vec3(0, 0, 0.5f)), glm::vec3(0.05f, 0.05f, 0.5f), bs, 36);   포신 길이 0.5  0.25 + 0.25
//   오른쪽은 hinge(up2, 같은 경첩, AY, -barrelangle)
// 사용법 실습19 깃대 높이 0.1 상부 몸체 윗면에 붙음 앞뒤로 까딱
//   glm::mat4 f1 = hinge(up1, glm::vec3(0, 0.05f, 0), AX, flagangle);
//   drawobj(glm::translate(f1, glm::vec3(0, 0.3f, 0)), glm::vec3(0.1f, 0.5f, 0.1f), fs, 36);   깃대 높이 0.5  0.05 + 0.25
// 사용법 크기 S 육면체 앞면을 윗변 기준으로 열기 실습15 f
//   glm::mat4 f = hinge(cube, glm::vec3(0, S / 2, S / 2), AX, -openangle);
//   drawobj(f, glm::vec3(S), start + F_FRONT * 6, 6);
// 크기 S 사각뿔 앞면을 바닥 모서리 기준으로 열기 실습16
//   drawobj(hinge(py, glm::vec3(0, -S / 2, S / 2), AX, openangle), glm::vec3(S), start + 0 * 3, 3)  양수가 바깥쪽
glm::mat4 hinge(glm::mat4 base, glm::vec3 pivot, glm::vec3 axis, float angle)
{
	glm::mat4 m = glm::translate(base, pivot);
	m = glm::rotate(m, glm::radians(angle), axis);
	return glm::translate(m, -pivot);
}

// 면 하나를 제자리에서 작게 뒷면 사라지기 실습15 b  면 가운데 c 기준
// 사용법 크기 S 육면체 뒷면 가운데 기준으로 작게
//       drawobj(shrinkface(cube, glm::vec3(0, 0, -S / 2), s), glm::vec3(S), cs + F_BACK * 6, 6);
// 칸 순서  shrinkface( 기준 행렬,  면 가운데 위치,  배율 )
glm::mat4 shrinkface(glm::mat4 base, glm::vec3 c, float s)
{
	glm::mat4 m = glm::translate(base, c);
	m = glm::scale(m, glm::vec3(s));
	return glm::translate(m, -c);
}

// 한 변 기준으로 위쪽으로 납작해지며 열리기 문 슬라이드 실습23 앞면
// top 은 윗변 높이 h 는 면 높이 s 는 1 닫힘 0 열림
// 사용법 크기 S 육면체 앞면 위로 접히듯 열기 s 는 1 닫힘 0 열림
//       drawobj(slideup(cube, S / 2, s), glm::vec3(S), cs + F_FRONT * 6, 6);
// 칸 순서  slideup( 기준 행렬,  윗변 높이,  남은 비율 1 닫힘 0 열림 )
glm::mat4 slideup(glm::mat4 base, float top, float s)
{
	glm::mat4 m = glm::translate(base, glm::vec3(0.0f, top, 0.0f));
	m = glm::scale(m, glm::vec3(1.0f, s, 1.0f));
	return glm::translate(m, glm::vec3(0.0f, -top, 0.0f));
}

// 두 물체 자리 바꾸기 진행도 t 0 ~ 1 실습17
// 원점 지나며 직선으로 바꾸기 t 키
// 사용법 전역 float swapt = 0; 키에서 켜고 drawScene 에서 swapt 를 0 에서 1 까지 늘림
//       glm::vec3 a(-0.2f, 0.1f, 0), b(0.2f, 0.1f, 0);
//       glm::mat4 up1 = glm::translate(mid, swapline(a, b, swapt));
//       glm::mat4 up2 = glm::translate(mid, swapline(b, a, swapt));
// 칸 순서  swapline( 출발 위치,  도착 위치,  진행도 0 ~ 1 )  결과는 위치 vec3
glm::vec3 swapline(glm::vec3 from, glm::vec3 to, float t)
{
	return from + (to - from) * t;
}
// 하나는 위로 하나는 아래로 반원 그리며 바꾸기 u 키 up 은 1 또는 -1
// 사용법 glm::mat4 m1 = glm::translate(world, swaparc(a, b, swapt, 0.5f, 1.0f));    위로
//       glm::mat4 m2 = glm::translate(world, swaparc(b, a, swapt, 0.5f, -1.0f));   아래로
// 칸 순서  swaparc( 출발 위치,  도착 위치,  진행도 0 ~ 1,  반원 높이,  1 위로 -1 아래로 )
glm::vec3 swaparc(glm::vec3 from, glm::vec3 to, float t, float h, float up)
{
	glm::vec3 p = from + (to - from) * t;
	p.y += up * h * sin(PI * t);
	return p;
}

// 걷기 팔다리 흔들기 각도 phase 를 매 프레임 증가 max 가 최대 각도
// 사용법 전역 float phase = 0;  drawScene 에서 phase += 0.1f; float sw = swingangle(phase, 30.0f);
// 칸 순서  swingangle( 진행 값 매 프레임 증가,  최대 각도 )  결과는 각도
float swingangle(float phase, float maxangle)
{
	return sin(phase) * maxangle;
}

// 왔다갔다 각도  -limit 과 +limit 사이를 step 씩 오감  그네 진자 문 여닫기
// 사용법 전역  float a1 = 0.0f; int a1dir = 1; bool swinganim = false; float jinja = 30.0f;
//       drawScene 에서  if (swinganim) pingpong(a1, a1dir, jinja, 1.0f);
//       그네  glm::mat4 rope = hinge(beam, glm::vec3(0, 0, 0), AX, a1);
//       진폭 키우기 줄이기는 jinja 만 바꾸면 됨  다음 끝에서 알아서 꺾임
// 칸 순서  pingpong( 각도,  방향 1 또는 -1,  최대 각도,  한 프레임 이동량 )
// 실수  끝에서 바꿀 것은 각도가 아니라 방향  dir 시작값 0 이면 안 움직임
void pingpong(float& a, int& dir, float limit, float step)
{
	a += step * dir;
	if (a >= limit) { a = limit; dir = -1; }
	if (a <= -limit) { a = -limit; dir = 1; }
}

// 컨베이어 벨트 상자 끝에서 사라지고 처음에서 다시 나타나기 실습20
// 원리  벨트가 움직인 거리 offset 은 계속 커짐  fmod 로 경로 길이로 나눈 나머지를 쓰면 끝에서 0 으로 돌아감
//   경로 길이 6.6 일 때  offset 6.5 면 s 6.5 끝 바로 앞   offset 6.7 이면 s 0.1 처음
// 상자마다 간격 gap 만큼 앞뒤로 벌려서 줄지어 감
// 사용법 전역  float beltoffset = 0.0f; bool beltmove = false;
//       drawScene 에서  if (beltmove) beltoffset += 0.01f;
//       키  b 로 beltmove = !beltmove;   리셋은 beltoffset = 0

// 일자 벨트  from 에서 to 로 가다가 끝나면 from 으로
// 사용법 상자 5개 0.8 간격  i 가 클수록 뒤에 섬
//   for (int i = 0; i < 5; i++) {
//       glm::vec3 p = beltline(glm::vec3(-2, 0.15f, 0), glm::vec3(2, 0.15f, 0), beltoffset, 0.8f, i);
//       drawobj(glm::translate(glm::mat4(1.0f), p), glm::vec3(0.3f), box[i], 36);
//   }
// 칸 순서  beltline( 시작 위치,  끝 위치,  벨트가 움직인 거리,  상자 간격,  상자 번호 )
glm::vec3 beltline(glm::vec3 from, glm::vec3 to, float offset, float gap, int i)
{
	float len = glm::length(to - from);
	float s = fmod(offset - i * gap + len * 100.0f, len);   // 음수 안 되게 len 여러 번 더함
	return from + (to - from) * (s / len);
}

// ㄱ자 벨트  start 에서 -z 로 seg 만큼 간 다음 코너에서 +x 로 seg 만큼  실습20 모양
// 위치는 lpathpos  방향은 lpathyaw  코너 앞뒤 0.3 에서 0 도에서 -90 도로 천천히 돎
// 사용법 실습20  start (-1.5, 0.6, 1.8)  seg 3.3  경로 길이 seg * 2
//   for (int i = 0; i < 5; i++) {
//       float s = fmod(beltoffset + (4 - i) * 1.3f, 3.3f * 2);   1번이 맨 앞
//       glm::mat4 box = glm::translate(glm::mat4(1.0f), lpathpos(glm::vec3(-1.5f, 0.6f, 1.8f), 3.3f, s) + glm::vec3(0, 0.15f, 0));
//       box = glm::rotate(box, glm::radians(lpathyaw(3.3f, s)), AY);
//       drawobj(box, glm::vec3(0.3f), boxs[i], 36);
//   }
// 칸 순서  lpathpos( 시작 위치,  한 구간 길이,  경로 위 거리 )   lpathyaw( 한 구간 길이,  경로 위 거리 )
glm::vec3 lpathpos(glm::vec3 start, float seg, float s)
{
	if (s < seg)
		return start + glm::vec3(0.0f, 0.0f, -s);
	return start + glm::vec3(s - seg, 0.0f, -seg);
}
float lpathyaw(float seg, float s)
{
	float a = seg - 0.3f, b = seg + 0.3f;
	if (s <= a) return 0.0f;
	if (s >= b) return -90.0f;
	return -90.0f * (s - a) / (b - a);
}

// 계층 예 로봇 팔다리 실습23 그대로 쓰면 됨
// robot 은 발바닥 위치와 방향  각 start 는 cube.obj 를 그 색으로 적은 시작 번호 꼭짓점 36개
// 사용법 공책에 cube.obj 를 색만 바꿔 다섯 번 적기 body head nose left right
//       drawrobot(glm::translate(glm::mat4(1.0f), glm::vec3(rx, 0, rz)), sw, body, head, nose, left, right);
void drawrobot(glm::mat4 robot, float swing, int bodystart, int headstart, int nosestart, int leftstart, int rightstart)
{
	for (int i = 0; i < 2; i++) {
		float side = (i == 0) ? -1.0f : 1.0f;
		int part = (i == 0) ? leftstart : rightstart;
		// 다리 엉덩이 높이 0.28 기준으로 흔들기
		glm::mat4 leg = hinge(robot, glm::vec3(0.055f * side, 0.28f, 0.0f), AX, swing * side);
		drawobj(glm::translate(leg, glm::vec3(0.055f * side, 0.14f, 0.0f)), glm::vec3(0.08f, 0.28f, 0.08f), part, 36);
		// 팔 어깨 높이 0.56 기준 다리와 반대로
		glm::mat4 arm = hinge(robot, glm::vec3(0.155f * side, 0.56f, 0.0f), AX, -swing * side);
		drawobj(glm::translate(arm, glm::vec3(0.155f * side, 0.44f, 0.0f)), glm::vec3(0.07f, 0.24f, 0.07f), part, 36);
	}
	drawobj(glm::translate(robot, glm::vec3(0.0f, 0.43f, 0.0f)), glm::vec3(0.24f, 0.3f, 0.14f), bodystart, 36);
	glm::mat4 head = glm::translate(robot, glm::vec3(0.0f, 0.66f, 0.0f));
	drawobj(head, glm::vec3(0.16f), headstart, 36);
	drawobj(glm::translate(head, glm::vec3(0.0f, 0.0f, 0.1f)), glm::vec3(0.04f, 0.04f, 0.05f), nosestart, 36);   // 코는 앞 +z
}

// 이동 방향 dx dz 로 몸 돌리는 각도 로봇 탱크가 가는 쪽 보기
// 사용법 robot = glm::rotate(robot, glm::radians(yawof(dx, dz)), AY);   가는 방향으로 몸 돌리기
float yawof(float dx, float dz)
{
	return glm::degrees(atan2(dx, dz));
}

// 행렬이 옮긴 위치 꺼내기 달 궤도 중심을 행성 위치로 할 때 실습18
// 사용법 glm::vec3 c = posof(planet1);   행성 행렬에서 지금 위치 꺼내기
glm::vec3 posof(glm::mat4 m)
{
	return glm::vec3(m * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

// 3D 상자끼리 겹치나 가운데 c 반크기 h
// 사용법 if (boxhit(p1, glm::vec3(0.1f), p2, glm::vec3(0.2f))) { 부딪힘 }
bool boxhit(glm::vec3 c1, glm::vec3 h1, glm::vec3 c2, glm::vec3 h2)
{
	return fabs(c1.x - c2.x) < h1.x + h2.x && fabs(c1.y - c2.y) < h1.y + h2.y && fabs(c1.z - c2.z) < h1.z + h2.z;
}

// 방 안에서 공 튕기기 벽 lo hi 반지름 rad 실습21
// 사용법 전역 glm::vec3 ballp(0), ballv(0.01f, 0.02f, 0.015f);
//       drawScene 에서 bounce3d(ballp, ballv, glm::vec3(-1.0f), glm::vec3(1.0f), 0.1f);   방 -1 ~ 1 공 반지름 0.1
void bounce3d(glm::vec3& p, glm::vec3& v, glm::vec3 lo, glm::vec3 hi, float rad)
{
	p += v;
	for (int k = 0; k < 3; k++) {
		if (p[k] > hi[k] - rad) { p[k] = hi[k] - rad; v[k] = -v[k]; }
		if (p[k] < lo[k] + rad) { p[k] = lo[k] + rad; v[k] = -v[k]; }
	}
}

// 점프 중력 y 높이 vy 속도 ground 발밑 높이 땅이면 true
// 사용법 키에서 if (ry <= 0.0f) rvy = 0.06f;   drawScene 에서 jumpstep(ry, rvy, 0.0f);
bool jumpstep(float& y, float& vy, float ground)
{
	vy -= 0.003f;
	y += vy;
	if (y <= ground) { y = ground; vy = 0.0f; return true; }
	return false;
}


// =====================================================================
// [15] 카메라 투영 뷰포트 여러 개
// =====================================================================
// 투영 원근 직각 바꾸기 키는 문제마다 다름 실습18 은 p 직각 P 원근
// 사용법 setproj(makeproj(perspectiveon, (float)width / height, 1.5f));   bool perspectiveon 은 상태 변수
// 칸 순서  makeproj( true 원근 false 직각,  가로 나누기 세로,  직각일 때 보이는 반높이 )
glm::mat4 makeproj(bool perspective, float aspect, float orthosize)
{
	if (perspective)
		return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);   // fovy 라디안 aspect 가로나누기세로 near far 양수
	return glm::ortho(-orthosize * aspect, orthosize * aspect, -orthosize, orthosize, -100.0f, 100.0f);
}

// 고정 카메라 eye 에서 at 을 봄
// 사용법 setview(lookcam(glm::vec3(0, 2, 5), glm::vec3(0)));   (0 2 5) 에서 원점 보기
// 칸 순서  lookcam( 카메라 위치,  바라보는 점 )
glm::mat4 lookcam(glm::vec3 eye, glm::vec3 at)
{
	return glm::lookAt(eye, at, AY);
}

// 카메라 자전 제자리에서 보는 방향만 돎 yaw 0 이면 -z 를 봄 pitch 는 아래로 숙이는 양
// 사용법 전역 glm::vec3 campos(0, 2, 5); float camyaw = 0;   y 키로 camyaw 증가
//       setview(yawcam(campos, camyaw, 0.3f));
// 칸 순서  yawcam( 카메라 위치,  좌우 도는 각도,  아래로 숙이는 양 )
glm::mat4 yawcam(glm::vec3 pos, float yaw, float pitch)
{
	glm::vec3 dir(-sin(glm::radians(yaw)), -pitch, -cos(glm::radians(yaw)));
	return glm::lookAt(pos, pos + dir, AY);
}

// 카메라 공전 원점 y축 기준 위치 pos 를 a 도 돌림 같이 원점을 바라보게 하려면 yaw 도 a 만큼 더함
// 사용법 전역 float camorbit = 0;   r 키로 camorbit 증가
//       setview(lookcam(orbitpos(glm::vec3(0, 2, 5), camorbit), glm::vec3(0)));
// 칸 순서  orbitpos( 처음 카메라 위치,  공전 각도 )  결과는 위치 vec3
glm::vec3 orbitpos(glm::vec3 pos, float a)
{
	float r = glm::radians(a);
	return glm::vec3(pos.x * cos(r) + pos.z * sin(r), pos.y, -pos.x * sin(r) + pos.z * cos(r));
}

// 쿼터뷰 중심을 비스듬히 위에서 거리 dist 각도 angle 높이 비율 hr
// 사용법 setview(quartercam(5.0f, 45.0f, 0.6f, glm::vec3(0)));
glm::mat4 quartercam(float dist, float angle, float hr, glm::vec3 center)
{
	float a = glm::radians(angle);
	glm::vec3 eye = center + glm::vec3(dist * sin(a), dist * hr, dist * cos(a));
	return glm::lookAt(eye, center, AY);
}

// 뷰포트 하나 자리 잡고 그 칸만 지우기 화면 나누기 실습22
// 사용법 drawScene 안에서 칸마다 setviewport 그다음 setproj setview 그다음 같은 그리기 함수
// 칸 순서  setviewport( 왼쪽 아래 x,  y,  폭,  높이,  배경 r,  g,  b )  픽셀 단위
void setviewport(int x, int y, int w, int h, float r, float g, float b)
{
	glViewport(x, y, w, h);              // 왼쪽 아래가 x y
	glScissor(x, y, w, h);
	glEnable(GL_SCISSOR_TEST);           // 이 칸만 지우기
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
}

// 평면도 위에서 내려다보기 xz 평면 화면 위쪽이 -z
// 사용법 setproj(makeproj(false, aspect, 2.0f)); setview(topview());   평면도 정면도 측면도는 보통 직각 투영
glm::mat4 topview()
{
	return glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
}

// 정면도 앞에서 xy 평면
glm::mat4 frontview()
{
	return glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f), AY);
}

// 측면도 오른쪽에서 zy 평면
glm::mat4 sideview()
{
	return glm::lookAt(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f), AY);
}

// 상태 켜고 끄기 모음
// 은면제거 h        glEnable(GL_DEPTH_TEST) / glDisable(GL_DEPTH_TEST)
// 와이어 솔리드 w/W  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) / GL_FILL
// 컬링              glEnable(GL_CULL_FACE); glCullFace(GL_BACK); glFrontFace(GL_CCW);
// 방 안쪽만 보이게   방 그릴 때 glCullFace(GL_FRONT) 방 안 물체는 다시 GL_BACK
// 점 크기 선 굵기    glPointSize(5.0f); glLineWidth(2.0f);


// =====================================================================
// [16] 키보드 마우스 패턴
// =====================================================================
// 소문자 대문자 구분 shift 감지  mods & GLFW_MOD_SHIFT
//   key 는 x 나 X 나 같은 GLFW_KEY_X  그래서 shift 눌렸는지로 대문자 구분
//   틀 KeyCallback 첫 줄에 이미 있음  bool shift = (mods & GLFW_MOD_SHIFT);
//   쓰는 모양 세 가지 실습14 ~ 17
//     if else 로 나누기
//       case GLFW_KEY_X:
//           if (mods & GLFW_MOD_SHIFT) anglex -= 2;   대문자 X
//           else anglex += 2;                         소문자 x
//           break;
//     한 줄로 값 고르기
//       case GLFW_KEY_X: anglex += shift ? -2 : 2; break;
//     방향만 정하고 애니메이션은 drawScene 에서  실습17
//       case GLFW_KEY_X: spinx = (mods & GLFW_MOD_SHIFT) ? -1 : 1; break;
//       drawScene 에서 anglex += 2.0f * spinx;
//   카메라 z/Z 이동 예  campos.z += shift ? -0.1f : 0.1f;   PRESS 와 REPEAT 블록에
// 누를 때 한 번      if (action == GLFW_PRESS)
// 누르고 있는 동안   if (action == GLFW_PRESS || action == GLFW_REPEAT)
// 켜고 끄기         spin = !spin;
// 같은 키 다시 누르면 멈춤 방향 키
//   int want = shift ? 1 : -1; dir = (dir == want) ? 0 : want;
// 계속 눌림 부드러운 이동 KeyCallback 말고 drawScene 안에서
//   if (glfwGetKey(gwindow, GLFW_KEY_LEFT) == GLFW_PRESS) x -= 0.01f;
// 숫자키  GLFW_KEY_1 ~ GLFW_KEY_9 GLFW_KEY_0   + 는 GLFW_KEY_EQUAL 과 GLFW_KEY_KP_ADD  - 는 GLFW_KEY_MINUS 와 GLFW_KEY_KP_SUBTRACT
// 화살표 GLFW_KEY_LEFT RIGHT UP DOWN   엔터 GLFW_KEY_ENTER   스페이스 GLFW_KEY_SPACE   ESC GLFW_KEY_ESCAPE
// 마우스 오른쪽 GLFW_MOUSE_BUTTON_RIGHT  가운데 GLFW_MOUSE_BUTTON_MIDDLE

// 드래그 상태 마우스 콜백 둘이 같이 씀
bool dragging = false;
float lastgx, lastgy;       // 직전 마우스 GL 좌표
double lastmx, lastmy;      // 직전 마우스 픽셀 좌표


// =====================================================================
// [17] 윈도우 뷰포트 변환 클리핑 계산
// =====================================================================
// 윈도우 xwl ~ xwr  ywb ~ ywt 를 뷰포트 xvl ~ xvr  yvb ~ yvt 로
void windowtoviewport(float xw, float yw, float xwl, float ywb, float xwr, float ywt,
	float xvl, float yvb, float xvr, float yvt, float& xv, float& yv)
{
	float sx = (xvr - xvl) / (xwr - xwl);
	float sy = (yvt - yvb) / (ywt - ywb);
	xv = xvl + (xw - xwl) * sx;
	yv = yvb + (yw - ywb) * sy;
}

// Cohen Sutherland 영역 코드 왼쪽 1 오른쪽 2 아래 4 위 8
int outcode(float x, float y, float xl, float yb, float xr, float yt)
{
	int c = 0;
	if (x < xl) c |= 1;
	if (x > xr) c |= 2;
	if (y < yb) c |= 4;
	if (y > yt) c |= 8;
	return c;
}

// Cohen Sutherland 선분 자르기 안 보이면 false 보이면 잘린 끝점으로 바꿔 줌
bool cohenclip(float& x1, float& y1, float& x2, float& y2, float xl, float yb, float xr, float yt)
{
	int c1 = outcode(x1, y1, xl, yb, xr, yt), c2 = outcode(x2, y2, xl, yb, xr, yt);
	while (true) {
		if ((c1 | c2) == 0) return true;      // 둘 다 안
		if (c1 & c2) return false;            // 같은 쪽 바깥
		int c = c1 ? c1 : c2;                 // 바깥 끝점 하나 골라 경계까지 당기기
		float x = 0, y = 0;
		if (c & 8) { x = x1 + (x2 - x1) * (yt - y1) / (y2 - y1); y = yt; }
		else if (c & 4) { x = x1 + (x2 - x1) * (yb - y1) / (y2 - y1); y = yb; }
		else if (c & 2) { y = y1 + (y2 - y1) * (xr - x1) / (x2 - x1); x = xr; }
		else { y = y1 + (y2 - y1) * (xl - x1) / (x2 - x1); x = xl; }
		if (c == c1) { x1 = x; y1 = y; c1 = outcode(x1, y1, xl, yb, xr, yt); }
		else { x2 = x; y2 = y; c2 = outcode(x2, y2, xl, yb, xr, yt); }
	}
}

// Liang Barsky 선분 자르기 u 구간 ustart ~ uend
bool liangclip(float& x1, float& y1, float& x2, float& y2, float xl, float yb, float xr, float yt)
{
	float dx = x2 - x1, dy = y2 - y1;
	float p[4] = { -dx, dx, -dy, dy };
	float q[4] = { x1 - xl, xr - x1, y1 - yb, yt - y1 };
	float us = 0.0f, ue = 1.0f;
	for (int k = 0; k < 4; k++) {
		if (p[k] == 0) { if (q[k] < 0) return false; continue; }   // 평행한데 밖
		float u = q[k] / p[k];
		if (p[k] < 0) us = glm::max(us, u);    // 밖에서 안으로
		else ue = glm::min(ue, u);             // 안에서 밖으로
	}
	if (us > ue) return false;
	float nx1 = x1 + us * dx, ny1 = y1 + us * dy;
	x2 = x1 + ue * dx; y2 = y1 + ue * dy;
	x1 = nx1; y1 = ny1;
	return true;
}


// =====================================================================
// [18] 예제 장면 drawScene KeyCallback 마우스 콜백
// =====================================================================
// 틀의 [문제 풀이] 를 통째로 지우고 이것으로 바꾸면 실행됨  cube sphere pyramid obj 필요
// 함수 쓰는 예 보는 용도 문제 풀 때는 필요한 부분만
// m 2D 3D 바꾸기
// 2D  왼쪽 클릭 사각형 만들기 드래그 이동  오른쪽 클릭 삭제  1 튕기기  2 지그재그  3 테두리  c 전부 삭제
// 3D  x/X y/Y 자전  r/R 공전  o 앞면 열기  p/P 원근 직각  w/W 와이어 솔리드  h 은면제거  화살표 이동
// q ESC 종료

bool mode3d = true;

// obj 담는 곳
std::vector<Vertex> cubev, spherev, pyv;
std::vector<Face> cubef, spheref, pyf;

// obj 읽기 main 에서 한 번
void initobj()
{
	loadobj("cube.obj", cubev, cubef);
	loadobj("sphere.obj", spherev, spheref);
	loadobj("pyramid.obj", pyv, pyf);
}

// 2D 상태
int anim2d = 0;              // 0 멈춤 1 튕기기 2 지그재그 3 테두리

// 3D 상태
float spinx = 0, spiny = 0;  // 자전 각도
int spinxdir = 0, spinydir = 0;
float orbitangle = 0;        // 공전 각도
int orbitdir = 0;
bool frontopen = false;      // 앞면 열기
float openangle = 0;
bool perspectiveon = true;
bool wireon = false;
bool depthon = true;
float movex = 0, movey = 0;

void resetall()
{
	scount = 0; selected = -1; anim2d = 0;
	spinx = spiny = 0; spinxdir = spinydir = 0;
	orbitangle = 0; orbitdir = 0;
	frontopen = false; openangle = 0;
	perspectiveon = true; wireon = false; depthon = true;
	movex = movey = 0;
}

void draw2d()
{
	// 움직임
	for (int i = 0; i < scount; i++) {
		if (anim2d == 1) movebounce(shapes[i]);
		if (anim2d == 2) movezigzag(shapes[i], 0.1f);
		if (anim2d == 3) moveedge(shapes[i], 0.01f);
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 공책 채우기
	n = 0;
	int crossstart = n;
	addcross(0.6f, 0.6f, 0.6f);
	int shapestart = n;
	addshapes();
	int shapecount = n - shapestart;
	int selstart = n;
	if (selected >= 0) {
		Shape& s = shapes[selected];
		addrectline(s.x - s.hw * s.scale, s.y - s.hh * s.scale, s.x + s.hw * s.scale, s.y + s.hh * s.scale, 0, 0, 0);
	}

	upload();
	set2d();
	glDrawArrays(GL_LINES, crossstart, 4);
	glDrawArrays(GL_TRIANGLES, shapestart, shapecount);
	if (selected >= 0) {
		glLineWidth(3.0f);
		glDrawArrays(GL_LINE_LOOP, selstart, 4);    // 선택 표시 검은 테두리
		glLineWidth(1.0f);
	}
}

void draw3d()
{
	// 움직임
	spinx += 2.0f * spinxdir;
	spiny += 2.0f * spinydir;
	orbitangle += 1.0f * orbitdir;
	if (frontopen && openangle < 90.0f) openangle += 2.0f;
	if (!frontopen && openangle > 0.0f) openangle -= 2.0f;

	glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (depthon) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);

	// 공책 채우기
	n = 0;
	int axesstart = n;
	addaxes(1.5f);
	float fc[6][3] = { { 1, 1, 0 }, { 0, 1, 1 }, { 1, 0, 1 }, { 1, 0.5f, 0 }, { 0.6f, 0.6f, 0.6f }, { 0.5f, 0, 1 } };
	int cubestart = n;
	addcubefaces(cubev, cubef, fc);
	float pc[5][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 0 }, { 0.5f, 0.5f, 0.5f } };
	int pystart = n;
	addpyramidfaces(pyv, pyf, pc);
	int spherestart = n;
	addobjshade(spherev, spheref, 0.3f, 0.8f, 1.0f);
	int spherecount = n - spherestart;
	int orbitstart = n;
	addorbit(72, 0.7f, 0.7f, 0.7f);

	upload();
	setcolor(false);
	float aspect = (float)width / height;
	setproj(makeproj(perspectiveon, aspect, 1.5f));
	setview(lookcam(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f)));

	// 전체를 x y 30도 기울이기
	glm::mat4 world = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), AX);
	world = glm::rotate(world, glm::radians(-30.0f), AY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawwith(world, GL_LINES, axesstart, 6);                              // 축
	drawwith(glm::scale(world, glm::vec3(1.2f)), GL_LINE_LOOP, orbitstart, 72);   // 궤도

	glPolygonMode(GL_FRONT_AND_BACK, wireon ? GL_LINE : GL_FILL);

	// 왼쪽 육면체 크기 S 제자리 자전 앞면은 윗변 기준으로 열림
	float S = 0.6f;
	glm::mat4 cube = spinat(world, glm::vec3(-0.8f + movex, movey, 0.0f), spinx, spiny);
	for (int k = 1; k < 6; k++)
		drawobj(cube, glm::vec3(S), cubestart + k * 6, 6);
	drawobj(hinge(cube, glm::vec3(0.0f, S / 2, S / 2), AX, -openangle), glm::vec3(S), cubestart + F_FRONT * 6, 6);

	// 오른쪽 사각뿔 제자리 자전
	glm::mat4 py = spinat(world, glm::vec3(0.8f + movex, movey, 0.0f), spinx, spiny);
	drawobj(py, glm::vec3(S), pystart, 18);

	// 구 지름 0.4 원점 y축 공전 궤도 반지름 1.2
	glm::mat4 ball = orbity(world, orbitangle, 1.2f);
	drawobj(ball, glm::vec3(0.4f), spherestart, spherecount);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// 그리기 매 프레임
void drawScene()
{
	if (mode3d) draw3d();
	else draw2d();
}

// 키보드 콜백
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool shift = (mods & GLFW_MOD_SHIFT);

	// 누르고 있는 동안 반복
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT:  movex -= 0.05f; break;
		case GLFW_KEY_RIGHT: movex += 0.05f; break;
		case GLFW_KEY_UP:    movey += 0.05f; break;
		case GLFW_KEY_DOWN:  movey -= 0.05f; break;
		}
	}

	// 누를 때 한 번
	if (action != GLFW_PRESS) return;
	switch (key)
	{
	case GLFW_KEY_Q:
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_M:        // 2D 3D 바꾸기
		mode3d = !mode3d;
		break;
	case GLFW_KEY_X:        // x 양 X 음 자전 같은 키 다시 누르면 멈춤
	{
		int want = shift ? -1 : 1;
		spinxdir = (spinxdir == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_Y:
	{
		int want = shift ? -1 : 1;
		spinydir = (spinydir == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_R:        // r 양 R 음 공전
	{
		int want = shift ? -1 : 1;
		orbitdir = (orbitdir == want) ? 0 : want;
		break;
	}
	case GLFW_KEY_O: frontopen = !frontopen; break;
	case GLFW_KEY_P: perspectiveon = !shift; break;     // p 원근 P 직각
	case GLFW_KEY_W: wireon = !shift; break;            // w 와이어 W 솔리드
	case GLFW_KEY_H: depthon = !depthon; break;
	case GLFW_KEY_1: anim2d = (anim2d == 1) ? 0 : 1; break;
	case GLFW_KEY_2: anim2d = (anim2d == 2) ? 0 : 2; break;
	case GLFW_KEY_3: anim2d = (anim2d == 3) ? 0 : 3; break;
	case GLFW_KEY_C: scount = 0; selected = -1; break;
	case GLFW_KEY_S: resetall(); break;
	}
}

// 마우스 버튼 콜백
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	float gx, gy;
	mousetogl(mx, my, gx, gy);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		int hit = pickshape(gx, gy);
		if (hit >= 0) {
			selected = bringtofront(hit);   // 잡은 도형 맨 위로
			dragging = true;
			lastgx = gx; lastgy = gy;
		}
		else {
			selected = pushshape(3, gx, gy);   // 빈 곳이면 그 자리에 사각형
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// 놓았을 때 겹치면 합치기 실습3
		if (dragging && selected >= 0)
			for (int i = 0; i < scount; i++)
				if (i != selected && shapeoverlap(shapes[i], shapes[selected])) {
					int keep = (i < selected) ? i : selected - 1;
					mergeshape(i, selected);
					selected = keep;
					break;
				}
		dragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		int hit = pickshape(gx, gy);
		if (hit >= 0) removeshape(hit);
	}
}

// 마우스 이동 콜백 드래그 중일 때만 이동량만큼 옮김
void CursorPosCallback(GLFWwindow* window, double x, double y)
{
	if (!dragging || selected < 0) return;
	float gx, gy;
	mousetogl(x, y, gx, gy);
	shapes[selected].x += gx - lastgx;
	shapes[selected].y += gy - lastgy;
	lastgx = gx;
	lastgy = gy;
}

// 마우스 휠 앞으로 1 뒤로 -1 줌 등에 사용
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (selected >= 0) {
		shapes[selected].scale += 0.1f * (float)yoffset;
		shapes[selected].scale = glm::clamp(shapes[selected].scale, 0.3f, 3.0f);
	}
}


// =====================================================================
// [19] 자주 나는 실수 체크
// =====================================================================
// 화면이 비었다
//   upload 를 안 불렀다  n 을 매 프레임 0 으로 안 비웠다  glDrawArrays 시작 번호 개수 틀림
//   glUseProgram glBindVertexArray 빠짐  uniform 이름 오타로 행렬이 0 행렬
//   glm::mat4 m; 만 쓰면 0 행렬 반드시 glm::mat4(1.0f)
// 3D 가 뒤죽박죽 겹친다
//   glEnable(GL_DEPTH_TEST) 빠짐  glClear 에 GL_DEPTH_BUFFER_BIT 빠짐
// 원근 투영인데 아무것도 안 보인다
//   카메라가 물체 안에 있음 lookAt eye 를 z 5 정도로  near 가 0 이하
// 회전이 이상한 곳을 중심으로 돈다
//   translate rotate 순서 바뀜  제자리는 translate 다음 rotate  공전은 rotate 다음 translate
//   orbity 반지름이 0 이 아니면 그만큼 떨어진 곳을 축으로 돎  제자리 회전은 반지름 0 이나 rotate
// 앞에서 한 회전 공전이 사라졌다
//   m = glm::translate(glm::mat4(1.0f), ...) 로 다시 시작함  glm::translate(m, ...) 으로
// 같은 크기인데 부품끼리 크기가 달라 보인다
//   m 하나로 translate 를 이어서 해서 위치가 쌓임  원근이라 멀면 작아 보임  기준 변수에서 새로 시작
// 왔다갔다가 안 된다 끝에서 떨기만 한다
//   끝에서 각도를 바꿨음 a = 1  방향을 바꿔야 함 dir = 1  dir 시작값 0 인지도 확인
// 각도가 너무 빨리 돈다
//   glm::rotate 는 라디안 glm::radians(각도) 로 감싸기
// 대문자 키가 안 먹는다
//   key 는 대소문자 같은 값 mods & GLFW_MOD_SHIFT 로 구분
// 마우스 위치가 위아래 반대
//   gy = 1 - my / height * 2  y 뒤집기
// 키 한 번에 여러 번 실행
//   GLFW_REPEAT 까지 받는 블록에 켜고 끄는 키를 넣었음 PRESS 블록으로
// obj 가 안 읽힌다 콘솔에 열기 실패
//   파일 위치 cpp 와 같은 프로젝트 폴더  VS 에서 Ctrl F5  initobj 에 loadobj 넣었는지
// obj 크기가 생각과 다르다
//   obj 는 크기 0.6  drawobj(m, 실제 크기, ...) 로 그리면 그 크기  drawwith 로 그리면 0.6 그대로
// 재정의 에러
//   틀에 있는 함수를 창고에서 또 붙였음 맨 위 목록 확인
// 공책 넘침
//   MAXV 키우기 sphere.obj 는 하나에 꼭짓점 2880 개
// =====================================================================
