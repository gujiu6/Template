/*矩阵
1.普通矩阵与取模矩阵
2.矩阵快速幂
*/
#include <bits/stdc++.h>
#include <cassert>
using namespace std;
using i64 = long long;
using u64 = unsigned long long;
const int MOD = 1e9+7;



//1.普通矩阵与取模矩阵
template <class T = int>
class Mat{
public:
    int n, m;//矩阵:n * m
    vector<vector<T>> a;
    Mat(int n = 0, int m = 0) : n(n), m(m), a(n, vector<T>(m)) {}
    //n阶单位矩阵
    static Mat eye(int n){
        Mat a(n, n);
        for (int i = 0; i < n; i++) {
            a[i][i] = 1;
        }
        return a;
    }
    vector<T> &operator[](int x) {
        return a[x];
    }
    const vector<T> &operator[](int x) const {
        return a[x];
    }
    friend Mat operator*(const Mat &x, const Mat &y) {
        //x:乘法左侧矩阵;y:乘法右侧矩阵;返回两个维数相容矩阵的普通乘积
        assert(x.m == y.n);
        Mat z(x.n, y.m);
        for (int i = 0; i < x.n; i++){
            for (int k = 0; k < x.m; k++){
                for (int j = 0; j < y.m; j++){
                    z[i][j] = (z[i][j] + 1LL * x[i][k] * y[k][j]);
                }
            }
        }
        return z;
    }
    Mat pow(unsigned long long b) const {
        //b:表示方阵的非负整数指数;返回当前方阵的b次幂
        assert(n == m);
        Mat x = *this;
        Mat ans = eye(n);
        while(b) {
            if(b & 1) ans = ans * x;
            b >>= 1;
            x = x * x;
        }
        return ans;
    }
};

//2.矩阵快速幂
template <class T = int>
vector<vector<T>> matPow(vector<vector<T>> A, u64 p, const vector<vector<T>> &f){
    //返回 A^p * f, A:n*n, f:初始矩阵,行数n
    int n = A.size();
    auto ans = f;
    auto mul = [&](const vector<vector<T>> &x, const vector<vector<T>> &y){
        //x、y是同阶方阵;返回普通矩阵乘积
        vector<vector<T>> z(n, vector<T>(n));
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < n; k++) {
                for (int j = 0; j < n; j++) {
                    z[i][j] = (z[i][j] + 1LL * x[i][k] * y[k][j]);
                }
            }
        }
        return z;
    };
    while(p > 0) {
        if(p & 1) ans = mul(A, ans);
        A = mul(A, A);
        p >>= 1;
    }
    return ans;
}
