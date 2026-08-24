//ODT珂朵莉树
#include <bits/stdc++.h>
#include <cassert>
using namespace std;
using i64 = long long;


template <class T = int>
class ODT {
private:
    struct Node {
        int l, r;
        T v;
        bool operator<(const Node &other) const {
            return l < other.l;
        }
    };
    int n;
    set<Node> s;
    map<T, int> cnt;       // cnt[c] = 颜色 c 的元素总数
    map<T, int> blocks;    // blocks[c] = 颜色 c 的不相交连续区间数量
public:
    ODT(const vector<T> &a): n(a.size() - 1) {
        for (int l = 1, r; l <= n; l = r + 1) {
            r = l;
            while(r < n && a[r + 1] == a[l]) {
                r++;
            }
            s.insert({l, r, a[l]});
            cnt[a[l]] += r - l + 1;
            blocks[a[l]]++;
        }
    }
    // 将包含 x 的区间拆开;返回左端点为 x 的区间
    auto split(int x) {
        if (x > n) {
            return s.end();
        }
        auto it = prev(s.upper_bound({x, 0, {}}));
        if (it->l == x) {
            return it;
        }
        auto [l, r, v] = *it;
        s.erase(it);
        blocks[v]++;
        s.insert({l, x - 1, v});
        return s.insert({x, r, v}).first;
    }
    // 合并 it 与后面的所有相邻同色区间
    auto mergeRight(typename set<Node>::iterator it) {
        while (true) {
            auto nxt = next(it);
            if (nxt == s.end() ||
                it->r + 1 != nxt->l ||
                it->v != nxt->v) {
                break;
            }
            auto [l1, r1, v] = *it;
            auto [l2, r2, _] = *nxt;
            s.erase(it);
            s.erase(nxt);
            blocks[v]--;
            it = s.insert({l1, r2, v}).first;
        }
        return it;
    }
    // 将闭区间 [l, r] 全部染成 v
    void assign(int l, int r, const T &v) {
        auto itr = split(r + 1);
        auto itl = split(l);
        // 删除 [l,r] 内所有旧区间的贡献
        for (auto it = itl; it != itr; ++it) {
            int len = it->r - it->l + 1;

            cnt[it->v] -= len;
            blocks[it->v]--;
        }
        // 删除原区间
        s.erase(itl, itr);
        // 插入新区间
        auto it = s.insert({l, r, v}).first;
        cnt[v] += r - l + 1;
        blocks[v]++;
        // 和左右同色区间合并
        if (it != s.begin()) {
            auto pre = prev(it);
            if (pre->r + 1 == it->l && pre->v == it->v) {
                int nl = pre->l;
                int nr = it->r;

                s.erase(pre);
                s.erase(it);

                blocks[v]--;

                it = s.insert({nl, nr, v}).first;
            }
        }
        it = mergeRight(it);
    }
    // 查询颜色 c 的元素总数
    int count(const T &c) const {
        auto it = cnt.find(c);
        if (it == cnt.end()) {
            return 0;
        }
        return it->second;
    }
    // 查询颜色 c 有多少个不相交连续区间
    int countBlocks(const T &c) const {
        auto it = blocks.find(c);
        if (it == blocks.end()) {
            return 0;
        }
        return it->second;
    }
    vector<T> values() const {
        vector<T> a(n + 1);
        for (auto [l, r, v] : s) {
            fill(a.begin() + l, a.begin() + r + 1, v);
        }
        return a;
    }
};