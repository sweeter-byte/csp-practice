#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// f(x, k) = ((x^2 + k^2) mod 2^3) XOR k
int f(int x, int k) {
    long long x2 = (long long)x * x;
    long long k2 = (long long)k * k;
    return ((x2 + k2) % 8) ^ k;
}

// g(x, k) transformation
int g(int x, int k) {
    // x is a 9-bit integer.
    // Split into high(a), mid(b), low(c) 3 bits each.
    // x = a * 2^6 + b * 2^3 + c
    int c = x & 7;        // Low 3 bits
    int b = (x >> 3) & 7; // Mid 3 bits
    int a = (x >> 6) & 7; // High 3 bits

    // Transform
    // a' = b
    // b' = c XOR f(b, k)
    // c' = a XOR f(c, k)
    int a_prime = b;
    int b_prime = c ^ f(b, k);
    int c_prime = a ^ f(c, k);

    // Recombine
    return (a_prime << 6) | (b_prime << 3) | c_prime;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    cin >> n >> m;

    vector<int> k(m);
    for (int i = 0; i < m; ++i) {
        cin >> k[i];
    }

    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }

    vector<int> mapping(512);
    for (int i = 0; i < 512; ++i) {
        mapping[i] = i;
    }

    for (int j = 0; j < m; ++j) {
        int current_k = k[j];
        for (int i = 0; i < 512; ++i) {
            mapping[i] = g(mapping[i], current_k);
        }
    }

    vector<int> rev_map(512);
    for (int i = 0; i < 512; ++i) {
        rev_map[mapping[i]] = i;
    }

    for (int i = 0; i < n; ++i) {
        cout << rev_map[a[i]] << (i == n - 1 ? "" : " ");
    }
    cout << endl;

    return 0;
}
