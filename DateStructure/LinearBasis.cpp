/*线性基
1.异或线性基
2.空间向量线性基
3.区间线性基
*/
#include <bits/stdc++.h>
#include <cassert>
using namespace std;
using i64 = long long;
using u64 = unsigned long long;
using ld = long double;
const ld eps = 1e-12;

//1.异或线性基
template <class T = u64, int W = 64>
class XorBasis {
public:
    array<T, W> a{};
    int rk = 0;//当前异或向量空间的秩
    // 插入x,返回是否使秩增加
    bool add(T x) {
        for (int i = W - 1; i >= 0; i--) {
            if (!(x >> i & 1)) continue;
            if (!a[i]) {
                a[i] = x;
                rk++;
                return true;
            }
            x ^= a[i];
        }
        return false;
    }
    // 判断 x 是否能由当前线性基异或表示
    bool has(T x) const {
        for (int i = W - 1; i >= 0; i--) {
            if ((x >> i & 1) && a[i]) {
                x ^= a[i];
            }
        }
        return x == 0;
    }
    // 返回所有可表示数中与 x 异或后的最大值
    T maxXor(T x = 0) const {
        for (int i = W - 1; i >= 0; i--) {
            x = max(x, x ^ a[i]);
        }
        return x;
    }
    // 合并另一个异或线性基
    void merge(const XorBasis &o) {
        for (auto &x : o.a) {
            if (x) {
                add(x);
            }
        }
    }
    // 返回第 k 小的可表示数，k 从 0 开始;largest={false:第 k 小;true:第 k 大}
    optional<T> kth(T k, bool largest = false) const {
        // rk == W 时不能计算 1ULL << W
        if (rk < W && k >= (T(1) << rk)) {
            return nullopt;
        }
        if(largest) {
            T total;
            if(rk == W) {
                total = numeric_limits<T>::max();
            }
            else {
                total = (T(1) << rk) - 1;
            }
            k = total - k;
        }
        auto b = a;
        //化为更规范的形式
        for (int i = 0; i < W; i++) {
            if (!b[i]) continue;
            for (int j = i + 1; j < W; j++) {
                if (b[j] >> i & 1) {
                    b[j] ^= b[i];
                }
            }
        }
        vector<T> v;
        // 从低位主元到高位主元
        for (int i = 0; i < W; i++) {
            if (b[i]) {
                v.push_back(b[i]);
            }
        }
        T ans = 0;
        for (int i = 0; i < (int)v.size(); i++) {
            if (k >> i & 1) {
                ans ^= v[i];
            }
        }
        return ans;
    }
    // 返回所有非零主元
    vector<T> getBasis() const {
        vector<T> res;
        for (int i = W - 1; i >= 0; i--) {
            if (a[i]) {
                res.push_back(a[i]);
            }
        }
        return res;
    }
    // 当前空间大小,即可表示的不同数的个数;rk == 64 时 2^64 无法用 u64 表示
    optional<T> size() const {
        if (rk == 64) {
            return nullopt;
        }
        return 1ULL << rk;
    }
};

//2.空间向量线性基
template <class T = ld>
class VectorBasis {
public:
    vector<vector<T>> a;// a[i]：主元在第 i 维的基向量
    int n;// 向量维度
    int rk = 0;// 当前空间的
    VectorBasis(int n) : n(n), a(n, vector<T>(n)) {}
    // 返回 x 是否使秩增加
    bool add(vector<T> x) {
        assert((int)x.size() == n);
        // 从左到右寻找主元
        for (int i = 0; i < n; i++) {
            if (fabsl(x[i]) < eps) continue;
            // 当前这一维没有主元
            if (a[i][i] < eps) {
                // 将主元归一化
                T p = x[i];
                for (int j = i; j < n; j++) {
                    x[j] /= p;
                }
                a[i] = x;
                rk++;
                return true;
            }
            // 消掉第 i 维
            T p = x[i];
            for (int j = i; j < n; j++) {
                x[j] -= p * a[i][j];
            }
        }
        return false;
    }
    // 判断 x 是否属于当前线性空间
    bool has(vector<T> x) const {
        assert((int)x.size() == n);
        for (int i = 0; i < n; i++) {
            if (x[i] < eps) continue;
            // 没有对应主元，无法表示
            if (a[i][i] < eps) {
                return false;
            }
            // 消掉第 i 维
            T p = x[i];
            for (int j = i; j < n; j++) {
                x[j] -= p * a[i][j];
            }
        }
        return true;
    }
    // 合并另一个线性基
    void merge(const VectorBasis &o) {
        assert(n == o.n);
        for (int i = 0; i < n; i++) {
            if (!isZero(o.a[i][i])) {
                add(o.a[i]);
            }
        }
    }
    // 返回当前线性基
    vector<vector<T>> getBasis() const {
        vector<vector<T>> res;
        for (int i = 0; i < n; i++) {
            if (!isZero(a[i][i])) {
                res.push_back(a[i]);
            }
        }
        return res;
    }
};

//3.区间线性基
template <class T = u64, int Bits = numeric_limits<T>::digits>
class RangeBasis {
    struct Entry {
        T value = 0;
        int pos = 0;// 该基向量对应的最靠右可用位置;查询 [l, r] 时只有 position >= l 的基向量才能使用
    };
    vector<array<Entry, Bits>> pre;//前缀 [0, i] 的异或线性基
public:
    RangeBasis() = default;
    explicit RangeBasis(const vector<T> &a) {
        build(a);
    }
    // 建立区间线性基
    void build(const vector<T> &a) {
        int n = a.size() - 1;
        pre.assign(n + 1, {});
        for (int i = 1; i <= n; i++) {
            // 继承前缀 [0, i - 1] 的线性基
            pre[i] = pre[i - 1];
            T value = a[i];
            int pos = i;
            // 普通异或线性基的高斯消元过程
            for (int bit = Bits - 1; bit >= 0 && value; bit--) {
                if (!(value >> bit & 1)) continue;
                auto &[basisValue, basisPos] = pre[i][bit];
                // 优先保留 pos 更靠右的基向量
                // 这样查询 [l,r] 时可以通过 pos >= l 判断能否使用
                if (basisPos < pos) {
                    swap(basisValue, value);
                    swap(basisPos, pos);
                }
                // 消掉当前最高位,继续向低位消元
                value ^= basisValue;
            }
        }
    }
    // 查询区间 [l, r] 内任意子集异或值与 x 异或后的最大值
    T maxXor(int l, int r, T x = 0) const {
        assert(1 <= l && l <= r && r < pre.size());
        T ans = x;
        // 从高位到低位贪心
        for (int bit = Bits - 1; bit >= 0; bit--) {
            auto [value, pos] = pre[r][bit];
            // position >= left：该基向量可以完全由区间 [left, right] 构造
            // 异或后变大则选择它
            if (pos >= l) {
                ans = max(ans, ans ^ value);
            }
        }
        return ans;
    }
    int size() const {
        return pre.size();
    }
};