#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <cmath>

using namespace std;


struct point {
	int x;
	int y;
	int z;
};

point plist[10];
bool  filled[10];
bool fpress = false;
point backupPlist[10];
bool  backupFilled[10];

double dist(point p)
{
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}
double dist2(point a, point b)
{
	int dx = a.x - b.x;
	int dy = a.y - b.y;
	int dz = a.z - b.z;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

int countFilled()
{
	int c = 0;
	for (int i = 0; i < 10; ++i) if (filled[i]) ++c;
	return c;
}
int topIndex()
{
	for (int i = 9; i >= 0; --i) if (filled[i]) return i;
	return -1;
}
int botIndex()
{
	for (int i = 0; i < 10; ++i) if (filled[i]) return i;
	return -1;
}

void printlist()
{
	for (int i = 9; i >= 0; --i)
	{
		cout << i;
		if (filled[i])
		{
			cout << " " << plist[i].x << " " << plist[i].y << " " << plist[i].z;
		}
		cout << endl;
	}
}
int main()
{
	printlist();
	while (1)
	{
		string command;
		cout << "command:";
		cin >> command;

		if (command == "+")
		{
			point p;
			cin >> p.x >> p.y >> p.z;

			if (countFilled() < 10)
			{
				int t = topIndex();
				int at = -1;
				for (int s = 1; s <= 10; ++s)
				{
					int idx = (t + s + 10) % 10;
					if (!filled[idx]) { at = idx; break; }
				}
				plist[at] = p;
				filled[at] = true;
			}
			else
			{
				cout << "리스트가 가득 찼습니다" << endl;
			}
			printlist();
		}
		if (command == "-")
		{
			int t = topIndex();
			if (t >= 0) filled[t] = false;
			printlist();
		}
		if (command == "d")
		{
			int b = botIndex();
			if (b >= 0) filled[b] = false;
			printlist();
		}
		if (command == "a")
		{
			cout << countFilled() << "개" << endl;
		}
		if (command == "e")
		{
			point p;
			cin >> p.x >> p.y >> p.z;

			if (countFilled() < 10)
			{
				if (!filled[0])
				{
					plist[0] = p;
					filled[0] = true;
				}
				else
				{
					int gap = -1;
					for (int i = 1; i < 10; ++i) if (!filled[i]) { gap = i; break; }
					for (int i = gap; i >= 1; --i)
					{
						plist[i] = plist[i - 1];
						filled[i] = filled[i - 1];
					}
					plist[0] = p;
					filled[0] = true;
				}
			}
			else
			{
				cout << "리스트가 가득 찼습니다" << endl;
			}
			printlist();
		}
		if (command == "b")
		{
			point np[10];
			bool nf[10];
			for (int i = 0; i < 10; ++i)
			{
				int to = (i + 9) % 10;
				np[to] = plist[i];
				nf[to] = filled[i];
			}
			for (int i = 0; i < 10; ++i) { plist[i] = np[i]; filled[i] = nf[i]; }
			printlist();
		}
		if (command == "c")
		{
			for (int i = 0; i < 10; ++i) filled[i] = false;
			printlist();
		}
		if (command == "f")
		{
			if (fpress == false)
			{
				for (int i = 0; i < 10; ++i) { backupPlist[i] = plist[i]; backupFilled[i] = filled[i]; }

				point arr[10];
				int m = 0;
				for (int i = 0; i < 10; ++i) if (filled[i]) arr[m++] = plist[i];

				for (int i = 0; i < m - 1; ++i)
				{
					for (int j = 0; j < m - 1 - i; ++j)
					{
						if (dist(arr[j]) > dist(arr[j + 1]))
						{
							point t = arr[j];
							arr[j] = arr[j + 1];
							arr[j + 1] = t;
						}
					}
				}

				for (int i = 0; i < 10; ++i) filled[i] = false;
				for (int i = 0; i < m; ++i) { plist[i] = arr[i]; filled[i] = true; }

				for (int i = 9; i >= 0; --i)
				{
					cout << i;
					if (filled[i])
					{
						cout << " " << plist[i].x << " " << plist[i].y << " " << plist[i].z;
						cout << "   " << dist(plist[i]);
					}
					cout << endl;
				}

				fpress = true;
			}
			else
			{
				for (int i = 0; i < 10; ++i) { plist[i] = backupPlist[i]; filled[i] = backupFilled[i]; }
				printlist();
				fpress = false;
			}
		}
		if (command == "g")
		{
			int idx[10];
			int m = 0;
			for (int i = 0; i < 10; ++i) if (filled[i]) idx[m++] = i;

			if (m < 2)
			{
				cout << "점이 2개 이상 필요합니다" << endl;
			}
			else
			{
				double maxd, mind;
				int maxa, maxb, mina, minb;
				bool first = true;

				for (int p1 = 0; p1 < m; ++p1)
				{
					for (int p2 = p1 + 1; p2 < m; ++p2)
					{
						int ia = idx[p1];
						int ib = idx[p2];
						double d = dist2(plist[ia], plist[ib]);

						cout << plist[ia].x << " " << plist[ia].y << " " << plist[ia].z;
						cout << "   ";
						cout << plist[ib].x << " " << plist[ib].y << " " << plist[ib].z;
						cout << " 거리: " << d << endl;

						if (first)
						{
							maxd = d;  maxa = ia;  maxb = ib;
							mind = d;  mina = ia;  minb = ib;
							first = false;
						}
						else
						{
							if (d > maxd) { maxd = d;  maxa = ia;  maxb = ib; }
							if (d < mind) { mind = d;  mina = ia;  minb = ib; }
						}
					}
				}
				cout << "가장 먼 두 점: ";
				cout << plist[maxa].x << " " << plist[maxa].y << " " << plist[maxa].z;
				cout << " 와 ";
				cout << plist[maxb].x << " " << plist[maxb].y << " " << plist[maxb].z;
				cout << ", 거리: " << maxd << endl;

				cout << "가장 가까운 두 점: ";
				cout << plist[mina].x << " " << plist[mina].y << " " << plist[mina].z;
				cout << " 와 ";
				cout << plist[minb].x << " " << plist[minb].y << " " << plist[minb].z;
				cout << ", 거리: " << mind << endl;
			}
		}
		if (command == "q")
		{
			break;
		}
	}
}
