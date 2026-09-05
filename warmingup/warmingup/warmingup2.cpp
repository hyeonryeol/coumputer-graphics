#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>     
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

using namespace std;

bool apress = false;
bool fpress = false;
bool dpress = false;
bool epress = false;
bool gpress = false;
bool hpress = false;
bool cpress = false;
bool ipress = false;

int jpress = 1;

string tolow(string s)
{
	for (int k = 0; k < (int)s.length(); ++k)
	{
		s[k] = tolower(s[k]);
	}
	return s;
}

int main()
{
	string filename;
	cout << "file name: ";
	cin >> filename;

	ifstream fin(filename);

	if (!fin)
	{
		cout << "파일오류" << endl;
		return 0;
	}

	string lines[20];
	string words[20][30];
	int wordcount[20] = {};
	int count = 0;

	while (count < 20 && getline(fin, lines[count]))
	{
		++count;
	}

	fin.close();

	for (int i = 0; i < count; ++i)
	{
		istringstream iss(lines[i]);
		string word;
		int n = 0;

		while (iss >> word)
		{
			words[i][n] = word;
			++n;
		}
		wordcount[i] = n;
	}
	for (int i = 0; i < count; ++i)
	{
		cout << lines[i] << endl;
	}

	while (1)
	{
		string command;
		cout << "command:";
		cin >> command;

		if (command == "a")
		{
			if (apress == false)
			{
				for (int i = 0; i < count; ++i)
				{
					string s = lines[i];
					for (int j = 0; j < s.length(); ++j)
					{
						if (islower(s[j])) s[j] = toupper(s[j]);
						else if (isupper(s[j])) s[j] = tolower(s[j]);
					}
					cout << s << endl;
				}
				apress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}
				apress = false;
			}
		}
		if (command == "d")
		{
			if (dpress == false)
			{
				for (int i = 0; i < count; ++i)
				{
					string d = lines[i];
					for (int j = d.length() - 1; j >= 0; --j)
					{
						cout << d[j];
					}
					cout << endl;
				}

				dpress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}

				dpress = false;
			}
		}
		if (command == "e")
		{
			if (epress == false)
			{
				for (int i = 0; i < count; ++i)
				{
					for (int j = 0; j < wordcount[i]; ++j)
					{
						if (j > 0) cout << "*";
						cout << words[i][j];
					}
					cout << endl;
				}
				epress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}
				epress = false;
			}
		}
		if (command == "b")
		{
			for (int i = 0; i < count; ++i)
			{
				cout << lines[i] << " " << wordcount[i] << endl;
			}
		}
		if (command == "c")
		{
			if (cpress == false)
			{
				HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
				int total = 0;

				for (int i = 0; i < count; ++i)
				{
					for (int j = 0; j < wordcount[i]; ++j)
					{
						if (j > 0) cout << " ";        

						if (isupper(words[i][j][0]))
						{
							SetConsoleTextAttribute(hcon, 12);
							cout << words[i][j];
							SetConsoleTextAttribute(hcon, 7);
							++total;                   
						}
						else
						{
							cout << words[i][j];
						}
					}
					cout << endl;
				}

				cout << "대문자로 시작하는 단어: " << total << endl;
				cpress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}
				cpress = false;
			}
		}
		if (command == "j")
		{
			for (int i = 0 + jpress; i < count; ++i)
			{
				cout << lines[i] << endl;
			}
			for (int i = 0; i < jpress; ++i)
			{
				cout << lines[i] << endl;

			}
			jpress = (jpress + 1) % count;
		}
		if (command == "f")
		{
			if (fpress == false)
			{
				if (epress == false)
				{

					for (int i = 0; i < count; ++i)
					{
						for (int j = wordcount[i] - 1; j >= 0; --j)
						{
							cout << words[i][j] << " ";

						}
						cout << endl;
					}
				}
				else
				{
					for (int i = 0; i < count; ++i)
					{
						for (int j = wordcount[i] - 1; j >= 0; --j)
						{
							if (j > 0) cout << "*";
							cout << words[i][j];
						}
						cout << endl;
					}
				}
				fpress = true;

			}
			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}
				fpress = false;
			}

		}
		if (command == "g")
		{
			if (gpress == false)
			{
				char oldc, newc;
				cout << "바꿀문자:";
				cin >> oldc;
				cout << "새문자:";
				cin >> newc;
				for (int i = 0; i < count; ++i)
				{
					string g = lines[i];
					for (int j = 0; j < (int)g.length(); ++j)
					{
						if (g[j] == oldc) g[j] = newc;
					}
					cout << g << endl;
				}
				gpress = true;
			}

			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}
				gpress = false;
			}
		}
		if (command == "h")
		{
			if (hpress == false)
			{
				for (int i = 0; i < count; ++i)
				{
					string g = lines[i];
					for (int j = 0; j < g.length(); ++j)
					{
						cout << g[j];
						if (g[j] >= '0' && g[j] <= '9')
						{
							cout << endl;
						}

					}
					cout << endl;
				}
				hpress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}
				hpress = false;
			}
		}
		if (command == "i")
		{
			if (ipress == false)
			{
				HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
				int total = 0;
				string w;
				cin >> w;
				for (int i = 0; i < count; ++i)
				{
					
					for (int j = 0; j < wordcount[i]; ++j)
					{
						if (j > 0) cout << " ";
						if (tolow(words[i][j]) == tolow(w))
						{
							SetConsoleTextAttribute(hcon, 12);
							cout << words[i][j];        
							SetConsoleTextAttribute(hcon, 7);
							++total;
							
						}
						else
						{
							cout << words[i][j];
						}
					}
						cout << endl;
				}
				cout << total <<"개" << endl;
				ipress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i)
				{
					cout << lines[i] << endl;
				}
				ipress =false;
			}
		}
		if (command == "q")
		{
			break;
		}
	}
}