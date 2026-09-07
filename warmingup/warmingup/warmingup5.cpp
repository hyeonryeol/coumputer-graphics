#include <string>
#include <iostream>
#include <windows.h>

using namespace std;

const int MAXB = 40;
int bsize = 30;

struct Rect { int lx, ly, w, h; };
Rect s1, s2;

bool occ1[MAXB][MAXB];
bool occ2[MAXB][MAXB];

// 도형의 칸을 occ 배열에 채운다
void buildOcc(const Rect& s, bool occ[MAXB][MAXB])
{

}

// 보드 출력 도형1 도형2 겹침 빈칸
void render()
{

}

// 좌표 두 개로 사각형 만들기
Rect makeRect(int x1, int y1, int x2, int y2)
{
	Rect s = { 0, 0, 1, 1 };
	return s;
}

// 이동과 확대 축소 도우미
void moveX(Rect& s, int d) {  }
void moveY(Rect& s, int d) {  }
void growX(Rect& s) {  }
void shrinkX(Rect& s) {  }
void growY(Rect& s) {  }
void shrinkY(Rect& s) {  }

int main()
{
	int x1, y1, x2, y2, x3, y3, x4, y4;

	cout << "input coord value1: ";
	cin >> x1 >> y1 >> x2 >> y2;
	cout << "input coord value2: ";
	cin >> x3 >> y3 >> x4 >> y4;

	s1 = makeRect(x1, y1, x2, y2);
	s2 = makeRect(x3, y3, x4, y4);
	render();

	while (1)
	{
		string cmd;
		cout << "input order: ";
		if (!(cin >> cmd)) break;

		if (cmd == "q") break;
		else if (cmd == "r") {  }
		else if (cmd == "c") {  }
		else if (cmd == "d") {  }
		// 도형1 명령
		else if (cmd == "x") {  }
		else if (cmd == "X") {  }
		else if (cmd == "y") {  }
		else if (cmd == "Y") {  }
		else if (cmd == "s") {  }
		else if (cmd == "S") {  }
		else if (cmd == "i") {  }
		else if (cmd == "I") {  }
		else if (cmd == "j") {  }
		else if (cmd == "J") {  }
		else if (cmd == "a") {  }
		else if (cmd == "A") {  }
		// 도형2 명령
		else if (cmd == "f") {  }
		else if (cmd == "F") {  }
		else if (cmd == "g") {  }
		else if (cmd == "G") {  }
		else if (cmd == "t") {  }
		else if (cmd == "T") {  }
		else if (cmd == "k") {  }
		else if (cmd == "K") {  }
		else if (cmd == "l") {  }
		else if (cmd == "L") {  }
		else if (cmd == "n") {  }
		else if (cmd == "N") {  }
		else if (cmd == "b") {  }
		else if (cmd == "m") {  }

		render();
	}
	return 0;
}
