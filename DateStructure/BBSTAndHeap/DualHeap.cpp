/*
1.带删堆
2.Top‑K和维护
*/
#include <bits/stdc++.h>
#include <cassert>
using namespace std;



template <typename T=int>
class DualHeap{
private:
    priority_queue<T>lo;
    priority_queue<T,vector<T>,greater<T>>hi;
    unordered_map<T,int>dead;
    int _k=0,sz=0,lo_sz=0,hi_sz=0;
    void clean(){
        while(!lo.empty()&&dead[lo.top()]>0){
            dead[lo.top()]--;
            lo.pop();
        }
        while(!hi.empty()&&dead[hi.top()]>0){
            dead[hi.top()]--;
            hi.pop();
        }
    }
    void balance(){
        clean();
        while(lo_sz<_k&&!hi.empty()){lo.push(hi.top());hi.pop();lo_sz++,hi_sz--;}
        while(lo_sz>_k){hi.push(lo.top());lo.pop();lo_sz--;hi_sz++;}
        clean();
    }
public:
    DualHeap()=default;
    DualHeap(int k):_k(k){}
    void insert(T x){
        if(!lo.empty()&&x<=lo.top())lo.push(x),lo_sz++;
        else hi.push(x),hi_sz++;
        sz++;
        balance();
    }
    void erase(T x){
        dead[x]++;
        sz--;
        if(!lo.empty()&&x<=lo.top()) lo_sz--;
        else hi_sz--;
        balance();
    }
    void set_k(int k){_k=k;balance();}
    T qry() {clean();return lo.top();}
    int size() const{return sz;}
};

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
    optional<T> qry() const {
        if(hi.size() < k) return nullopt;
        return sum;
    }
};



