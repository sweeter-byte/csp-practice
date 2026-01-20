#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// Constants and Globals
const int MAX_NODES = 20000005; // 2e7 nodes should be sufficient
const int MAX_BITS = 30;

struct Node {
    int ls, rs; // Left child (bit 0), Right child (bit 1)
    int cnt;    // Number of elements in this subtree
    int ans;    // Min partitioning cost (chromatic number) for this subtree
} tree[MAX_NODES];

int pool_ptr = 0;
int new_node() {
    int u = ++pool_ptr;
    tree[u].ls = tree[u].rs = tree[u].cnt = tree[u].ans = 0;
    return u;
}

int n, q, W;
int root[500005];
bool active[500005];
long long total_repair_cost = 0;

// Declaration of match function
int max_match(int u, int v, int dep);

// Pushup: Calculate cnt and ans from children
void pushup(int u, int dep) {
    int l = tree[u].ls;
    int r = tree[u].rs;
    tree[u].cnt = tree[l].cnt + tree[r].cnt;
    
    if (tree[u].cnt == 0) {
        tree[u].ans = 0;
        return;
    }

    int w_bit = (W >> dep) & 1;
    if (w_bit == 0) {
        // Current bit of W is 0.
        // Elements in L and R are compatible (XOR result has bit 1 > 0).
        // Conflict graph is disjoint union of L and R conflicts.
        // We can reuse colors.
        int ans_l = l ? tree[l].ans : 0;
        int ans_r = r ? tree[r].ans : 0;
        tree[u].ans = max(ans_l, ans_r);
    } else {
        // Current bit of W is 1.
        // Elements in L (among themselves) conflict (XOR < W). L is clique.
        // Elements in R (among themselves) conflict (XOR < W). R is clique.
        // We need to pair elements x in L and y in R such that x ^ y >= W to save colors.
        // Cost = |L| + |R| - max_matching(L, R).
        int match = max_match(l, r, dep - 1);
        tree[u].ans = tree[l].cnt + tree[r].cnt - match;
    }
}

// Computes max matching size between subtree u and v at depth `dep`
// Compatibility condition: x in u, y in v, x ^ y >= W (considering bits 0..dep)
int max_match(int u, int v, int dep) {
    if (!u || !v) return 0;
    if (dep < 0) return min(tree[u].cnt, tree[v].cnt); // Leaf level matching

    int w_bit = (W >> dep) & 1;
    if (w_bit == 1) {
        // W bit is 1. We MUST pick x, y such that x^y has bit 1.
        // So must pair (u->left, v->right) and (u->right, v->left).
        // (same-side pairs yield bit 0 < 1, which fails immediately)
        return max_match(tree[u].ls, tree[v].rs, dep - 1) + 
               max_match(tree[u].rs, tree[v].ls, dep - 1);
    } else {
        // W bit is 0.
        // Pairs differing at this bit (cross pairs) yield 1 > 0. AUTOMATICALLY COMPATIBLE.
        // Pairs same at this bit (parallel pairs) yield 0. Need to check lower bits.
        // This forms a specialized max flow problem.
        // S -> L1, R1; L2, R2 -> T.
        // Cross edges (L1->R2, R1->L2) are infinite capacity (always match).
        // Parallel edges (L1->L2, R1->R2) are constrained by recursive match.
        
        int A = max_match(tree[u].ls, tree[v].ls, dep - 1); // Match between L1 and L2
        int B = max_match(tree[u].rs, tree[v].rs, dep - 1); // Match between R1 and R2
        
        int cntL1 = tree[tree[u].ls].cnt;
        int cntR1 = tree[tree[u].rs].cnt;
        int cntL2 = tree[tree[v].ls].cnt;
        int cntR2 = tree[tree[v].rs].cnt;
        
        // Min-Cut formula result
        int res = cntL1 + cntR1; // Cut Source
        res = min(res, cntL2 + cntR2); // Cut Sink
        res = min(res, cntL1 + cntL2 + B); // Cut mixed 1
        res = min(res, cntR1 + cntR2 + A); // Cut mixed 2
        return res;
    }
}

// Insert value x into subtree u
void insert(int &u, int x, int dep) {
    if (!u) u = new_node();
    if (dep < 0) {
        tree[u].cnt++;
        tree[u].ans = tree[u].cnt; // Each copy forms a clique with others? Yes x^x=0 < W.
        return;
    }
    int bit = (x >> dep) & 1;
    if (bit == 0) insert(tree[u].ls, x, dep - 1);
    else insert(tree[u].rs, x, dep - 1);
    pushup(u, dep);
}

// Merge tree v into u
int merge(int u, int v, int dep) {
    if (!u || !v) return u | v;
    
    // If leaf, just add counts
    if (dep < 0) {
        tree[u].cnt += tree[v].cnt;
        tree[u].ans = tree[u].cnt;
        return u;
    }
    
    // Recursive merge
    tree[u].ls = merge(tree[u].ls, tree[v].ls, dep - 1);
    tree[u].rs = merge(tree[u].rs, tree[v].rs, dep - 1);
    
    // Recompute attributes
    pushup(u, dep);
    return u;
}

int main() {
    // Optimize IO
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> n >> W)) return 0;

    for (int i = 1; i <= n; ++i) {
        int m;
        cin >> m;
        root[i] = 0;
        active[i] = true;
        for (int j = 0; j < m; ++j) {
            int val;
            cin >> val;
            insert(root[i], val, MAX_BITS);
        }
        total_repair_cost += tree[root[i]].ans;
    }

    cin >> q;
    while (q--) {
        int op;
        cin >> op;
        if (op == 1) {
            int u, x;
            cin >> u >> x;
            // Subtract old cost
            total_repair_cost -= tree[root[u]].ans;
            insert(root[u], x, MAX_BITS);
            // Add new cost
            total_repair_cost += tree[root[u]].ans;
        } else if (op == 2) {
            int u, v;
            cin >> u >> v;
            total_repair_cost -= tree[root[u]].ans;
            total_repair_cost -= tree[root[v]].ans;
            root[u] = merge(root[u], root[v], MAX_BITS);
            active[v] = false;
            // No need to clear root[v] as it's not reused and we use pool
            total_repair_cost += tree[root[u]].ans;
        } else if (op == 3) {
            cout << total_repair_cost << "\n";
        }
    }

    return 0;
}
