#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

//Splay
template <class T = int>
class Splay {
private:
    struct Node {
        T x{};
        int ch[2];
        int fa = 0;
        int cnt = 0;//当前值出现次数
        int sz = 0;//子树元素总数
    };
    vector<Node> t{{}};
    int root = 0;
    int size(int u) const {
        return u ? t[u].sz : 0;
    }
    void pull(int u) {
        if(u) {
            t[u].sz = size(t[u].ch[0]) + size(t[u].ch[1]) + t[u].cnt;
        }
    }
    //判断u是其父亲的左儿子还是右儿子
    int dir(int u) const {
        return t[t[u].fa].ch[1] == u;
    }
    //将u旋转上去
    void rot(int u) {
        int p = t[u].fa;
        int g = t[p].fa;
        int d = dir(u);
        int v = t[u].ch[d ^ 1];
        //p->u
        if(g) {
            t[g].ch[t[g].ch[1] == p] = u;
        }
        t[u].fa = g;
        //u的反方向儿子->p
        t[u].ch[d ^ 1] = p;
        t[p].fa = u;
        //v->p的d方向
        t[p].ch[d] = v;
        if(v){
            t[v].fa = p;
        }
        pull(p);
        pull(u);
    }
    // 将u旋转到g的儿子位置
    // g=0表示旋转到整棵树根
    void splay(int u, int g = 0) {
        while(t[u].fa != g) {
            int p = t[u].fa;
            int q = t[p].fa;
            if(q != g) {
                // zig-zig 或 zig-zag
                if((t[q].ch[1] == p) == (t[p].ch[1] == u)) {
                    rot(p);
                }
                else {
                    rot(u);
                }
            }
            rot(u);
        }
        if(!g) {
            root = u;
        }
    }
    // 查找x
    // 找到返回节点编号
    // 找不到返回0，并把最后访问的节点splay到根
    int find(const T& x) {
        int u = root;
        int p = 0;
        while(u && t[u].x != x) {
            p = u;
            u = t[u].ch[t[u].x < x];
        }
        if(u) {
            splay(u);
        }
        else if(p) {
            splay(p);
        }
        return u;
    }

public:
    // 插入一个x
    void add(const T& x) {
        if(!root) {
            root = (int)t.size();
            t.emplace_back({x, {}, 0, 1, 1});
            return;
        }
        int u = root;
        int p = 0;
        int d = 0;
        while(u) {
            if(t[u].x == x) {
                t[u].cnt++;
                pull(u);
                splay(u);
                return;
            }
            p = u;
            d = t[u].x < x;
            u = t[u].ch[d];
        }
        u = (int)t.size();
        t.emplace_back({x, {}, p, 1, 1});
        t[p].ch[d] = u;
        // 插入后将新节点旋到根
        splay(u);
    }
    // 删除一个x
    // 返回删除前是否存在x
    bool erase(const T& x) {
        int u = find(x);
        if(!u) {
            return false;
        }
        // find后u已经是root
        if(t[u].cnt > 1) {
            t[u].cnt--;
            pull(u);
            return true;
        }
        int l = t[u].ch[0];
        int r = t[u].ch[1];
        // 没有左子树
        if(!l) {
            root = r;
            if(r) {
                t[r].fa = 0;
            }
            return true;
        }
        // 先将左子树变成一棵独立的树
        t[l].fa = 0;
        root = l;
        // 找左子树中的最大节点
        int p = l;
        while(t[p].ch[1]) {
            p = t[p].ch[1];
        }
        // 最大节点旋转到左子树根
        splay(p);
        // 此时root=p，且p没有右儿子
        t[p].ch[1] = r;
        if(r) {
            t[r].fa = p;
        }
        pull(p);
        return true;
    }
    // 返回给定值在当前有序集合中的排都改成1base名
    // 和Treap完全一致：1-based
    // 即：返回第一个 >= x 的位置
    int rank(const T& x) {
        int u = root;
        int p = 0;
        int ans = 1;
        while(u) {
            p = u;
            if(x <= t[u].x) {
                u = t[u].ch[0];
            }
            else {
                ans += size(t[u].ch[0]) + t[u].cnt;
                u = t[u].ch[1];
            }
        }
        if(p) {
            splay(p);
        }
        return ans;
    }
    // 第k小
    // 和Treap完全一致：k从1开始
    T kth(int k) {
        assert(1 <= k && k <= size(root));
        int u = root;
        while(true) {
            int left_sz = size(t[u].ch[0]);
            if(k <= left_sz) {
                u = t[u].ch[0];
            }
            else if(k <= left_sz + t[u].cnt) {
                splay(u);
                return t[u].x;
            }
            else {
                k -= left_sz + t[u].cnt;
                u = t[u].ch[1];
            }
        }
    }
    // 严格小于x的最大值
    optional<T> prev(const T& x) {
        int u = root;
        int p = 0;
        int best = 0;

        while(u) {
            p = u;
            if(t[u].x < x) {
                best = u;
                u = t[u].ch[1];
            }
            else {
                u = t[u].ch[0];
            }
        }
        if(best) {
            splay(best);
            return t[best].x;
        }
        if(p) {
            splay(p);
        }
        return nullopt;
    }
    // 严格大于x的最小值
    optional<T> next(const T& x) {
        int u = root;
        int p = 0;
        int best = 0;
        while(u) {
            p = u;
            if(t[u].x > x) {
                best = u;
                u = t[u].ch[0];
            }
            else {
                u = t[u].ch[1];
            }
        }
        if(best) {
            splay(best);
            return t[best].x;
        }
        if(p) {
            splay(p);
        }
        return nullopt;
    }
    // x的数量
    int count(const T& x) {
        int u = find(x);
        return u ? t[u].cnt : 0;
    }
};