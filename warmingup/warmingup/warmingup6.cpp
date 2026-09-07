#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

using namespace std;

struct V { double x, y, z; };
struct T { double s, t; };

V verts[1000];
T texs[1000];
int nv = 0;
int nt = 0;

int fvi[1000][16];
int fti[1000][16];
int fcnt[1000];
bool fhastex[1000];
int nf = 0;

int main()
{
	string fname;
	cout << "input data file name: ";
	cin >> fname;

	ifstream fin(fname);
	if (!fin)
	{
		cout << "파일 없음" << endl;
		return 0;
	}

	string line;
	while (getline(fin, line))
	{
		// 주석 떼기 
		string body = "";
		for (int k = 0; k < (int)line.length(); k++)
		{
			if (line[k] == '#') break;
			body += line[k];
		}

		istringstream iss(body);
		string tag;
		if (!(iss >> tag)) continue;

		if (tag == "v")
		{
			V v;
			iss >> v.x >> v.y >> v.z;
			verts[nv] = v;
			nv++;
		}
		else if (tag == "vt")
		{
			T t;
			iss >> t.s >> t.t;
			texs[nt] = t;
			nt++;
		}
		else if (tag == "f")
		{
			int cnt = 0;
			bool hastex = true;
			bool ok = true;
			string tok;

			while (iss >> tok)
			{
				// 토큰을 슬래시로 두 조각 
				istringstream ts(tok);
				string a = "";
				string b = "";
				getline(ts, a, '/');
				getline(ts, b, '/');

				// 정점 번호가 숫자인지 확인
				bool numok = (a.length() > 0);
				for (int k = 0; k < (int)a.length(); k++)
					if (!isdigit(a[k])) numok = false;
				if (!numok)
				{
					cout << "면 값 오류: " << tok << endl;
					ok = false;
					break;
				}
				int vi = stoi(a);

				// 텍스처 번호가 있으면
				int ti = 0;
				if (b.length() > 0)
				{
					for (int k = 0; k < (int)b.length(); k++)
						if (!isdigit(b[k])) numok = false;
					if (!numok)
					{
						cout << "면 값 오류: " << tok << endl;
						ok = false;
						break;
					}
					ti = stoi(b);
				}
				if (ti == 0) hastex = false;

				if (cnt < 16)
				{
					fvi[nf][cnt] = vi;
					fti[nf][cnt] = ti;
					cnt++;
				}
			}

			if (ok)
			{
				fcnt[nf] = cnt;
				fhastex[nf] = hastex;
				nf++;
			}
		}
	}
	fin.close();

	// 정점 값이 겹치는지 확인
	bool dup = false;
	for (int i = 0; i < nv; i++)
		for (int j = i + 1; j < nv; j++)
			if (verts[i].x == verts[j].x && verts[i].y == verts[j].y && verts[i].z == verts[j].z)
			{
				cout << "정점 " << (i + 1) << " 과 " << (j + 1) << " 값 중복" << endl;
				dup = true;
			}

	// 면마다 출력
	for (int i = 0; i < nf; i++)
	{
		// 꼭짓점이 세 개인지 확인
		if (fcnt[i] != 3)
		{
			cout << "Face " << (i + 1) << ": 꼭짓점 " << fcnt[i] << "개, 삼각형 아님" << endl;
			continue;
		}

		// 정점 번호가 범위 안인지 확인
		bool range = true;
		for (int k = 0; k < 3; k++)
			if (fvi[i][k] < 1 || fvi[i][k] > nv) range = false;
		if (!range)
		{
			cout << "Face " << (i + 1) << ": 정점 번호 범위 벗어남" << endl;
			continue;
		}

		// 세 번호가 서로 다른지 확인
		if (fvi[i][0] == fvi[i][1] || fvi[i][1] == fvi[i][2] || fvi[i][0] == fvi[i][2])
		{
			cout << "Face " << (i + 1) << ": 정점 번호 중복, 삼각형 아님" << endl;
			continue;
		}

		// 정상일 때 출력
		cout << "Face " << (i + 1) << " (" << fvi[i][0] << ", " << fvi[i][1] << ", " << fvi[i][2] << "): vertex ";
		for (int k = 0; k < 3; k++)
		{
			int idx = fvi[i][k] - 1;
			cout << "(" << verts[idx].x << ", " << verts[idx].y << ", " << verts[idx].z << ") ";
		}
		cout << endl;

		if (fhastex[i])
		{
			cout << "        texture ";
			for (int k = 0; k < 3; k++)
			{
				int idx = fti[i][k] - 1;
				cout << "(" << texs[idx].s << ", " << texs[idx].t << ") ";
			}
			cout << endl;
		}
	}

	if (!dup) cout << "No duplicate vertex value" << endl;

	return 0;
}
