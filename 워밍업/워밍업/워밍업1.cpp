#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
using namespace std;

int num1[10][10] = {};
int num2[10][10] = {};
int result[10][10] = {};
int result1[10][10] = {};

int main()
{
	
	srand((unsigned)time(NULL));
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
		num1[i][j] = rand() % 10;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			cout << num1[i][j] << " ";

		}
		cout << endl;
	}
	cout << endl;
	cout << endl;
	cout << endl;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			num2[i][j] = rand() % 10;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			cout << num2[i][j]<< " ";

		}
		cout << endl;
	}
	
	while (1)
	{
	string command;
	cout << "command" << endl;
	cin >> command;
		if (command == "a")
		{
		
			for (int i = 0; i < 4;++i)
			{
				for(int j = 0; j < 4;++j)
				{
					result[i][j] = num1[i][j] + num2[i][j];
				}
			}
			for (int i = 0; i < 4;++i)
			{
				for (int j = 0; j < 4;++j)
				{
					cout << result[i][j] << " ";

				}
				cout << endl;
			}
			
		}
		if (command == "d")
		{
			
			for (int i = 0; i < 4;++i)
			{
				for (int j = 0; j < 4;++j)
				{
					result[i][j] = num1[i][j] - num2[i][j];
				}
			}
			for (int i = 0; i < 4;++i)
			{
				for (int j = 0; j < 4;++j)
				{
					cout << result[i][j] << " ";

				}
				cout << endl;
			}
			
		}
		if (command == "m")
		{

			for (int i = 0; i < 4;++i)
			{
				for (int j = 0; j < 4;++j)
				{
					result1[i][j] = num1[i][0] * num2[0][j];
				}
			}
		
			result[0][0] = result1[0][0] + result1[1][1] + result1[2][2] + result1[3][3];
				
			for (int i = 0; i < 4;++i)
			{
				for (int j = 0; j < 4;++j)
				{
					cout << result[i][j] << " ";

				}
				cout << endl;
			}

		}
		if (command == "q")
		{
			break;
		}
		if (command == "s")
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					num1[i][j] = rand() % 10;
				}
			}
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					cout << num1[i][j] << " ";

				}
				cout << endl;
			}
			cout << endl;
			cout << endl;
			cout << endl;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					num2[i][j] = rand() % 10;
				}
			}
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					cout << num2[i][j] << " ";

				}
				cout << endl;
			}
		}
		if (command == "+")
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					if (num1[i][j] == 9)
					{
						cout << 0 << " ";
						

					}
					else
					{
						cout << num1[i][j] + 1 << " ";
						
					}
					
					
				}
				cout << endl;
			}
				cout << endl;
				cout << endl;
				cout << endl;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (num1[i][j] == 9)
				{
					cout << 0 << " ";
					
				}
				else
				{
					cout << num2[i][j] + 1 << " ";
				}
			}
			cout << endl;
		}
		}
	}
}