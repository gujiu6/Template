/*
1.带删堆
2.Top‑K和维护
*/
#include <bits/stdc++.h>
#include <cassert>
using namespace std;



//1.带删堆
template <class T = int, class Cmp = less<T>> 
class ErasableHeap {
public:
    priority_queue<T, vector<T>, Cmp> a, b;
    int n;
    void clear() {
        while(!a.empty() && !b.empty() && a.top() == b.top()) {
            a.pop();
            b.pop();
        }
    }
    void push(const T& x) {
        a.push(x);
        n++;
    }
    void erase(const T& x) {
        b.push(x);
        n--;
        clear();
    }
    const T &top() {
        clear();
        return a.top();
    }
    void pop() {
        clear();
        a.pop();
        n--;
        clear();
    }
    int size() const {
        return n;
    }
    bool empty() const {
        return n == 0;
    }
};

//2.Top‑K和维护
template <class T = int>
class TopK {
public:
    int k;
    T sum{};
    multiset<T> hi, lo;
    TopK (int k): k(k) {
        assert(k >= 0);
    }
    void fix() {
        while(hi.size() > k) {
            auto it = hi.begin();
            sum -= *it;
            lo.insert(*it);
            hi.erase(it);
        }
        while(hi.size() < k && !lo.empty()) {
            auto it = prev(lo.end());
            sum += *it;
            hi.insert(*it);
            lo.erase(it);
        }
    }
    void insert(T x) {
        if(!hi.empty() && x >= *hi.begin()) {
            hi.insert(x);
            sum += x;
        }
        else {
            lo.insert(x);
        }
        fix();
    }
    bool erase(T x) {
        auto it = hi.find(x);
        if(it != hi.end()) {
            sum -= x;
            hi.erase(it);
            fix();
            return true;
        }
        it = lo.find(x);
        if(it == lo.end()) return false;
        lo.erase(it);
        return true;
    }
    //最大的 k 个元素之和,第 k 大元素
    optional<pair<T, T>> qry() const {
        if(hi.size() < k) return nullopt;
        return {sum, *hi.begin()};
    }
};



