#include <bits/stdc++.h>
#include <vector>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int n, m;
	cin >> n >> m;

	vector<int> An(n + 1, 0);
	for (int i = 1; i <= n; i++) {
		cin >> An[i];
	}

	vector<vector<int>> Sn(m, vector<int>(0));
	for (int i = 0; i < m; i++) {
		int size;
		cin >> size;
		Sn[i].resize(size);
		for (int j = 0; j < size; j++) {
			cin >> Sn[i][j];
		}
	}

	for (int i = 0; i < m; i++) {
		int size;
		cin >> size;
		vector<int> current_T(size);
		for (int j = 0; j < size; j++) {
			cin >> current_T[j];
		}

		// Answer1 
		bool Answer1 = (Sn[i] == current_T);

		// Answer2 
		int ans1 = 0, ans2 = 0;
		for (int x : Sn[i]) {
			ans1 ^= An[x];
		}
		for (int x : current_T) {
			ans2 ^= An[x];
		}

		bool Answer2 = (ans1 == ans2);

		if (Answer1 == Answer2) {
			cout << "correct\n";
		} else {
			cout << "wrong\n";
		}
	}
	return 0;
}











