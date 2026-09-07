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

char card[6][6];
bool  opened[6][6];
int x, y;
int opencount = 0;
int wincount = 0;


void printboard()
{
	system("cls");
	for (int i = 0; i < x; ++i)
	{
		cout << " " << (char)('a' + i) << " ";
	}
	cout << endl;
	for (int i = 0; i < y; ++i)
	{
		cout << i + 1;

		for (int j = 0; j < x; ++j)
		{
			if (opened[i][j])
			{
				cout << " " << (char)toupper(card[i][j]) << " ";
			}
			else
			{
				cout << " * ";
			}
		}
		cout << endl;
	}
}

void setup()
{
	char bag[36];
	int total = x * y;
	int pairs = total / 2;
	int n = 0;
	for (int i = 0; i < pairs; ++i)
	{
		bag[n++] = 'a' + i;
		bag[n++] = 'a' + i;
	}
	if (total % 2 == 1)
	{
		bag[n++] = '?';
	}
	for (int i = n - 1; i > 0; --i)
	{
		int r = rand() % (i + 1);
		char t = bag[i];
		bag[i] = bag[r];
		bag[r] = t;
	}
	int k = 0;
	for (int i = 0; i < y; ++i)
	{
		for (int j = 0; j < x; ++j)
		{
			card[i][j] = bag[k];
			k++;
		}
	}
	for (int i = 0; i < 6; ++i)      
		for (int j = 0; j < 6; ++j)
			opened[i][j] = false;
}

int main()
{
	srand((unsigned)time(NULL));


	int a = 1;
	char b = 'a';
	cout << "가로세로크기입력:";
	cin >> x >> y;
	
	setup();

	while (1)
	{
		
		printboard();
		cout << "좌표입력";
		string s;
		cin >> s;



		if (s == "q") break;
		if (s == "r") 
		{ 
			setup(); 
			continue;
			
		}
		if (s == "h")
		{
			for (int i = 0; i < y; ++i)
			{
				for (int j = 0; j < x; ++j)
				{
					opened[i][j] = true;
				}
			}
			printboard();
			cout << "엔터를 누르세요";
			cin.ignore();
			cin.get();
			for (int i = 0; i < y; ++i)
			{
				for (int j = 0; j < x; ++j)
				{
					opened[i][j] = false;
				}
			}

			continue;
		}


		int j = s[0] - 'a';
		int i = s[1] - '1';
		opened[i][j] = true;
		string s2;
		cin >> s2;
		int j2 = s2[0] - 'a';
		int i2 = s2[1] - '1';
		opened[i2][j2] = true;


		printboard();

		if (card[i][j] == card[i2][j2] || card[i][j] == '?' || card[i2][j2] == '?')
		{
			wincount++;
		}
		else
		{
			cout << "불일치. 엔터를 누르세요";
			cin.ignore();
			cin.get();

			opened[i][j] = false;
			opened[i2][j2] = false;
		}
		opencount++;
		bool win = true;
		for (int i = 0; i < y; ++i)
		{
			for (int j = 0; j < x; ++j)
			{
				if (opened[i][j] == false)
				{
					win = false;
				}
			}
		}
		if (opencount == 20 || win == true)
		{
			cout << "게임끝" << endl;
			cout << "점수: " << wincount;
			break;
		}

	}


}