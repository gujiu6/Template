/*线段树家族
1.单点修改
2.懒标记
3.仿射懒更新
4.区间最值与历史最值
*/
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
const i64 INF = 1e18;


//1.单点修改线段树
namespace Seg {

struct Info {
    i64 val = 0;
    Info() = default;
    Info(i64 x) {
        val = x;
    }
    friend Info operator+ (const Info& a, const Info& b) {
        return Info(a.val + b.val);
    }
};
template <class Info = Info>
class Seg {
public:
    int n;
    vector<Info> tr;
    Seg(int n = 0): n(n) {
        tr.assign((n << 1) + 2, Info{});
    }
    Seg(const vector<Info>& a): n(a.size() - 1){
        tr.assign((n << 1) + 2, Info{});
        for(int i = 1; i <= n; i++) {
            tr[n + i - 1] = a[i];
        }
        for(int i = n - 1; i >= 1; i--) {
            pull(i);
        }
    }
private:
    void pull(int p) {
        tr[p] = tr[p << 1] + tr[p << 1 | 1];
    }
public:
    void set(int p, Info v) {
        p = p + n - 1;
        tr[p] = v;
        for(p >>= 1; p; p >>= 1) {
            pull(p);
        }
    }
    void add(int p, Info v) {
        p = p + n - 1;
        tr[p] = tr[p] + v;
        for(p >>= 1; p; p >>= 1) {
            pull(p);
        }
    }
    // 查询闭区间 [l, r]
    Info qry(int l, int r) const {
        Info x{}, y{};
        l = l + n - 1;
        r = r + n;
        while(l < r) {
            if(l & 1) {
                x = x + tr[l++];
            }
            if(r & 1) {
                y = tr[--r] + y ;
            }
            l >>= 1;
            r >>= 1;
        }
        return x + y;
    }
};

}

//2.懒标记线段树
namespace LazySeg{

struct Tag {
    bool has_set = false;
    i64 set = 0;
    i64 add = 0;
    // 先执行当前 Tag，再执行 t
    void apply(const Tag& t) {
        if(t.has_set) {
            has_set = true;
            set = t.set;
            add = t.add;
        }
        else {
            add += t.add;
        }
    }
};
struct Info {
    i64 sum = 0;
    int len = 0;
    Info() = default;
    Info(i64 x) {
        sum = x;
        len = 1;
    }
    void apply(const Tag& t) {
        if(t.has_set) {
            sum = t.set * len;
        }
        sum += t.add * len;
    }
    friend Info operator+ (const Info& a, const Info& b) {
        Info c;
        c.sum = a.sum + b.sum;
        c.len = a.len + b.len;
        return c;
    }
};
template <class Info = Info, class Tag = Tag>
class LazySeg {
public:
    int n;
    vector<Info> tr;
    vector<Tag> tag;
    LazySeg(int n = 0):n(n), tr((n << 2) + 4, Info{}), tag((n << 2) + 4, Tag{}) {
        if(n) build(1, 1, n);
    }
    LazySeg(const vector<Info>& a): n(a.size() - 1), tr((n << 2) + 4, Info{}), tag((n << 2) + 4, Tag{}) {
        if(n) build(1, 1, n, a);
    }
private:
    void build(int p, int l, int r) {
        tr[p].len = r - l + 1;
        tr[p].sum = 0;
        if (l == r) return;
        int mid = (l + r) >> 1;
        build(p << 1, l, mid);
        build(p << 1 | 1, mid + 1, r);
    }
    void build(int p, int l, int r, const vector<Info>& a) {
        if(l == r) {
            tr[p] = a[l];
            return ;
        }
        int mid = (l + r) >> 1;
        build(p << 1, l, mid, a);
        build(p << 1 | 1, mid + 1, r, a);
        pull(p);
    }
    void pull(int p) {
        tr[p] = tr[p << 1] + tr[p << 1 | 1];
    }
    void apply(int p, const Tag& t) {
        tr[p].apply(t);
        tag[p].apply(t);
    }
    void push(int p) {
        apply(p << 1, tag[p]);
        apply(p << 1 | 1, tag[p]);
        tag[p] = Tag{};
    }
    void modify(int p, int l, int r, int ql, int qr, const Tag& t) {
        if(ql <= l && r <= qr) {
            apply(p, t);
            return;
        }
        push(p);
        int mid = (l + r) >> 1;
        if(ql <= mid) modify(p << 1, l, mid, ql, qr, t);
        if(qr > mid) modify(p << 1 | 1, mid + 1, r, ql, qr, t);
        pull(p);
    }
    Info qry(int p, int l, int r, int ql, int qr) {
        if(ql <= l && r <= qr) return tr[p];
        push(p);
        int mid = (l + r) >> 1;
        if(qr <= mid) return qry(p << 1, l, mid, ql, qr);
        if(ql > mid) return qry(p << 1 | 1, mid + 1, r, ql, qr);
        return qry(p << 1, l, mid, ql, qr) + qry(p << 1 | 1, mid + 1, r, ql, qr);
    }
public:
    // 区间加 x
    void add(int l, int r, i64 x) {
        modify(1, 1, n, l, r, Tag{.has_set = false, .set = 0, .add = x});
    }
    // 区间赋值 x
    void set(int l, int r, i64 x) {
        modify(1, 1, n, l, r, Tag{.has_set = true, .set = x, .add = 0});
    }
    // 区间信息
    Info qry(int l, int r) {
        return qry(1, 1, n, l, r);
    }
};

}

//3.仿射懒更新(区间加、乘、赋值、异或、取模)
namespace AffineSeg{
struct AffineTag {
    i64 mul = 1;
    i64 add = 0;
    void apply(const AffineTag& t) {
        mul *= t.mul;
        add = add * t.mul + t.add;
    }
};
struct AffineInfo {
    i64 sum = 0;
    int len = 0;
    i64 mx = -INF;
    AffineInfo() = default;
    AffineInfo(i64 x) {
        sum = x;
        mx = x;
        len = 1;
    }
    void apply(const AffineTag& t) {
        sum = sum * t.mul + t.add * len;
        mx = mx * t.mul + t.add;
    }
    friend AffineInfo operator+ (const AffineInfo& a, const AffineInfo& b) {
        AffineInfo c;
        c.sum = a.sum + b.sum;
        c.len = a.len + b.len;
        c.mx = max(a.mx, b.mx);
        return c;
    }
};
template <class Info = AffineInfo, class Tag = AffineTag>
class AffineSeg {
public:
    int n;
    vector<Info> tr;
    vector<Tag> tag;
    AffineSeg(int n = 0):n(n), tr((n << 2) + 4, Info{}), tag((n << 2) + 4, Tag{}) {
        if(n) build(1, 1, n);
    }
    AffineSeg(const vector<Info>& a): n(a.size() - 1), tr((n << 2) + 4, Info{}), tag((n << 2) + 4, Tag{}) {
        if(n) build(1, 1, n, a);
    }
private:
    void build(int p, int l, int r) {
        tr[p].len = r - l + 1;
        tr[p].sum = 0;
        tr[p].mx = 0;
        if (l == r) return;
        int mid = (l + r) >> 1;
        build(p << 1, l, mid);
        build(p << 1 | 1, mid + 1, r);
    }
    void build(int p, int l, int r, const vector<Info>& a) {
        if(l == r) {
            tr[p] = a[l];
            return ;
        }
        int mid = (l + r) >> 1;
        build(p << 1, l, mid, a);
        build(p << 1 | 1, mid + 1, r, a);
        pull(p);
    }
    void pull(int p) {
        tr[p] = tr[p << 1] + tr[p << 1 | 1];
    }
    void apply(int p, const Tag& t) {
        tr[p].apply(t);
        tag[p].apply(t);
    }
    void push(int p) {
        apply(p << 1, tag[p]);
        apply(p << 1 | 1, tag[p]);
        tag[p] = Tag{};
    }
    void modify(int p, int l, int r, int ql, int qr, const Tag& t) {
        if(ql <= l && r <= qr) {
            apply(p, t);
            return;
        }
        push(p);
        int mid = (l + r) >> 1;
        if(ql <= mid) modify(p << 1, l, mid, ql, qr, t);
        if(qr > mid) modify(p << 1 | 1, mid + 1, r, ql, qr, t);
        pull(p);
    }
    Info qry(int p, int l, int r, int ql, int qr) {
        if(ql <= l && r <= qr) return tr[p];
        push(p);
        int mid = (l + r) >> 1;
        if(qr <= mid) return qry(p << 1, l, mid, ql, qr);
        if(ql > mid) return qry(p << 1 | 1, mid + 1, r, ql, qr);
        return qry(p << 1, l, mid, ql, qr) + qry(p << 1 | 1, mid + 1, r, ql, qr);
    }
    void mod(int p, int l, int r, int ql, int qr, i64 x) {
        if (r < ql || l > qr || tr[p].mx < x) return;
        if (l == r) {
            tr[p].sum %= x;
            tr[p].mx = tr[p].sum;
            return;
        }
        push(p);
        int mid = (l + r) >> 1;
        mod(p << 1, l, mid, ql, qr, x);
        mod(p << 1 | 1, mid + 1, r, ql, qr, x);
        pull(p);
    }
public:
    // 通用仿射:x -> mul * x + add
    void affine(int l, int r, i64 mul, i64 add) {
        modify(1, 1, n, l, r, Tag{mul, add});
    }
    // 区间加:x -> x + x0
    void add(int l, int r, i64 x) {
        affine(l, r, 1, x);
    }
    // 区间乘:x -> x * x0
    void mul(int l, int r, i64 x) {
        affine(l, r, x, 0);
    }
    // 区间赋值:x -> x0
    void set(int l, int r, i64 x) {
        affine(l, r, 0, x);
    }
    // 区间取模:x -> x % x0
    void mod(int l, int r, i64 x) {
        mod(1, 1, n, l, r, x);
    }
    // 区间信息
    Info qry(int l, int r) {
        return qry(1, 1, n, l, r);
    }
};

}

//4.区间最值与历史最值(吉司机线段树)
