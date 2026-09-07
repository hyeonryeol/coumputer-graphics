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

char board[30][30];
bool opened[30][30];
int x1, y1;
int x2, y2;
int x3, y3;
int x4, y4;

void printboard()
{

	for (int i = 0; i < 30; ++i)
	{
		for (int j = 0; j < 30; ++j)
		{
			if (opened[i][j])
			{
				cout << "O" << " ";
			}
			else
			{
				cout << "." << " ";

			}

		}
		cout << endl;
	}
}

int main()
{
	printboard();
	while (1)
	{

		cout << "첫번째 도형 좌표입력" << endl;
	
		cin >> x1 >> y1;
		cin >> x2 >> y2;
		if (x1 > x2)
		{
			if (y1 > y2)
			{
				for (int i = x2; i < x1; ++i)
				{
					for (int j = y2; j < y1; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
			else
			{
				for (int i = x2; i < x1; ++i)
				{
					for (int j = y1; j < y2; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
		}
		else
		{
			if (y1 > y2)
			{
				for (int i = x1; i < x2; ++i)
				{
					for (int j = y2; j < y1; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
			else
			{
				for (int i = x1; i < x2; ++i)
				{
					for (int j = y1; j < y2; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
		}
		cout << "두번째 도형 좌표입력" << endl;

		cin >> x3 >> y1;
		cin >> x2 >> y2;
		if (x3 > x2)
		{
			if (y1 > y2)
			{
				for (int i = x2; i < x3; ++i)
				{
					for (int j = y2; j < y1; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
			else
			{
				for (int i = x2; i < x3; ++i)
				{
					for (int j = y1; j < y2; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
		}
		else
		{
			if (y1 > y2)
			{
				for (int i = x3; i < x2; ++i)
				{
					for (int j = y2; j < y1; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
			else
			{
				for (int i = x3; i < x2; ++i)
				{
					for (int j = y1; j < y2; ++j)
					{
						opened[j][i] = true;

					}
				}
			}
		}
		printboard();

	}
}