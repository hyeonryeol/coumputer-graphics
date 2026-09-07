#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
using namespace std;

int num1[10][10] = {};
int num2[10][10] = {};
int result[10][10] = {};
int result1[10][10] = {};
int pluscount = 0;

bool epress = false;
bool fpress = false;

int det1 = 0;
int det2 = 0;
int det3 = 0;
int det4 = 0;
int det5 = 0;
int det6 = 0;
int det7 = 0;
int det8 = 0;
int det9 = 0;
int det10 = 0;
int det11 = 0;
int det12 = 0;
int det13 = 0;
int det14 = 0;
int det15 = 0;
int det16 = 0;
int det17 = 0;
int det18 = 0;
int det19 = 0;
int det20 = 0;
int det21 = 0;

int det22 = 0;
int det23 = 0;
int det24 = 0;
int det25 = 0;
int det26 = 0;
int det27 = 0;
int det28 = 0;
int det29 = 0;
int det30 = 0;
int det31 = 0;
int det32 = 0;
int det33 = 0;
int det34 = 0;
int det35 = 0;
int det36 = 0;
int det37 = 0;
int det38 = 0;
int det39 = 0;
int det40 = 0;
int det41 = 0;
int det42 = 0;



int main()
{

	srand((unsigned)time(NULL));
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			num1[i+pluscount][j+pluscount] = rand() % 9 + 1;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			cout << num1[i+pluscount][j+pluscount] << " ";

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
			num2[i][j] = rand() % 9 + 1;
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

	while (1)
	{
		string command;
		cout << "command" << endl;
		cin >> command;
		if (command == "a")
		{

			for (int i = 0; i < 4;++i)
			{
				for (int j = 0; j < 4;++j)
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

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result[i][j] = 0;
					for (int k = 0; k < 4; ++k)
					{
						result[i][j] += num1[i][k] * num2[k][j];
					}
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
					num1[i][j] = rand() % 9 + 1;
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
					num2[i][j] = rand() % 9 + 1;
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
						num1[i][j] = 0;
					}
					else
					{
						num1[i][j] += 1;
					}
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
					if (num2[i][j] == 9)
					{
						num2[i][j] = 0;
					}
					else
					{
						num2[i][j] += 1;
					}
					cout << num2[i][j] << " ";
				}
				cout << endl;
			}

		}
		if (command == "-")
		{


				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						if (num1[i][j] == 0)
						{
							num1[i][j] = 9;
						}
						else
						{
							num1[i][j] -= 1;
						}
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
						if (num2[i][j] == 0)
						{
							num2[i][j] = 9;
						}
						else
						{
							num2[i][j] -= 1;
						}
						cout << num2[i][j] << " ";
					}
					cout << endl;
				}


		}
		if (command == "r")
		{
			det1 = num1[1][1] * (num1[2][2] * num1[3][3] - num1[3][2] * num1[2][3]);
			det2 = num1[2][1] * (num1[1][2] * num1[3][3] - num1[3][2] * num1[1][3]);
			det3 = num1[3][1] * (num1[1][2] * num1[2][3] - num1[2][2] * num1[1][3]);
			det4 = det1 - det2 + det3;
			det5 = det4 * num1[0][0];
			det6 = num1[0][1] * (num1[2][2] * num1[3][3] - num1[3][2] * num1[2][3]);
			det7 = num1[2][1] * (num1[0][2] * num1[3][3] - num1[3][2] * num1[0][3]);
			det8 = num1[3][1] * (num1[0][2] * num1[2][3] - num1[2][2] * num1[0][3]);
			det9 = det6 - det7 + det8;
			det10 = det9 * num1[1][0];
			det11 = num1[0][1] * (num1[1][2] * num1[3][3] - num1[3][2] * num1[1][3]);
			det12 = num1[1][1] * (num1[0][2] * num1[3][3] - num1[3][2] * num1[0][3]);
			det13 = num1[3][1] * (num1[0][2] * num1[1][3] - num1[1][2] * num1[0][3]);
			det14 = det11 - det12 + det13;
			det15 = det14 * num1[2][0];
			det16 = num1[0][1] * (num1[1][2] * num1[2][3] - num1[2][2] * num1[1][3]);
			det17 = num1[1][1] * (num1[0][2] * num1[2][3] - num1[2][2] * num1[0][3]);
			det18 = num1[2][1] * (num1[0][2] * num1[1][3] - num1[1][2] * num1[0][3]);
			det19 = det16 - det17 + det18;
			det20 = det19 * num1[3][0];
			det21 = det5 - det10 + det15 - det20;
			cout << det21 << endl;

			det22 = num2[1][1] * (num2[2][2] * num2[3][3] - num2[3][2] * num2[2][3]);
			det23 = num2[2][1] * (num2[1][2] * num2[3][3] - num2[3][2] * num2[1][3]);
			det24 = num2[3][1] * (num2[1][2] * num2[2][3] - num2[2][2] * num2[1][3]);
			det25 = det22 - det23 + det24;
			det26 = det25 * num2[0][0];
			det27 = num2[0][1] * (num2[2][2] * num2[3][3] - num2[3][2] * num2[2][3]);
			det28 = num2[2][1] * (num2[0][2] * num2[3][3] - num2[3][2] * num2[0][3]);
			det29 = num2[3][1] * (num2[0][2] * num2[2][3] - num2[2][2] * num2[0][3]);
			det30 = det27 - det28 + det29;
			det31 = det30 * num2[1][0];
			det32 = num2[0][1] * (num2[1][2] * num2[3][3] - num2[3][2] * num2[1][3]);
			det33 = num2[1][1] * (num2[0][2] * num2[3][3] - num2[3][2] * num2[0][3]);
			det34 = num2[3][1] * (num2[0][2] * num2[1][3] - num2[1][2] * num2[0][3]);
			det35 = det32 - det33 + det34;
			det36 = det35 * num2[2][0];
			det37 = num2[0][1] * (num2[1][2] * num2[2][3] - num2[2][2] * num2[1][3]);
			det38 = num2[1][1] * (num2[0][2] * num2[2][3] - num2[2][2] * num2[0][3]);
			det39 = num2[2][1] * (num2[0][2] * num2[1][3] - num2[1][2] * num2[0][3]);
			det40 = det37 - det38 + det39;
			det41 = det40 * num2[3][0];
			det42 = det26 - det31 + det36 - det41;
			cout << det42 << endl;
		}
		if (command == "t")
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					cout << num1[j][i] << " ";

				}
				cout << endl;

			}
			det1 = num1[1][1] * (num1[2][2] * num1[3][3] - num1[3][2] * num1[2][3]);
			det2 = num1[2][1] * (num1[1][2] * num1[3][3] - num1[3][2] * num1[1][3]);
			det3 = num1[3][1] * (num1[1][2] * num1[2][3] - num1[2][2] * num1[1][3]);
			det4 = det1 - det2 + det3;
			det5 = det4 * num1[0][0];
			det6 = num1[0][1] * (num1[2][2] * num1[3][3] - num1[3][2] * num1[2][3]);
			det7 = num1[2][1] * (num1[0][2] * num1[3][3] - num1[3][2] * num1[0][3]);
			det8 = num1[3][1] * (num1[0][2] * num1[2][3] - num1[2][2] * num1[0][3]);
			det9 = det6 - det7 + det8;
			det10 = det9 * num1[1][0];
			det11 = num1[0][1] * (num1[1][2] * num1[3][3] - num1[3][2] * num1[1][3]);
			det12 = num1[1][1] * (num1[0][2] * num1[3][3] - num1[3][2] * num1[0][3]);
			det13 = num1[3][1] * (num1[0][2] * num1[1][3] - num1[1][2] * num1[0][3]);
			det14 = det11 - det12 + det13;
			det15 = det14 * num1[2][0];
			det16 = num1[0][1] * (num1[1][2] * num1[2][3] - num1[2][2] * num1[1][3]);
			det17 = num1[1][1] * (num1[0][2] * num1[2][3] - num1[2][2] * num1[0][3]);
			det18 = num1[2][1] * (num1[0][2] * num1[1][3] - num1[1][2] * num1[0][3]);
			det19 = det16 - det17 + det18;
			det20 = det19 * num1[3][0];
			det21 = det5 - det10 + det15 - det20;
			cout << det21 << endl;
			cout << endl;
			cout << endl;
			cout << endl;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					cout << num2[j][i] << " ";

				}
				cout << endl;
			}
			det22 = num2[1][1] * (num2[2][2] * num2[3][3] - num2[3][2] * num2[2][3]);
			det23 = num2[2][1] * (num2[1][2] * num2[3][3] - num2[3][2] * num2[1][3]);
			det24 = num2[3][1] * (num2[1][2] * num2[2][3] - num2[2][2] * num2[1][3]);
			det25 = det22 - det23 + det24;
			det26 = det25 * num2[0][0];
			det27 = num2[0][1] * (num2[2][2] * num2[3][3] - num2[3][2] * num2[2][3]);
			det28 = num2[2][1] * (num2[0][2] * num2[3][3] - num2[3][2] * num2[0][3]);
			det29 = num2[3][1] * (num2[0][2] * num2[2][3] - num2[2][2] * num2[0][3]);
			det30 = det27 - det28 + det29;
			det31 = det30 * num2[1][0];
			det32 = num2[0][1] * (num2[1][2] * num2[3][3] - num2[3][2] * num2[1][3]);
			det33 = num2[1][1] * (num2[0][2] * num2[3][3] - num2[3][2] * num2[0][3]);
			det34 = num2[3][1] * (num2[0][2] * num2[1][3] - num2[1][2] * num2[0][3]);
			det35 = det32 - det33 + det34;
			det36 = det35 * num2[2][0];
			det37 = num2[0][1] * (num2[1][2] * num2[2][3] - num2[2][2] * num2[1][3]);
			det38 = num2[1][1] * (num2[0][2] * num2[2][3] - num2[2][2] * num2[0][3]);
			det39 = num2[2][1] * (num2[0][2] * num2[1][3] - num2[1][2] * num2[0][3]);
			det40 = det37 - det38 + det39;
			det41 = det40 * num2[3][0];
			det42 = det26 - det31 + det36 - det41;
			cout << det42 << endl;
		}
		if (command == "e")
		{
			if (epress == false)
			{

				for (int i = 0; i < 4; ++i)
				{
					int minv = num1[i][0];
					for (int j = 0; j < 4; ++j)
					{
						if (minv > num1[i][j])
						{
							minv = num1[i][j];

						}
					}
					for (int j = 0; j < 4; ++j)
					{
						cout << num1[i][j] - minv << " ";
					}

					cout << endl;
				}
				cout << endl;
				cout << endl;
				cout << endl;

				for (int i = 0; i < 4; ++i)
				{
					int minv = num2[i][0];
					for (int j = 0; j < 4; ++j)
					{
						if (minv > num2[i][j])
						{
							minv = num2[i][j];

						}
					}
					for (int j = 0; j < 4; ++j)
					{
						cout << num2[i][j] - minv << " ";
					}

					cout << endl;
				}
			}
			else
			{
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
						cout << num2[i][j] << " ";

					}
					cout << endl;
				}
			}
			if (epress == false)
			{
				epress = true;
			}
			else
			{
				epress = false;
			}
		}
		if (command == "f")
		{
			if (fpress == false)
			{


				int maxv[4];

				for (int j = 0; j < 4; ++j)
				{
					maxv[j] = num1[0][j];

					for (int i = 0; i < 4; ++i)
					{
						if (maxv[j] < num1[i][j]) maxv[j] = num1[i][j];
					}
				}


				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						cout << num1[i][j] + maxv[j] << " ";
					}
					cout << endl;
				}
				cout << endl;
				cout << endl;
				cout << endl;
				int maxv2[4];
				for (int j = 0; j < 4; ++j)
				{
					maxv2[j] = num2[0][j];
					for (int i = 0; i < 4; ++i)
					{
						if (maxv2[j] < num2[i][j])
						{
							maxv2[j] = num2[i][j];

						}
					}
				}
				for (int i = 0; i < 4; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						cout << num2[i][j] + maxv2[j] << " ";
					}
					cout << endl;
				}
			}
			else
			{
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
						cout << num2[i][j] << " ";

					}
					cout << endl;
				}
			}
			if (fpress == false)
			{
				fpress = true;
			}
			else
			{
				fpress = false;
			}
		}

	}
}
