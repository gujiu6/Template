/*
1.普通DSU
2.带权DSU
3.可撤销DSU
4.可持久化DSU
*/
#include <bits/stdc++.h>
using namespace std;
const int INF = 1e9+7;
using i64 = long long;



//1.普通DSU
class DSU {
public:
    int n;
    vector<int> fa,sz;
    DSU(int n): n(n), fa(n + 1), sz(n + 1, 1) {
        iota(fa.begin(), fa.end(), 0);
    }
    int find(int i){
        if(fa[i] != i){
            fa[i] = find(fa[i]);
        }
        return fa[i];
    }
    bool merge(int x, int y){
        x = find(x), y = find(y);
        if(x == y) return false;
        if(sz[x] < sz[y]) swap(x, y);
        fa[y] = x;
        sz[x] += sz[y];
        return true;
    }
    bool same(int x, int y){
        return find(x) == find(y);
    }
    int size(int x){
        return sz[find(x)];
    }
};

//2.带权DSU
template <typename T = i64>
class WDSU {
public:
    int n;
    vector<int> fa, sz;
    vector<T> w;
    WDSU(int n): n(n), fa(n + 1, 0), w(n + 1, 0), sz(n + 1, 1) {
        iota(fa.begin(), fa.begin() + n + 1, 0);
    }
    int find(int i) {
        if(i != fa[i]) {
            int f = fa[i];
            fa[i] = find(fa[i]);
            w[i] += w[f];
        }
        return fa[i];
    }
    bool merge(int x, int y, T d) {
        int fx = find(x), fy = find(y);
        T v = d + w[x] - w[y];
        if(fx == fy) {
            return v == T{};
        }
        if(sz[fx] < sz[fy]) {
            fa[fx] = fy;
            w[fx] = -v;
            sz[fy] += sz[fx];
        }
        else {
            fa[fy] = fx;
            w[fy] = v;
            sz[fx] += sz[fy];
        }
        return true;
    }
    bool same(int x, int y) {
        return find(x) == find(y);
    }
    int qry(int x, int y) {
        assert(same(x, y));
        return w[y] - w[x];
    }
};

//3.可撤销DSU
class RollbackDSU {
public:
    vector<int> fa, sz;
    struct History {
        int x, px, sx;
        int y, py, sy;
        bool merged;
    };
    vector<History> st;
    RollbackDSU(int n = 0): fa(n + 1), sz(n + 1, 1){
        iota(fa.begin(), fa.end(), 0);
    }
    int find(int i) const {
        while(fa[i] != i) {
            i = fa[i];
        }
        return i;
    }
    int snap() const {
        //返回当前回滚栈高度作为快照编号
        return st.size();
    }
    bool merge(int x, int y) {
        x = find(x), y = find(y);
        if(x == y) {
            st.push_back({x, fa[x], sz[x], y, fa[y], sz[y], false});
            return false;
        }
        if(sz[x] < sz[y]) swap(x, y);
        st.push_back({x, fa[x], sz[x], y, fa[y], sz[y], true});
        sz[x] += sz[y];
        fa[y] = x;
        return true;
    }
    void rollback(int s) {
        //s:此前由snap返回的回滚栈高度;撤销到快照s时的并查集状态,无返回值
        while(st.size() > s) {
            auto [x, px, sx, y, py, sy, merged] = st.back();
            st.pop_back();
            if(merged) {
                fa[x] = px;
                sz[x] = sx;
                fa[y] = py;
                sz[y] = sy;
            }
        }
    }
    bool same(int x, int y) const {
        return find(x) == find(y);
    }
    int size(int i) const {
        return sz[find(i)];
    }
};

//4.可持久化DSU
struct PerDSU {
public:
    struct Node {
        int l = 0, r = 0, p = 0, sz = 0;
    };
    int n;
    vector<Node> t{{}};
    PerDSU(int n = 0): n(n){}
private:
    int build(int l, int r) {
        int i = t.size();
        t.push_back({});
        if(l == r) {
            t[i].p = l;
            t[i].sz = 1;
        }
        else {
            int mid = (l + r) >> 1;
            t[i].l = build(l, mid);
            t[i].r = build(mid + 1, r);
        }
        return i;
    }
    pair<int, int> qry(int i, int l, int r, int p) const {
        //i:第一个元素编号;p:节点编号或当前位置;返回当前位置,节点或结构保存的查询值
        if(l == r) {
            return {t[i].p, t[i].sz};
        }
        int mid = (l + r) >> 1;
        if(p <= mid) return qry(t[i].l, l, mid, p);
        else return qry(t[i].r, mid + 1, r, p);
    }
public:
    //初始版本
    int build() {
        return build(1, n);
    }
    pair<int, int> qry(int root, int p) const{
        //root:版本根编号;p:节点编号或当前位置;返回当前位置,节点或结构保存的查询值
        return qry(root, 1, n, p);
    }
    //修改
    int set(int i, int l, int r, int p, int f, int sz) {
        //i:第一个元素编号;p:节点编号或当前位置;f:父节点数组;sz:子树或集合大小;把指定位置或节点改为给定值,无返回值
        int y = t.size();
        t.push_back(t[i]);
        if(l == r) {
            t[y].p = f;
            t[y].sz = sz;
            return y;
        }
        int mid = (l + r) >> 1;
        if(p <= mid) t[y].l = set(t[y].l, l, mid, p, f, sz);
        else t[y].r = set(t[y].r, mid + 1, r, p, f, sz);
        return y;
    }
    int find(int root, int i) const {
        //root:版本根编号;i:第一个元素编号;返回x所在连通块的代表元,并按当前实现压缩访问路径
        while(true) {
            int p = qry(root, i).first;
            if(p == i) return i;
            i = p;
        }
    }
    int merge(int root, int x, int y) {
        //root要从中继续合并的历史版本根;合并两个元素所在连通块,并返回是否实际发生合并或新的代表元。
        x = find(root, x);
        y = find(root, y);
        if(x == y) return root;
        auto [px, sx] = qry(root, x);
        auto [py, sy] = qry(root, y);
        if(sx < sy) {
            swap(x, y);
            swap(sx, sy);
        }
        root = set(root, 1, n, y, x, sy);
        root = set(root, 1, n, x, x, sx + sy);
        return root;
    }
    bool same(int root, int x, int y) const {
        return find(root, x) == find(root, y);
    }
    int size(int root, int x) const {
        return qry(root, find(root, x)).second;
    }
};