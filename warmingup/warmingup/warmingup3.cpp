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
int bottom;
int pcount;
bool fpress = false;
point backup[10];
int backupbottom;
int backuppcount;

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

int slot(int k)
{
	return (bottom + k) % 10;
}

bool used(int i)
{
	int rel = (i - bottom + 10) % 10;
	return rel < pcount;
}
void printlist()
{
	for (int i = 9; i >= 0; --i)
	{
		cout << i;
		if (used(i))
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

			if (pcount < 10)
			{
				plist[slot(pcount)] = p;
				pcount++;
			}
			else
			{
				cout << "리스트가 가득 찼습니다" << endl;
			}
			printlist();
		}
		if (command == "-")
		{
			if (pcount > 0)
			{
				pcount--;
			}
			printlist();
		}
		if (command == "d")
		{
			if (pcount > 0)
			{
				bottom = (bottom + 1) % 10;
				pcount--;
			}
			printlist();
		}
		if (command == "a")
		{
			cout << pcount << "개" << endl;
		}
		if (command == "e")
		{
			point p;
			cin >> p.x >> p.y >> p.z;

			if (pcount < 10)
			{
				for (int k = pcount - 1; k >= 0; --k)
				{
					plist[slot(k + 1)] = plist[slot(k)];
				}
				plist[bottom] = p;
				pcount++;
			}
			else
			{
				cout << "리스트가 가득 찼습니다" << endl;
			}
			printlist();
		}
		if (command == "b")
		{
			if (pcount > 0)
			{
				point tmp[10];
				for (int k = 0; k < pcount; ++k) tmp[k] = plist[slot(k)];

				bottom = (bottom + 9) % 10;

				for (int k = 0; k < pcount; ++k) plist[slot(k)] = tmp[k];
			}
			printlist();
		}
		if (command == "c")
		{
			bottom = 0;
			pcount = 0;
			printlist();
		}
		if (command == "f")
		{
			if (fpress == false)
			{
				for (int i = 0; i < 10; ++i) backup[i] = plist[i];
				backupbottom = bottom;
				backuppcount = pcount;

				
				point tmp[10];
				for (int k = 0; k < pcount; ++k) tmp[k] = plist[slot(k)];
				for (int k = 0; k < pcount; ++k) plist[k] = tmp[k];
				bottom = 0;

				
				for (int i = 0; i < pcount - 1; ++i)
				{
					for (int j = 0; j < pcount - 1 - i; ++j)
					{
						if (dist(plist[j]) > dist(plist[j + 1]))
						{
							point t = plist[j];
							plist[j] = plist[j + 1];
							plist[j + 1] = t;
						}
					}
				}

				for (int i = 9; i >= 0; --i)
				{
					cout << i;
					if (i < pcount)
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
				for (int i = 0; i < 10; ++i) plist[i] = backup[i];
				bottom = backupbottom;
				pcount = backuppcount;

				printlist();
				fpress = false;
			}
		}
		if (command == "g")
		{
			if (pcount < 2)
			{
				cout << "점이 2개 이상 필요합니다" << endl;
			}
			else
			{
				double maxd, mind;
				int maxa, maxb, mina, minb;
				bool first = true;

				for (int p1 = 0; p1 < pcount; ++p1)
				{
					for (int p2 = p1 + 1; p2 < pcount; ++p2)
					{
						int ia = slot(p1);
						int ib = slot(p2);
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
