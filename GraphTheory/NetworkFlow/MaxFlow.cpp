/*                      时间复杂度       空间复杂度
1.Dinic最大流             O(V^2E)        O(V+E)
2.HLPP/预流推进(更快)      O(V^2E)        O(V+E)
*/
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

//1.Dinic最大流
template <class T = int>
class Flow {
public:
    struct E {
        int v, rev;//v:转移目标状态;rev:反向边编号
        T cap;//剩余容量
    };
    int n;//残量网络点数
    vector<vector<E>> e;//每个点发出的残量边
    vector<int> h, cur;//h:Dinic层次图中的距离标号,cur:每个点尚未尝试的第一条残量边下标
    Flow(int n = 0): n(n), e(n + 1), h(n + 1), cur(n + 1) {}
    //添加有向边u->v,容量c
    int add(int u, int v, T c) {
        int id = e[u].size();
        e[u].push_back({v, (int)e[v].size(), c});
        e[v].push_back({u, id, 0});
        return id;
    }
    //BFS建立层次图
    bool bfs(int s, int t) {
        fill(h.begin(), h.end(), -1);
        queue<int> q;
        h[s] = 0;
        q.push(s);
        while(!q.empty()) {
            auto u = q.front(); q.pop();
            for(const auto &[v, r, c] : e[u]) {
                if(c > 0 && h[v] == -1) {
                    h[v] = h[u] + 1;
                    q.push(v);
                }
            }
        }
        return h[t] != -1;
    }
    //DFS增广
    T dfs(int u, int t, T f) {
        if(u == t) return f;
        T ans = 0;
        for(int &i = cur[u]; i < e[u].size(); i++) {
            auto &a = e[u][i];
            if(a.cap == 0 || h[a.v] != h[u] + 1) {
                continue;
            }
            T d = dfs(a.v, t, min(f - ans, a.cap));
            a.cap -= d;
            e[a.v][a.rev].cap += d;
            ans += d;
            if(ans == f) break;
        }
        return ans;
    }
    //最大流
    T flow(int s, int t, T lim = numeric_limits<T>::max()) {
        T ans = 0;
        while(ans < lim && bfs(s, t)) {
            fill(cur.begin(), cur.end(), 0);
            ans += dfs(s, t, lim - ans);
        }
        return ans;
    }
    //最大流结束后,返回残量网络中从s可达的点集,即最小割的源点
    vector<bool> cut(int s) const {
        vector<bool> vis(n + 1);
        queue<int> q;
        vis[s] = true;
        q.push(s);
        while(!q.empty()) {
            auto u = q.front(); q.pop();
            for(const auto &[v, r, c] : e[u]) {
                if(c > 0 && !vis[v]) {
                    vis[v] = true;
                    q.push(v);
                }
            }
        }
        return vis;
    }
};

//2.HLPP/预流推进
template <class T = int>
class PushRelabel {
public:
    struct E {
        int v, rev;
        T c;
    };
    int n;
    vector<vector<E>> e;//残量网络邻接表
    vector<T> ex;//每个点当前的超额流
    vector<int> h, cur;
    vector<bool> in;
    queue<int> q;
    PushRelabel(int n = 0): n(n), e(n + 1), ex(n + 1), in(n + 1), h(n + 1), cur(n + 1) {}
    //添加有向边u->v,容量为c
    void add(int u, int v, T c) {
        e[u].push_back({v, (int)e[v].size(), c});
        e[v].push_back({u, (int)e[u].size() - 1, c});
    }
    //将具有正超额流的点加入活跃队列
    void active(int u, int s, int t) {
        if (u != s && u != t && ex[u] > 0 && !in[u]) {
            in[u] = true;
            q.push(u);
        }
    }
    // 沿允许边推送流量。
    void push(int u, E &a, int s, int t) {
        if (!a.c || h[u] != h[a.v] + 1) {
            return;
        }
        T f = min(ex[u], a.c);
        //修改正向残量边
        a.c -= f;
        //修改反向残量边
        e[a.v][a.r].c += f;
        //更新两个端点的超额流
        ex[u] -= f;
        ex[a.v] += f;
        //如果终点成为活跃点，则加入队列。
        active(a.v, s, t);
    }
    // 求 s -> t 的最大流。
    T flow(int s, int t) {
        if (s == t) {
            return 0;
        }
        // 初始化源点高度。
        h[s] = n;
        // 将源点的所有流预先推出。
        for (auto &a : e[s]) {
            T f = a.c;
            // 正向边容量全部推出。
            a.c = 0;
            // 反向边增加对应容量。
            e[a.v][a.r].c += f;
            // 终点获得超额流。
            ex[a.v] += f;
            // 源点失去超额流
            ex[s] -= f;
            // 激活终点。
            active(a.v, s, t);
        }
        // 不断处理活跃点。
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            in[u] = false;
            while (ex[u] > 0){
                // 当前弧已经全部尝试过
                if (cur[u] == (int)e[u].size()) {
                    // 重新计算高度
                    h[u] = 2 * n;
                    for (auto a : e[u]) {
                        if (a.c) {
                            h[u] = min(h[u], h[a.v] + 1);
                        }
                    }
                    // 重新从第一条边开始
                    cur[u] = 0;
                    continue;
                }
                // 尝试沿当前弧推流。
                push(u, e[u][cur[u]], s, t);
                // 当前边已经不能继续使用,或者不满足允许边条件,则跳过
                if (!e[u][cur[u]].c || h[u] != h[e[u][cur[u]].v] + 1){
                    ++cur[u];
                }
            }
        }
        return ex[t];
    }

};
