/*
1.二维前缀和
2.二维差分
3.k阶差分
4.等差数列差分
5.Kadane求非空最大子 段/矩阵 和
6.所有区间 gcd
*/
#include <bits/stdc++.h>
#include <cassert>
using namespace std;
using i64 = long long;

//1.二维前缀和
template <class T = i64>
struct Prefix {
    int n, m;
    vector<vector<T>> s;
    Prefix(const vector<vector<T>>& a): n(a.size() - 1), m(a[0].size() - 1), s(n + 1, vector<T>(m + 1)) {
        for(int i = 1; i <= n; i++) {
            for(int j = 1; j <= n; j++) {
                s[i][j] = s[i - 1][j] + s[i][j - 1] - s[i - 1][j - 1] + a[i][j];
            }
        }
    }
    T sum(int x1, int y1, int x2, int y2)const {
        return s[x2][y2] - s[x1 - 1][y2] - s[x2][y1 - 1] + s[x1 - 1][y1 - 1];
    }
};

//2.二维差分
template <class T = i64>
struct Diff {
    int n, m;
    vector<vector<T>> d;
    Diff(int n, int m): n(n), m(m), d(n + 2, vector<T>(m + 2)) {}
    void add(int x1, int y1, int x2, int y2, T v) {
        assert(0 <= x1 && x1 <= x2 && x2 <= n);
        assert(0 <= y1 && y1 <= y2 && y2 <= n);
        d[x1][y1] += v;
        d[x2 + 1][y1] -= v;
        d[x1][y2 + 1] -= v;
        d[x2 + 1][y2 + 1] += v;
    }
    vector<vector<T>> qry() const {
        vector<vector<T>> a(n + 1, vector<T>(m + 1));
        for(int i = 1; i <= n; i++) {
            for(int j = 1; j <= m; j++) {
                a[i][j] = d[i][j] + d[i][j - 1] + d[i - 1][j] - d[i - 1][j - 1];
            }
        }
        return a;
    }
};

//3.k阶差分
template <class T = i64>
vector<T> kthDiff(vector<T> a, int k) {
    assert(k >= 0);
    while(k--) {
        for(int i = 1; i < a.size() - 1; i++) {
            a[i] = a[i + 1] - a[i];
        }
        if(!a.empty()) {
            a.pop_back();
        }
    }
    return a;
}

//4.等差数列差分
template <class T = i64>
struct ArithmeticDiff {
    int n;
    vector<T> dd;//二阶差分
    ArithmeticDiff (int n): n(n), dd(n + 2) {}
    //[l,r]加等差数列初始s,公差d
    void add(int l, int r, T s, T d) {
        int len = (r - l + 1);
        T t = s + (len - 1) * d;
        dd[l] += s;
        dd[l + 1] += d - s;
        dd[r + 1] -= t + d;
        dd[r + 2] += t;
    }
    vector<T> build() {
        vector<T> a(n + 1);
        for(int i = 1; i <= n; i++) {
            dd[i] += dd[i - 1];
        }
        for(int i = 1; i <= n; i++) {
            a[i] = a[i - 1] + dd[i];
        }
        return a;
    }
};

//5.Kadane求非空最大子段和
template <class T = i64>
T maxSub(const vector<T>& a) {
    assert(!a.empty());
    auto ans = a[1], cur = a[1];
    for(int i = 2; i < a.size(); i++) {
        cur = max(a[i], cur + a[i]);
        ans = max(ans, cur);
    }
    return ans;
}
//子矩阵和
template <class T = i64>
T maxSubMat(const vector<vector<T>>& a) {
    assert(!a.empty() && !a[0].empty());
    int n = a.size() - 1, m = a[0].size();
    auto ans = a[1][1];
    for(int u = 1; u <= n; u++) {
        vector<T> s(m + 1);
        for(int d = u; d <= n; d++) {
            for(int j = 1; j <= m; j++) {
                s[j] += a[d][j];
            }
            ans = max(ans, maxSub(s));
        }
    }
    return ans;
}

//6.所有区间 gcd
template <class T = i64>
struct gcdRanges {
    int n;
    vector<vector<pair<int, T>>> g;
    gcdRanges (const vector<T>& a): n(a.size() - 1), g(n + 1) {
        vector<pair<int, T>> pre;
        for(int r = 1; r <= n; r++) {
            vector<pair<int, T>> cur{{r, abs(a[r])}};
            for(auto[l, x] : pre) {
                x = gcd(x, abs(a[r]));
                if(cur.back().second == x) {
                    cur.back().first = l;
                }
                else {
                    cur.emplace_back({l, x});
                }
            }
            reverse(cur.begin(), cur.end());
            g[r] = cur;
            pre = move(cur);
            reverse(pre.begin(), pre.end());
        }
    }
    //[l, r]区间gcd
    T qry(int l, int r) const {
        assert(1 <= l && l <= r && r <= n);
        auto it = upper_bound(g[r].begin(), g[r].end(), l, [](int x, const auto& p){
            return x < p.first;
        });
        return prev(it)->second;
    }
};