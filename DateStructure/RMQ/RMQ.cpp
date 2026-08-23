/*RMQ与静态区间结构                   时间复杂度       空间复杂度
1.普通ST                                            O(nlogn)
    建表                            O(nlogn)
    查询                            O(1)
4.笛卡尔树Cartesian                  O(n)           O(n)
*/
#include <bits/stdc++.h>
#include <cassert>
#include <bit>
using namespace std;
const int INF = 1e9+7;


//1.普通ST
template <class T , class F>
class ST1 {
public:
    int n, lg2;
    vector<vector<T>> st;
    F op;
    ST1(const vector<T>& a, F f = {}): n(a.size() - 1), op(f) {
        lg2 = n ? bit_width(unsigned(n)) : 0;
        st.assign(lg2 + 1, vector<T>(n + 1));
        if(!n) return;
        st[0] = a;
        for(int p = 1; p <= lg2; p++) {
            for(int i = 1; i + (1LL << p) - 1 <= n; i++) {
                st[p][i] = op(st[p - 1][i], st[p - 1][i + (1LL << (p - 1))]);
            }
        }
    }

    T qry(int l, int r) const {
        assert(1 <= l && l <= r && r <= n);
        int p = bit_width(unsigned(r - l + 1)) - 1;
        T ans = op(st[p][l], st[p][r - (1LL << p) + 1]);
        return ans;
    }
};

//4.笛卡尔树Cartesian, index:BST, value:Heap
//中序遍历仍是原下标顺序
namespace Cartesian {

struct Cart {
    int root = 0;
    vector<int> fa, left, right;
};
template <class T = int>
Cart Cartesian(const vector<T>& a) {
    int n = (int)a.size() - 1;
    Cart c{0, vector<int>(n + 1, 0), vector<int>(n + 1, 0), vector<int>(n + 1, 0)};
    vector<int> s;
    s.reserve(n);
    for(int i = 1; i <= n; i++) {
        int last = 0;
        //维护单调不降栈
        while(!s.empty() && a[i] < a[s.back()]) {
            last = s.back();
            s.pop_back();
        }
        //i成为栈顶的右儿子
        if(!s.empty()) {
            c.fa[i] = s.back();
            c.right[s.back()] = i;
        }
        //最后弹出的节点成为i的左儿子
        if(last != 0) {
            c.fa[last] = i;
            c.left[i] = last;
        }
        s.emplace_back(i);
    }
    if(n) {
        c.root = s[0];
    }
    return c;
}

}
