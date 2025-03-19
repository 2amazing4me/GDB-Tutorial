// https://www.pbinfo.ro/probleme/3990/dinamica06

#include <iostream>

using namespace std;

#define MOD 123457

int exp(int n)
{
	if (n == 1)
		return 3;
	if (n % 2 == 0) {
		int sol = exp(n / 2);
		return sol * sol % MOD;
	} else
		return 3 * exp(n - 1) % MOD;
}

int main()
{
	int n;
	cin >> n;
	cout << (2 * exp(n - 1)) % MOD << endl;
	return 0;
}