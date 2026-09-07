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

string giword = "";   
string tolow(string s)
{
	for (int k = 0; k < (int)s.length(); ++k)
	{
		s[k] = tolower(s[k]);
	}
	return s;
}


void rebuildWords(string lines[], int count, string words[][30], int wordcount[])
{
	for (int i = 0; i < count; ++i)
	{
		string t = lines[i];
		for (int j = 0; j < (int)t.length(); ++j)
		{
			if (t[j] == '*') t[j] = ' ';
		}
		istringstream iss(t);
		string word;
		int n = 0;
		while (iss >> word)
		{
			words[i][n] = word;
			++n;
		}
		wordcount[i] = n;
	}
}


void printLine(HANDLE hcon, const string& line, bool hc, const string& iword, bool hi)
{
	int n = (int)line.length();
	int j = 0;
	while (j < n)
	{
		if (isspace((unsigned char)line[j]) || line[j] == '*')
		{
			cout << line[j];
			++j;
			continue;
		}
		int k = j;
		while (k < n && !isspace((unsigned char)line[k]) && line[k] != '*') ++k;
		string w = line.substr(j, k - j);

		bool red = false;
		if (hc && !w.empty() && isupper((unsigned char)w[0])) red = true;
		if (hi && tolow(w) == tolow(iword)) red = true;

		if (red)
		{
			SetConsoleTextAttribute(hcon, 12);
			cout << w;
			SetConsoleTextAttribute(hcon, 7);
		}
		else
		{
			cout << w;
		}
		j = k;
	}
}

int main()
{
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);

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

	rebuildWords(lines, count, words, wordcount);


	string bakA[20], bakD[20], bakE[20], bakF[20], bakG[20], bakH[20];

	for (int i = 0; i < count; ++i)
	{
		printLine(hcon, lines[i], cpress, giword, ipress);
		cout << endl;
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
				for (int i = 0; i < count; ++i) bakA[i] = lines[i];
				for (int i = 0; i < count; ++i)
				{
					string s = lines[i];
					for (int j = 0; j < (int)s.length(); ++j)
					{
						if (islower(s[j])) s[j] = toupper(s[j]);
						else if (isupper(s[j])) s[j] = tolower(s[j]);
					}
					lines[i] = s;
				}
				rebuildWords(lines, count, words, wordcount);
				apress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i) lines[i] = bakA[i];
				rebuildWords(lines, count, words, wordcount);
				apress = false;
			}
			for (int i = 0; i < count; ++i) { printLine(hcon, lines[i], cpress, giword, ipress); cout << endl; }
		}
		if (command == "d")
		{
			if (dpress == false)
			{
				for (int i = 0; i < count; ++i) bakD[i] = lines[i];
				for (int i = 0; i < count; ++i)
				{
					string d = lines[i];
					string r = "";
					for (int j = (int)d.length() - 1; j >= 0; --j)
					{
						r += d[j];
					}
					lines[i] = r;
				}
				rebuildWords(lines, count, words, wordcount);
				dpress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i) lines[i] = bakD[i];
				rebuildWords(lines, count, words, wordcount);
				dpress = false;
			}
			for (int i = 0; i < count; ++i) { printLine(hcon, lines[i], cpress, giword, ipress); cout << endl; }
		}
		if (command == "e")
		{
			if (epress == false)
			{
				for (int i = 0; i < count; ++i) bakE[i] = lines[i];
				for (int i = 0; i < count; ++i)
				{
					string s = "";
					for (int j = 0; j < wordcount[i]; ++j)
					{
						if (j > 0) s += "*";
						s += words[i][j];
					}
					lines[i] = s;
				}
				rebuildWords(lines, count, words, wordcount);
				epress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i) lines[i] = bakE[i];
				rebuildWords(lines, count, words, wordcount);
				epress = false;
			}
			for (int i = 0; i < count; ++i) { printLine(hcon, lines[i], cpress, giword, ipress); cout << endl; }
		}
		if (command == "b")
		{
			for (int i = 0; i < count; ++i)
			{
				printLine(hcon, lines[i], cpress, giword, ipress);
				cout << " " << wordcount[i] << endl;
			}
		}
		if (command == "c")
		{
			if (cpress == false) cpress = true;
			else cpress = false;

			int total = 0;
			for (int i = 0; i < count; ++i)
			{
				for (int j = 0; j < wordcount[i]; ++j)
				{
					if (!words[i][j].empty() && isupper((unsigned char)words[i][j][0])) ++total;
				}
			}

			for (int i = 0; i < count; ++i)
			{
				printLine(hcon, lines[i], cpress, giword, ipress);
				cout << endl;
			}
			if (cpress) cout << "대문자로 시작하는 단어: " << total << endl;
		}
		if (command == "j")
		{
			for (int i = 0 + jpress; i < count; ++i)
			{
				printLine(hcon, lines[i], cpress, giword, ipress);
				cout << endl;
			}
			for (int i = 0; i < jpress; ++i)
			{
				printLine(hcon, lines[i], cpress, giword, ipress);
				cout << endl;
			}
			jpress = (jpress + 1) % count;
		}
		if (command == "f")
		{
			if (fpress == false)
			{
				
				char sepc = epress ? '*' : ' ';
				for (int i = 0; i < count; ++i) bakF[i] = lines[i];
				for (int i = 0; i < count; ++i)
				{
					string tok[30];
					int n = 0;
					string cur = "";
					string g = lines[i];
					for (int j = 0; j <= (int)g.length(); ++j)
					{
						if (j == (int)g.length() || g[j] == ' ' || g[j] == sepc)
						{
							if (!cur.empty()) { tok[n++] = cur; cur = ""; }
						}
						else
						{
							cur += g[j];
						}
					}
					string s = "";
					for (int j = n - 1; j >= 0; --j)
					{
						if (j < n - 1) s += sepc;
						s += tok[j];
					}
					lines[i] = s;
				}
				rebuildWords(lines, count, words, wordcount);
				fpress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i) lines[i] = bakF[i];
				rebuildWords(lines, count, words, wordcount);
				fpress = false;
			}
			for (int i = 0; i < count; ++i) { printLine(hcon, lines[i], cpress, giword, ipress); cout << endl; }
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
				for (int i = 0; i < count; ++i) bakG[i] = lines[i];
				for (int i = 0; i < count; ++i)
				{
					string g = lines[i];
					for (int j = 0; j < (int)g.length(); ++j)
					{
						if (g[j] == oldc) g[j] = newc;
					}
					lines[i] = g;
				}
				rebuildWords(lines, count, words, wordcount);
				gpress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i) lines[i] = bakG[i];
				rebuildWords(lines, count, words, wordcount);
				gpress = false;
			}
			for (int i = 0; i < count; ++i) { printLine(hcon, lines[i], cpress, giword, ipress); cout << endl; }
		}
		if (command == "h")
		{
			if (hpress == false)
			{
				for (int i = 0; i < count; ++i) bakH[i] = lines[i];
				for (int i = 0; i < count; ++i)
				{
					string g = lines[i];
					string s = "";
					for (int j = 0; j < (int)g.length(); ++j)
					{
						s += g[j];
						if (g[j] >= '0' && g[j] <= '9')
						{
							s += '\n';
						}
					}
					lines[i] = s;
				}
				rebuildWords(lines, count, words, wordcount);
				hpress = true;
			}
			else
			{
				for (int i = 0; i < count; ++i) lines[i] = bakH[i];
				rebuildWords(lines, count, words, wordcount);
				hpress = false;
			}
			for (int i = 0; i < count; ++i) { printLine(hcon, lines[i], cpress, giword, ipress); cout << endl; }
		}
		if (command == "i")
		{
			if (ipress == false)
			{
				cin >> giword;
				ipress = true;
			}
			else
			{
				ipress = false;
			}

			int total = 0;
			for (int i = 0; i < count; ++i)
			{
				for (int j = 0; j < wordcount[i]; ++j)
				{
					if (tolow(words[i][j]) == tolow(giword)) ++total;
				}
			}

			for (int i = 0; i < count; ++i)
			{
				printLine(hcon, lines[i], cpress, giword, ipress);
				cout << endl;
			}
			if (ipress) cout << total << "개" << endl;
		}
		if (command == "q")
		{
			break;
		}
	}
}
