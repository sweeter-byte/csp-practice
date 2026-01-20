#include<bits/stdc++.h>
using namespace std;

const int MAXZ = 405;
char grid[MAXZ][MAXZ];
int Z;
int current_rotation = 0; // 0: 0, 1: 90 CW, 2: 180 CW, 3: 270 CW relative to original view

struct Step {
    int op;
    int u, v, L, d, r_rot; // For op 1
    int u2, d2, l2, r2, o; // For op 2
};

void map_coord(int r, int c, int &rr, int &cc) {
    int times = current_rotation % 4;
    int tr = r, tc = c;
    for (int i = 0; i < times; ++i) {
        // Rotate (tr, tc) 90 degrees CCW
        int nr = Z - 1 - tc;
        int nc = tr;
        tr = nr;
        tc = nc;
    }
    rr = tr;
    cc = tc;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> Z;

    for (int i = 0; i < Z; ++i) {
        cin >> grid[i]; // 直接读入一行，自动补 \0
    }

    int k;
    cin >> k;
    int t;
    cin >> t;

    vector<Step> steps;
    steps.reserve(t);

    for (int i = 0; i < t; ++i) {
        Step s;
        cin >> s.op;
        if (s.op == 1) {
            cin >> s.u >> s.v >> s.L >> s.d >> s.r_rot;
        } else {
            cin >> s.u2 >> s.d2 >> s.l2 >> s.r2 >> s.o;
        }
        steps.push_back(s);
    }

    // Reverse simulation
    for (int i = t - 1; i >= 0; --i) {
        const Step &s = steps[i];

        if (s.op == 1) {
            current_rotation = (current_rotation + s.r_rot) % 4;
        }
        
        if (s.op == 1) {
            int u = s.u - 1;
            int v = s.v - 1;
            int L = s.L;
            int d = s.d; // Clockwise degrees
            
            vector<string> block(L, string(L, ' '));
            for(int r = 0; r < L; ++r) {
                for(int c = 0; c < L; ++c) {
                    int rr, cc;
                    map_coord(u + r, v + c, rr, cc);
                    block[r][c] = grid[rr][cc];
                }
            }

            vector<string> new_block = block;
            int times_ccw = (d / 90) % 4; // d is 90, 180, 270.
            
            for(int r = 0; r < L; ++r) {
                for(int c = 0; c < L; ++c) {
                    int nr = r, nc = c;
                    // Apply CCW rotation `times_ccw` times to (r, c)
                    for(int k=0; k<times_ccw; ++k) {
                        int tr = L - 1 - nc;
                        int tc = nr;
                        nr = tr;
                        nc = tc;
                    }
                    new_block[nr][nc] = block[r][c];
                }
            }
            
            // Write back
            for(int r = 0; r < L; ++r) {
                for(int c = 0; c < L; ++c) {
                    int rr, cc;
                    map_coord(u + r, v + c, rr, cc);
                    grid[rr][cc] = new_block[r][c];
                }
            }
            
        } else { // op == 2
            int u = s.u2 - 1;
            int d_row = s.d2 - 1;
            int l = s.l2 - 1;
            int r_col = s.r2 - 1;
            int o = s.o;
            
            int H = d_row - u + 1;
            int W = r_col - l + 1;
            
            vector<string> block(H, string(W, ' '));
            for(int r = 0; r < H; ++r) {
                for(int c = 0; c < W; ++c) {
                    int rr, cc;
                    map_coord(u + r, l + c, rr, cc);
                    block[r][c] = grid[rr][cc];
                }
            }
            
            vector<string> new_block = block;
            
            if (o == 1) { // Up-Down Flip
                // Inverse is Up-Down Flip
                for(int r = 0; r < H; ++r) {
                     new_block[r] = block[H - 1 - r];
                }
            } else { // Left-Right Flip
                // Inverse is Left-Right Flip
                 for(int r = 0; r < H; ++r) {
                    for(int c = 0; c < W; ++c) {
                        new_block[r][c] = block[r][W - 1 - c];
                    }
                }
            }
            
            // Write back
             for(int r = 0; r < H; ++r) {
                for(int c = 0; c < W; ++c) {
                    int rr, cc;
                    map_coord(u + r, l + c, rr, cc);
                    grid[rr][cc] = new_block[r][c];
                }
            }
        }
    }

    int max_r = -1;
    int max_c = -1;
    
    // Buffer to hold logical image for efficient output
    vector<string> logical_grid(Z, string(Z, ' '));
    
    for(int r=0; r<Z; ++r) {
        for(int c=0; c<Z; ++c) {
            int rr, cc;
            map_coord(r, c, rr, cc);
            char ch = grid[rr][cc];
            logical_grid[r][c] = ch;
            if (ch != '?') {
                if (r > max_r) max_r = r;
                if (c > max_c) max_c = c;
            }
        }
    }
    
    int n = max_r + 1;
    int m = max_c + 1;
    
    cout << n << " " << m << "\n";
    for(int r=0; r<n; ++r) {
        for(int c=0; c<m; ++c) {
            cout << logical_grid[r][c];
        }
        cout << "\n";
    }

    return 0;
}
