/*
1.Treap
*/
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;


//1.Treap
template <class T = int>
class Treap {
private:
    struct Node {
        T x{};
        unsigned pri = 0;
        int l = 0, r = 0, cnt = 0, sz = 0;//左儿子,右儿子,重复次数,子树大小
    };
    vector<Node> t{{}};
    int root = 0;//根节点编号,节点编号从1开始
    unsigned seed = 712367821;//随机数种子
    unsigned rnd() {
        seed ^= seed << 13;
        seed ^= seed >> 17;
        seed ^= seed << 5;
        return seed;
    }
    int size(int u) const {
        return u ? t[u].sz : 0;
    }
    void pull(int u) {
        t[u].sz = size(t[u].l) + size(t[u].r) + t[u].cnt;
    }
    //d = 1:右旋;d = 0:左旋
    int rot(int u, int d) {
        int v = d ? t[u].l : t[u].r;
        if(d) {
            t[u].l = t[v].r;
            t[v].r = u;
        }
        else {
            t[u].r = t[v].l;
            t[v].l = u;
        }
        pull(u);
        pull(v);
        return v;
    }
    int add(int u, const T& x) {
        if(!u) {
            u = t.size();
            t.emplace_back({x, rnd(), 0, 0, 1, 1});
            return u;
        }
        if(t[u].x == x) {
            t[u].cnt++;
        }
        else if(x < t[u].x) {
            t[u].l = add(t[u].l, x);
            if(t[t[u].l].pri < t[u].pri) {
                u = rot(u, 1);
            }
        }
        else {
            t[u].r = add(t[u].r, x);
            if(t[t[u].r].pri < t[u].pri) {
                u = rot(u, 0);
            }
        }
        pull(u);
        return u;
    }
    //删除一个x, 返回{删除后当前子树的新根,删除前是否存在x}
    pair<int, bool> erase(int u, const T& x) {
        if(!u) {
            return {0, false};
        }
        bool ok = false;
        if(x < t[u].x) {
            auto [v, z] = erase(t[u].l, x);
            t[u].l = v;
            ok = z;
        }
        else if(x > t[u].x) {
            auto [v, z] = erase(t[u].r, x);
            t[u].r = v;
            ok = z;
        }
        else {
            ok = true;
            //有重复元素,只删除一个
            if(t[u].cnt > 1) {
                t[u].cnt--;
            }
            //至少有一个儿子为空
            else if(!t[u].l || !t[u].r) {
                u = t[u].l ? t[u].l : t[u].r;
            }
            //左右儿子都存在
            else {
                //让优先级更小的儿子旋到上面
                int d = (t[t[u].l].pri < t[t[u].r].pri);
                u = rot(u, d);
                if(d) {
                    auto [v, z] = erase(t[u].r, x);
                    t[u].r = v;
                }
                else {
                    auto [v, z] = erase(t[u].l, x);
                    t[u].l = v;
                }
            }
        }
        if(u) pull(u);
        return {u, ok};
    }
public:
    //插入一个x
    void add(const T& x) {
        root = add(root, x);
    }
    //删除一个x,返回删除前是否存在x
    bool erase(const T& x) {
        auto [u, ok] = erase(root, x);
        root = u;
        return ok;
    }
    //返回给定值在当前有序集合中的排名
    int rank(const T& x) const{
        int u = root;
        int ans = 1;
        while(u) {
            if(x <= t[u].x) {
                //x不大于当前值
                u = t[u].l;
            }
            else {
                ans += size(t[u].l) + t[u].cnt;
                u = t[u].r;
            }
        
        }
        return ans;
    }
    //第k小
    T kth(int k) const {
        assert(1 <= k && k <= size(root));
        int u = root;
        while(true) {
            int left_sz = size(t[u].l);
            if(k <= left_sz) {
                u = t[u].l;
            }
            else if(k <= left_sz + t[u].cnt) {
                return t[u].x;
            }
            else {
                k -= left_sz + t[u].cnt;
                u = t[u].r;
            }
        }
    }
    //严格小于x的最大值
    optional<T> prev(const T& x) const {
        int u = root;
        optional<T> ans = nullopt;
        while(u) {
            if(t[u].x < x) {
                ans = t[u].x;
                u = t[u].r;
            }
            else {
                u = t[u].l;
            }
        }
        return ans;
    }
    //严格大于x的最小值
    optional<T> next(const T& x) const {
        int u = root;
        optional<T> ans = nullopt;
        while(u) {
            if(t[u].x > x) {
                ans = t[u].x;
                u = t[u].l;
            }
            else {
                u = t[u].r;
            }
        }
        return ans;
    }
    //x的数量
    int count(const T& x) const {
        int u = root;
        while(u) {
            if(t[u].x == x) {
                return t[u].cnt;
            }
            else if(t[u].x < x) {
                u = t[u].r;
            }
            else {
                u = t[u].l;
            }
        }
        return 0;
    }
};