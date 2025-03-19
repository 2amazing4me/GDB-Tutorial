// https://www.pbinfo.ro/probleme/1148/existaimparedivimp

#include <iostream>

using namespace std;

int existapare(int a[], int st, int dr)
{
	if (st == dr) {
		if (a[st] % 2 != 0)
			return 1;
		else
			return 0;
	} else {
		int mij = (st + dr) / 2;
		int x = existapare(a, st, mij);
		int y = existapare(a, mij + 1, dr);
		return x || y;
	}
}

int main()
{
	int n;
	cin >> n;
	int a[n];
	for (int i = 0; i < n; ++i)
		cin >> a[i];
	if (existapare(a, 0, n - 1) == 1)
		cout << 'DA';
	else
		cout << 'NU';
	return 0;
}