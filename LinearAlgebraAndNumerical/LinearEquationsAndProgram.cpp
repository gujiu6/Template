/*线性方程与规划
1.高斯消元:唯一解、无解、多解(同余,异或方程组)
2.高斯消元:列主元(普通方程组)
3.高斯消元:bitset优化(异或方程组)
*/
#include <bits/stdc++.h>
#include <cassert>
using namespace std;
using i64 = long long;
using i128 = __int128;
using ld = long double;
constexpr ld eps = 1e-12;

//1.高斯消元:唯一解、无解、多解(同余,异或方程组)
namespace gaussMod {

template <class T = int>
struct ModLinearSol {
	vector<T> particular;//一组特解
	vector<vector<T>> basis;//齐次方程 Ax = 0 的基础解系;size == 0:唯一解
	vector<vector<T>> mat;//高斯消元后的最简矩阵
	vector<int> pivot;
};
template <class T = int>
optional<ModLinearSol<T>> gaussMod(vector<vector<T>> a, i64 mod) {
	//Ax = b (% mod)
	assert(!a.empty() && a[0].size() >= 2);
	int m = a.size(), n = a[0].size() - 1;
	auto norm = [&](T x)->T {
		return ((x % mod) + mod) % mod;
	};
	auto pow = [&](int a, int b)->int {
		int ans = 1;
		while(b) {
			if(b & 1) ans = (i128)ans * a % mod;
			a = (i128)a * a % mod;
			b >>= 1;
		}
		return ans;
	};
	//预先规范化整个矩阵
	for(auto &row : a) {
		assert(row.size() == n + 1);
		for(auto &x : row) {
			x = norm(x);
		}
	}
	vector<int> pivot(n, -1);//第 col 个未知数的主元所在行;-1:x[col] 是自由变量
	int rk = 0;
	for(int col = 0; col < n && rk < m; col++) {
		int row = rk;
		while(row < m && a[row][col] == 0) {
			row++;
		}
		if(row == m) continue;
		swap(a[row], a[rk]);
		int inv = pow(a[rk][col], mod - 2);//求主元逆元
		//主元行乘逆元,使主元变成 1
		for(int j = n; j >= col; j--) {
			a[rk][j] = (i128)a[rk][j] * inv % mod;
		}
		for(int i = 0; i < m; i++) {
			if(i == rk) continue;
			int q = a[i][col];
			if(q == 0) continue;
			for(int j = col; j <= n; j++) {
				a[i][j] = norm(a[i][j] - (i128)q * a[rk][j] % mod);
			}
		}
		pivot[col] = rk++;
	}
	//无解(0 == 常数)
	for(int i = rk; i < m; i++) {
		if(a[i][n]) return nullopt;
	}
	ModLinearSol<T> sol;
	sol.particular.assign(n, 0);//初始化特解
	for(int col = 0; col < n; col++) {
		if(pivot[col] != -1) {
			sol.particular[col] = a[pivot[col]][n];
		}
	}
	for(int free = 0; free < n; free++) {
		if(pivot[free] != -1) continue;
		vector<T> v(n);
		v[free] = 1;
		for(int col = 0; col < n; col++) {
			if(pivot[col] == -1) continue;
			int row = pivot[col];
			v[col] = (mod - a[row][free]) % mod;
		}
		sol.basis.emplace_back(move(v));
	}
	sol.mat = move(a);
	sol.pivot = move(pivot);
	return sol;
}

}


//2.高斯消元:列主元(普通方程组)
optional<pair<bool, vector<ld>>> gaussPivot(vector<vector<ld>> a){
	//Ax = b
    int m = a.size(), n = a[0].size() - 1, rk = 0;
    vector<int> pivot(n, -1);//第 col 个未知数的主元所在行;-1:x[col] 是自由变量
    for (int col = 0; col < n && rk < m; col++) {
		//部分主元选取
        int row = rk;
        for (int i = rk + 1; i < m; i++) {
            if (fabsl(a[i][col]) > fabsl(a[row][col])) {
                row = i;
            }
        }
        if (fabsl(a[row][col]) <= eps) {
            continue;
        }
        swap(a[row], a[rk]);
		//主元行归一化
        for (int j = n; j >= col; j--) {
            a[rk][j] /= a[rk][col];
        }
		//用主元消掉其他所有行
        for (int i = 0; i < m; i++) {
            if (i == rk) continue;
            ld q = a[i][col];
            for (int j = col; j <= n; j++) {
                a[i][j] -= q * a[rk][j];
            }
        }
		//记录主元并增加秩
        pivot[col] = rk++;
    }
	//无解(0 == 常数)
    for (int i = rk; i < m; i++) {
        if (fabsl(a[i][n]) > eps) {
            return nullopt;
        }
    }
	vector<ld> x(n);
	for (int col = 0; col < n; col++){
		if(pivot[col] != -1) {
			x[col] = a[pivot[col]][n];
		}
    }
	if(rk == n) return {{true, x}};//唯一解
    return {{false, x}};//无穷多解,返回一组特解
}

//3.高斯消元:bitset优化(异或方程组)
namespace gaussXor {

template <int MAXN>
struct XorLinearSol {
	vector<int> particular;
	vector<vector<int>> basis;
	vector<bitset<MAXN + 1>> mat;
	vector<int> pivot;
};
template <int MAXN>
optional<XorLinearSol<MAXN>> gaussXor(vector<bitset<MAXN>> a, int n) {
	int m = a.size(), rk = 0;
	vector<int> pivot(n, -1);
	for(int col = 0; col < n && rk < m; col++) {
		int row = rk;
		while(row < m && !a[row][col]) {
			row++;
		}
		if(row == m) continue;
		swap(a[rk], a[row]);
		for(int i = 0; i < m; i++) {
			if(i != rk && a[i][col]) {
				a[i] ^= a[rk];
			}
		}
		pivot[col] = rk++;
	}
	for(int i = rk; i < m; i++) {
		if(a[i][n]) return nullopt;
	}
	XorLinearSol<MAXN> sol;
	sol.particular.assign(n, 0);
	for(int col = 0; col < n; col++) {
		if(pivot[col] != -1) {
			sol.particular[col] = a[pivot[col]][n];
		}
	}
	for(int free = 0; free < n; free++) {
		if(pivot[free] != -1) continue;
		vector<int> v(n, 0);
		v[free] = 1;
		for(int col = 0; col < n; col++) {
			if(pivot[col] == -1) continue;
			int row = pivot[col];
			v[col] = a[row][free];
		}
		sol.basis.emplace_back(move(v));
	}
	sol.mat = move(a);
	sol.pivot = move(pivot);
	return sol;
}
}

