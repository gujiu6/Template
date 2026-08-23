/*线性方程与规划

*/
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using ld = long double;
constexpr ld eps = 1e-12;

//2.高斯消元:列主元
optional<pair<bool, vector<ld>>> gaussPivot(vector<vector<ld>> a){
	//Ax = b
    int m = a.size(), n = a[0].size() - 1, rk = 0;
    vector<int> pivot(n, -1);//第 col 个未知数的主元在哪一行
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
            if (i == rk) {
                continue;
            }
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




