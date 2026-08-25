/*
1.字符串哈希
2.KMP
3.扩展KMP/Z函数
4.Manacher
*/
#include <bits/stdc++.h>
#include <cassert>
using namespace std;
using i64 = long long;


//1.字符串哈希(1-based)
class StrHash {
private:
    static i64 get_base() {
        static const i64 base = [] {
            mt19937_64 rng(
                chrono::steady_clock::now().time_since_epoch().count()
                ^ (uintptr_t)new char
            );
            uniform_int_distribution<i64> dist(256, min(p1, p2) - 2);
            return dist(rng);
        }();

        return base;
    }
public:
    static constexpr i64 p1 = 1000000007;
    static constexpr i64 p2 = 1000000009;
    i64 base;
    vector<i64> h1, h2, pw1, pw2;
    StrHash(const string& s, i64 base = 911382323): base(base), h1(s.size() + 1), h2(s.size() + 1), pw1(s.size() + 1, 1), pw2(s.size() + 1, 1) {
        int n = s.size() - 1;
        for(int i = 1; i <= n; i++) {
            int c = (unsigned char)s[i] + 1;
            h1[i] = (h1[i - 1] * base + c) % p1;
            h2[i] = (h2[i - 1] * base + c) % p2;
            pw1[i] = pw1[i - 1] * base % p1;
            pw2[i] = pw2[i - 1] * base % p2;
        }
    }
    //s[l,r]的哈希
    pair<i64, i64> get(int l, int r) const {
        assert(1 <= l && l <= r && r < h1.size());
        i64 x = (h1[r] - h1[l - 1] * pw1[r - l + 1] % p1 + p1) % p1;
        i64 y = (h2[r] - h2[l - 1] * pw2[r - l + 1] % p2 + p2) % p2;
        return {x, y};
    }
    //在s中寻找t的所有出现位置
    static vector<int> math(const string& s, const string& t) {
        int n = s.size() - 1, m = t.size() - 1;
        if(m > n) return {};
        StrHash a(s), b(t);
        vector<int> ans;
        for(int i = 1; i + m - 1 <= n; i++) {
            if(a.get(i, i + m - 1) == b.get(1, m)) {
                ans.push_back(i);
            }
        }
        return ans;
    }
};

//2.KMP
namespace KMP {

vector<int> prefix(const string& s) {
    int n = s.size() - 1;
    vector<int> p(n + 1);
    for(int i = 2; i <= n; i++) {
        int j = p[i - 1];
        while(j && s[i] != s[j + 1]) {
            j = p[j];
        }
        if(s[i] == s[j + 1]) {
            j++;
        }
        p[i] = j;
    }
    return p;
}

vector<int> KMP(const string &s, const string &t) {
    int n = s.size() - 1, m = t.size() - 1;
    if(m == 0) {
        vector<int> ans(n);
        iota(ans.begin(), ans.end(), 1);
        return ans;
    }
    auto p = prefix(t);
    vector<int> ans;
    int j = 0;
    for(int i = 1; i <= n; i++) {
        while(j && s[i] != t[j + 1]) {
            j = p[j];
        }
        if(s[i] == t[j + 1]) {
            j++;
        }
        if(j == m) {
            ans.push_back(i - m + 1);
            j = p[j];
        }
    }
    return ans;
}

//最小循环元
string minPeriod(const string& s) {
    int n = s.size() - 1;
    if(n == 0) return "";
    auto p = prefix(s);
    int len = n - p[n];
    if(n % len == 0) return s.substr(1, len);
    else return s;
}
//返回所有非空真Border长度,递增
vector<int> borders(const string &s) {
    int n = s.size() - 1;
    if (n == 0) return {};
    auto p = prefix(s);
    vector<int> ans;
    //n本身不是真Border,从最长真Border开始
    for (int x = p[n]; x; x = p[x]) {
        ans.push_back(x);
    }
    reverse(ans.begin(), ans.end());
    return ans;
}


}

//3.扩展KMP/Z函数
//z[i]:记录后缀s[i,n]与整串前缀的最长公共前缀长度
namespace Zfunc {

vector<int> Zfunc(const string& s) {
    int n = s.size() - 1;
    vector<int> z(n + 1);
    z[1] = n;
    for(int i = 2, l = 1, r = 1, len; i <= n; i++) {
        len = r >= i ? min(r - i + 1, z[i - l + 1]) : 0;
        while(i + len <= n && s[i + len] == s[1 + len])
            len++;
        if(i + len - 1 > r) {
            l = i;
            r = i + len - 1;
        }
        z[i] = len;
    }
    return z;
}
vector<int> Efunc(const string& s, const string& t) {
    int n = s.size() - 1, m = t.size() - 1;
    vector<int> e(n + 1), z = Zfunc(t);
    for(int i = 1, c = 1, r = 0, len; i <= n; i++) {
        len = r >= i ? min(r - i + 1, z[i - c + 1]) : 0;
        while(i + len <= n && len < m && s[i + len] == t[1 + len]) 
            len++;
        if(i + len - 1 > r) {
            c = i;
            r = i + len - 1;
        }
        e[i] = len;
    }
    return e;
}

}

//4.Manacher(zuo神)
class Manacher {
public:
    string s;
    string ss;//ss[1..m] = #a#b#c#
    vector<int> p;//以 ss[i] 为中心的回文半径
    int n, m;
    Manacher(const string& s) : s(s) {
        n = s.size() - 1;
        m = 2 * n + 1;
        ss.assign(m + 1, '#');
        p.assign(m + 1, 0);
        // 构造 #a#b#c#
        for (int i = 1, j = 1; i <= m; i++) {
            ss[i] = (i & 1) ? '#' : s[j++];
        }
        //c:当前最右回文的中心;r:当前最右回文的右边界的下一位
        for (int i = 1, c = 1, r = 1, len; i <= m; i++) {
            len = r > i ? min(p[2 * c - i], r - i) : 1;
            while (i - len >= 1 && i + len <= m && ss[i - len] == ss[i + len]) {
                len++;
            }
            if (i + len > r) {
                c = i;
                r = i + len;
            }
            //原串回文长度:p[i]-1
            p[i] = len;
        }
    }
    //最长奇数长度回文子串
    string longestOdd() const {
        int pos = 1;
        for (int i = 1; i <= m; i += 2) {
            if (p[i] > p[pos]) {
                pos = i;
            }
        }
        int len = p[pos] - 1;
        int l = (pos - p[pos] + 1) / 2 + 1;
        return s.substr(l - 1, len);
    }
    //最长偶数长度回文子串
    string longestEven() const {
        int pos = 2;
        for (int i = 2; i <= m; i += 2) {
            if (p[i] > p[pos]) {
                pos = i;
            }
        }
        int len = p[pos] - 1;
        int l = (pos - p[pos] + 1) / 2 + 1;
        return s.substr(l - 1, len);
    }
    //最长回文子串
    string longest() const {
        string a = longestOdd();
        string b = longestEven();
        return a.size() >= b.size() ? a : b;
    }
    //判断原字符串 s[l..r] 是否为回文串
    bool isPal(int l, int r) const {
        if(l > r || l < 1 || r > n) {
            return false;
        }
        int center = l + r;//原串 [l,r] 在处理串中的中心
        int len = r - l + 1;//原串长度
        return p[center] >= len + 1;
    }
};