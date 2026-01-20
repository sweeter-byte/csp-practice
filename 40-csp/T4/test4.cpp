#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int MAXN = 10000005;
const int MOD = 998244353;

// Arrays for linear sieve
int min_prime[MAXN];
int low[MAXN]; // Stores the power of the smallest prime factor (p^k)
int primes[MAXN];
int prime_cnt = 0;

// Function values
// f: For Op=0
// s1, s3, s4, s5: Components for Op=1
// We use dynamic vectors to allow conditional allocation based on Op/N if needed, 
// though for CP global arrays are standard. Here we use heap vectors for safety given stack limits.
// Actually, global static is better for BSS, but 5 * 80MB = 400MB is large.
// Let's use globals for simplicity, assuming 1024MB limit allows it.
long long f[MAXN];
long long s1[MAXN];
long long s3[MAXN];
long long s4[MAXN];
long long s5[MAXN];

// Power function: (base^exp) % MOD
long long power(long long base, long long exp) {
    long long res = 1;
    base %= MOD;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % MOD;
        base = (base * base) % MOD;
        exp /= 2;
    }
    return res;
}

// Geometric sum: 1 + p + ... + p^k
// = (p^{k+1} - 1) / (p - 1)
long long sum_geom(long long p, int k) {
    if (k < 0) return 0;
    if (k == 0) return 1;
    if (p == 1) return (k + 1) % MOD;
    long long num = (power(p, k + 1) - 1 + MOD) % MOD;
    long long den = power(p - 1, MOD - 2);
    return (num * den) % MOD;
}

// Calculate components for p^k
void calc_pk(int p, int k, int pk_val) {
    long long p_long = p;
    
    // --- Calculate f(p^k) ---
    // f(p^k) = sum_{s=k to 4k} N(s) p^{s-k}
    long long val_f = 0;
    long long p_pow = 1; // p^{s-k}
    // N(s) logic:
    // s ranges from k to 4k.
    // N(s) starts at 1 (for s=k), increases to 2k+1 (for s=2k), stays 2k+1 (s=3k), decrease?
    // Let's stick to the reliable loop for k <= 23.
    for (int s = k; s <= 4 * k; ++s) {
        // Pairs (a, g) such that a, g <= 2k and a+g=s
        int min_a = max(0, s - 2 * k);
        int max_a = min(s, 2 * k);
        int count = max(0, max_a - min_a + 1);
        if (count > 0) {
            val_f = (val_f + count * p_pow) % MOD;
        }
        p_pow = (p_pow * p_long) % MOD;
    }
    f[pk_val] = val_f;

    // We only need S components if we are doing Op=1 later, but we calculate them always for simplicity.
    // --- Calculate S1(p^k) ---
    // sum_{g=0 to 2k} p^g
    s1[pk_val] = sum_geom(p_long, 2 * k);

    // --- Calculate S3(p^k) ---
    // sum_{a=ceil(k/2) to 2k} p^{2a-k}
    long long val_s3 = 0;
    int start_a = (k + 1) / 2;
    // Series: p^{2*start_a - k} + p^{2*(start_a+1) - k} + ...
    // This is geometric series with ratio p^2.
    // First term: p^{2*start_a - k}
    // Num terms: (2*k) - start_a + 1
    int num_terms_s3 = 2 * k - start_a + 1;
    if (num_terms_s3 > 0) {
        long long first = power(p_long, 2 * start_a - k);
        if (num_terms_s3 == 1) val_s3 = first;
        else {
            long long ratio = (p_long * p_long) % MOD;
            // Sum = first * (ratio^count - 1) / (ratio - 1)
            // But if p=1, ratio=1 -> val = count * first
            if (ratio == 1) {
                val_s3 = (first * num_terms_s3) % MOD;
            } else {
                long long num = (power(ratio, num_terms_s3) - 1 + MOD) % MOD;
                long long den = power(ratio - 1, MOD - 2);
                val_s3 = (first * num) % MOD * den % MOD;
            }
        }
    }
    s3[pk_val] = val_s3;

    // --- Calculate S4(p^k) ---
    // (2k + 1) * p^k
    s4[pk_val] = ((2 * k + 1) * power(p_long, k)) % MOD;

    // --- Calculate S5(p^k) ---
    // sum_{a=ceil(k/2) to floor(3k/2)} p^{2k-a}
    // Reverse order a' = 2k-a? No, simple sum.
    // indices of power: 2k - a.
    // a goes from start_a to end_a = floor(3k/2)
    // max power: 2k - start_a
    // min power: 2k - end_a
    // consecutive powers? Yes, a increments by 1.
    long long val_s5 = 0;
    int end_a = (3 * k) / 2;
    int num_terms_s5 = end_a - start_a + 1;
    if (num_terms_s5 > 0) {
        // Powers are: p^{2k-start_a}, p^{2k-(start_a+1)} ...
        // This is geometric series with ratio 1/p?
        // Let's view from smallest power: a = end_a => power = 2k - end_a.
        long long min_pow = 2 * k - end_a;
        // Sum p^{min_pow} + ... + p^{min_pow + count - 1}
        if (p_long == 1) {
            val_s5 = num_terms_s5;
        } else {
            long long first = power(p_long, min_pow);
            long long num = (power(p_long, num_terms_s5) - 1 + MOD) % MOD;
            long long den = power(p_long - 1, MOD - 2);
            val_s5 = (first * num) % MOD * den % MOD;
        }
    }
    s5[pk_val] = val_s5;
}

int main() {
    // Optimize I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int op, n;
    if (!(cin >> op >> n)) return 0;

    // Init f[1], g components[1]
    f[1] = 1; 
    s1[1] = 1; s3[1] = 1; s4[1] = 1; s5[1] = 1;
    // low[1] is undefined/unused, loop starts from 2

    long long total_ans = 1; // Start with B=1 value
    // B=1 value: f(1)=1. g(1)=1.
    // If Op=1, ans starts at 1 - 1 = 0?
    // Wait. For B=1: A=1, G=1. Tuple (1,1,1,1,1,1,1). Distinct=1 != 6.
    // Bad sum g(1) = 2(1) + 1 + 1 + 2(1) - 5(1) = 1.
    // So Value = 1 - 1 = 0. Correct.
    if (op == 1) total_ans = 0;

    for (int i = 2; i <= n; ++i) {
        if (min_prime[i] == 0) {
            min_prime[i] = i;
            low[i] = i;
            primes[prime_cnt++] = i;
            calc_pk(i, 1, i); // i is prime, k=1
        }
        for (int j = 0; j < prime_cnt; ++j) {
            int p = primes[j];
            if (p * i > n) break; // Optimization: check overflow or bound
            // Actually i*p could overflow int if i close to 2e9, but N<=1e7 so safe.
            int next_val = i * p;
            min_prime[next_val] = p;

            if (i % p == 0) {
                low[next_val] = low[i] * p;
                if (low[next_val] == next_val) {
                    // next_val is a prime power p^k
                    // recover k from power ? Or store k?
                    // efficiently: next_val = p^k. 
                    // k is roughly log_p(next_val).
                    // Or just iterate divide.
                    int temp = next_val;
                    int k = 0;
                    while (temp > 1) { temp /= p; k++; }
                    calc_pk(p, k, next_val);
                } else {
                    // next_val = upper * (p^k)
                    int p_pow = low[next_val];
                    int upper = next_val / p_pow;
                    // Multiplicative property
                    f[next_val] = (f[upper] * f[p_pow]) % MOD;
                    s1[next_val] = (s1[upper] * s1[p_pow]) % MOD;
                    s3[next_val] = (s3[upper] * s3[p_pow]) % MOD;
                    s4[next_val] = (s4[upper] * s4[p_pow]) % MOD;
                    s5[next_val] = (s5[upper] * s5[p_pow]) % MOD;
                }
                break; // Linear sieve break
            } else {
                low[next_val] = p;
                // next_val = i * p (coprime)
                f[next_val] = (f[i] * f[p]) % MOD;
                s1[next_val] = (s1[i] * s1[p]) % MOD;
                s3[next_val] = (s3[i] * s3[p]) % MOD;
                s4[next_val] = (s4[i] * s4[p]) % MOD;
                s5[next_val] = (s5[i] * s5[p]) % MOD;
            }
        }

        // Add to result
        long long current_val = f[i];
        if (op == 1) {
            // Subtract bad
            // g(i) = 2*s1 + s3 + s4 + 2*s5 - 5*i
            long long bad = (2 * s1[i]) % MOD;
            bad = (bad + s3[i]) % MOD;
            bad = (bad + s4[i]) % MOD;
            bad = (bad + 2 * s5[i]) % MOD;
            long long s12 = i % MOD; // S12(i) = i
            bad = (bad - 5 * s12) % MOD;
            if (bad < 0) bad += MOD;
            
            current_val = (current_val - bad + MOD) % MOD;
        }
        total_ans = (total_ans + current_val) % MOD;
    }

    cout << total_ans << endl;
    return 0;
}
