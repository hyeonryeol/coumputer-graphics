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
	for (int i = 0; i < MAXB; ++i)
		for (int j = 0; j < MAXB; ++j)
			occ[i][j] = false;

	for (int r = 0; r < s.h; ++r)
	{
		for (int c = 0; c < s.w; ++c)
		{
			int row = (s.ly + r) % bsize;
			int col = (s.lx + c) % bsize;
			occ[row][col] = true;
		}
	}
}

// 보드를 그린다
void render()
{
	buildOcc(s1, occ1);
	buildOcc(s2, occ2);

	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int row = 0; row < bsize; ++row)
	{
		for (int col = 0; col < bsize; ++col)
		{
			if (occ1[row][col] && occ2[row][col])
			{
				SetConsoleTextAttribute(hcon, 12);   // 겹치는 칸은 빨강
				cout << "# ";
				SetConsoleTextAttribute(hcon, 7);
			}
			else if (occ1[row][col]) cout << "0 ";
			else if (occ2[row][col]) cout << "x ";
			else cout << ". ";
		}
		cout << endl;
	}
}

// 좌표 두 개로 사각형을 만든다
Rect makeRect(int x1, int y1, int x2, int y2)
{
	int lx = (x1 < x2) ? x1 : x2;
	int ly = (y1 < y2) ? y1 : y2;
	int w = (x1 < x2 ? x2 - x1 : x1 - x2) + 1;
	int h = (y1 < y2 ? y2 - y1 : y1 - y2) + 1;

	Rect s = { lx, ly, w, h };
	return s;
}

// 도형을 한 칸 이동한다
void moveX(Rect& s, int d) { s.lx = (s.lx + d + bsize) % bsize; }
void moveY(Rect& s, int d) { s.ly = (s.ly + d + bsize) % bsize; }

// 도형 크기를 한 칸 조절한다
void growX(Rect& s)   { if (s.lx + s.w < bsize) s.w++; }
void shrinkX(Rect& s) { if (s.w > 1) s.w--; }
void growY(Rect& s)   { if (s.ly + s.h < bsize) s.h++; }
void shrinkY(Rect& s) { if (s.h > 1) s.h--; }

// 도형이 보드 안에 들어가는지 확인한다
bool fits(const Rect& s, int b) { return s.lx + s.w <= b && s.ly + s.h <= b; }

int main()
{
	int x1, y1, x2, y2, x3, y3, x4, y4;

	cout << "input value1: ";
	cin >> x1 >> y1 >> x2 >> y2;
	cout << "input value2: ";
	cin >> x3 >> y3 >> x4 >> y4;

	s1 = makeRect(x1, y1, x2, y2);
	s2 = makeRect(x3, y3, x4, y4);
	render();

	while (1)
	{
		string cmd;
		cout << "command: ";
		if (!(cin >> cmd)) break;

		if (cmd == "q") break;
		else if (cmd == "r") {
			cout << "input coord value1: ";
			cin >> x1 >> y1 >> x2 >> y2;
			cout << "input coord value2: ";
			cin >> x3 >> y3 >> x4 >> y4;
			bsize = 30;
			s1 = makeRect(x1, y1, x2, y2);
			s2 = makeRect(x3, y3, x4, y4);
		}
		else if (cmd == "c") {                 // 보드를 키운다
			if (bsize < 40) bsize++;
		}
		else if (cmd == "d") {                 // 보드를 줄인다
			if (bsize > 10 && fits(s1, bsize - 1) && fits(s2, bsize - 1)) bsize--;
		}
		// 도형1 명령
		else if (cmd == "x") moveX(s1, +1);
		else if (cmd == "X") moveX(s1, -1);
		else if (cmd == "y") moveY(s1, +1);
		else if (cmd == "Y") moveY(s1, -1);
		else if (cmd == "s") { shrinkX(s1); shrinkY(s1); }
		else if (cmd == "S") { growX(s1);   growY(s1);   }
		else if (cmd == "i") growX(s1);
		else if (cmd == "I") shrinkX(s1);
		else if (cmd == "j") growY(s1);
		else if (cmd == "J") shrinkY(s1);
		else if (cmd == "a") { growX(s1);   shrinkY(s1); }
		else if (cmd == "A") { shrinkX(s1); growY(s1);   }
		else if (cmd == "b") { cout << "면적: " << s1.w << "x" << s1.h << " = " << s1.w * s1.h << endl; 
		continue;
		}

		// 도형2 명령
		else if (cmd == "f") moveX(s2, +1);
		else if (cmd == "F") moveX(s2, -1);
		else if (cmd == "g") moveY(s2, +1);
		else if (cmd == "G") moveY(s2, -1);
		else if (cmd == "t") { shrinkX(s2); shrinkY(s2); }
		else if (cmd == "T") { growX(s2);   growY(s2);   }
		else if (cmd == "k") growX(s2);
		else if (cmd == "K") shrinkX(s2);
		else if (cmd == "l") growY(s2);
		else if (cmd == "L") shrinkY(s2);
		else if (cmd == "n") { growX(s2);   shrinkY(s2); }
		else if (cmd == "N") { shrinkX(s2); growY(s2);   }
		else if (cmd == "m") { cout << "면적: " << s2.w << "x" << s2.h << " = " << s2.w * s2.h << endl; 
		continue;
		}

		render();
	}
	return 0;
}
