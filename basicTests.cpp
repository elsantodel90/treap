#include <map>
#include <set>
#include <vector>
#include <cassert>
#include "treap.h"

using namespace std;

#define forn(i,n) for(int i=0;i<(int)(n);i++)

struct DummyStuff
{
    int value;
    set<int> descendants;
    vector<int> updates;
    vector<int> pendingUpdates;
    DummyStuff(int _value = 0) {value = _value; descendants.insert(_value);}
    void update(Nodo<DummyStuff> *node)
    {
        #define all(c) (c).begin(), (c).end()
        descendants.clear();
        descendants.insert(value);
        forn(i,2) if (node->_h[i]) descendants.insert(all(node->_h[i]->dat.descendants));
    }
    void lazyPropagation(Nodo<DummyStuff> *node)
    {
        forn(i,2)
        if (node->_h[i])
        {
            vector<int> &v = node->_h[i]->dat.updates;
            vector<int> &p = node->_h[i]->dat.pendingUpdates;
            v.insert(v.end(), pendingUpdates.begin(), pendingUpdates.end());
            p.insert(p.end(), pendingUpdates.begin(), pendingUpdates.end());
        }
        pendingUpdates.clear();
    }
    bool operator ==(const DummyStuff &o) const { return value == o.value; }
    bool operator <(const DummyStuff &o) const { return value < o.value; }
    bool operator >(const DummyStuff &o) const { return value > o.value; }
    bool operator >=(const DummyStuff &o) const { return value >= o.value; }
};

typename map<int, Nodo<DummyStuff> * >::const_iterator masUno(typename map<int, Nodo<DummyStuff> * >::const_iterator it)
{
    it++;
    return it;
}

#define meter(x, expected) {pair<Nodo<DummyStuff> * , bool> par = t.insertar(DummyStuff(x)); assert(expected == par.second); pointers[x] = par.first;}
#define check() for(map<int, Nodo<DummyStuff> * >::const_iterator it = pointers.begin(); it != pointers.end(); it++) {checkElem(it->first, it->second, ((masUno(it) != pointers.end()) ? masUno(it)->second: NULL));}

void testLowerBound()
{
    Treap<DummyStuff> t;
    assert(t.lowerBound(5) == NULL);
    map<int, Nodo<DummyStuff> * > pointers;
    #define checkElem(x, p, next) assert(t.lowerBound(x-1) == p); assert(t.lowerBound(x) == p); assert(t.lowerBound(x+1) == next);
    
    meter(10, true); check();
    meter(20, true); check();
    meter(15, true); check();
    meter(17, true); check();
    meter(12, true); check();
    meter(-1, true); check();
    meter(32, true); check();
    meter(34, true); check();
    meter(36, true); check();
    meter(23, true); check();
}

void testUpperBound()
{
    Treap<DummyStuff> t;
    assert(t.upperBound(5) == NULL);
    map<int, Nodo<DummyStuff> * > pointers;
    #undef checkElem
    #define checkElem(x, p, next) assert(t.upperBound(x-1) == p); assert(t.upperBound(x) == next); assert(t.upperBound(x+1) == next);
    
    meter(10, true); check();
    meter(20, true); check();
    meter(15, true); check();
    meter(17, true); check();
    meter(12, true); check();
    meter(-1, true); check();
    meter(32, true); check();
    meter(34, true); check();
    meter(36, true); check();
    meter(23, true); check();
}

void testInsertionPoint()
{
    Treap<DummyStuff> t;
    assert(t.insertionPoint(5) == NULL);
    map<int, Nodo<DummyStuff> * > pointers;
    #define checkInsertionPoint(val) assert(t.insertionPoint(val) != NULL && ((t.insertionPoint(val)->_h[0] == NULL) || (t.insertionPoint(val)->_h[1] == NULL)) &&  (t.insertionPoint(val)->_h[val > t.insertionPoint(val)->dat.value] == NULL));
    #undef checkElem
    #define checkElem(x, p, next) checkInsertionPoint(x-1) ;assert(t.insertionPoint(x) == p); checkInsertionPoint(x+1);
    
    meter(10, true); check();
    meter(20, true); check();
    meter(20, false); check();
    meter(20, false); check();
    meter(20, false); check();
    meter(15, true); check();
    meter(15, false); check();
    meter(17, true); check();
    meter(12, true); check();
    meter(-1, true); check();
    meter(-1, false); check();
    meter(32, true); check();
    meter(34, true); check();
    meter(34, false); check();
    meter(36, true); check();
    meter(23, true); check();
}

void testEsta()
{
    Treap<DummyStuff> t;
    assert(!t.esta(5));
    map<int, Nodo<DummyStuff> * > pointers;
    #undef checkElem
    #define checkElem(x, p, next) assert(!t.esta(x-1)) ; assert(t.esta(x)); assert(!t.esta(x+1));
    
    meter(10, true); check();
    meter(20, true); check();
    meter(20, false); check();
    meter(20, false); check();
    meter(20, false); check();
    meter(15, true); check();
    meter(15, false); check();
    meter(17, true); check();
    meter(12, true); check();
    meter(-1, true); check();
    meter(-1, false); check();
    meter(32, true); check();
    meter(34, true); check();
    meter(34, false); check();
    meter(36, true); check();
    meter(23, true); check();
}

void testErase()
{
    Treap<DummyStuff> t;
    assert(!t.erase(DummyStuff(5)));
    map<int, Nodo<DummyStuff> * > pointers;
    
    meter(10, true); check();
    assert(t.erase(10)); pointers.erase(10);
    meter(20, true); check();
    meter(15, true); check();
    meter(17, true); check();
    assert(t.erase(17)); pointers.erase(17);
    meter(12, true); check();
    assert(!t.erase(17));
    assert(t.erase(12)); pointers.erase(12);
    meter(-1, true); check();
    meter(32, true); check();
    assert(t.erase(32)); pointers.erase(32);
    meter(34, true); check();
    meter(36, true); check();
    assert(t.erase(36)); pointers.erase(36);
    assert(!t.erase(36)); 
    meter(23, true);
    assert(t.erase(23)); pointers.erase(23);
    check();
}

void testSetLikeStuff()
{
    testLowerBound();
    testUpperBound();
    testInsertionPoint();
    testEsta();
    testErase();
}

vector<int> vdfs;

void dfs(Nodo<DummyStuff> *p)
{
    if (p)
    {
        dfs(p->_h[0]);
        vdfs.push_back(p->dat.value);
        dfs(p->_h[1]);
    }
}

void testInsertarAUnLado()
{
    Treap<DummyStuff> t;
    t.insertarAUnLado(0, 20);
    t.insertarAUnLado(0, 10);
    t.insertarAUnLado(1, 30);
    t.insertarAUnLado(1, -1);
    t.insertarAUnLado(1, -2);
    t.insertarAUnLado(1, -2);
    t.insertarAUnLado(1, -2);
    t.insertarAUnLado(1, 3);
    t.insertarAUnLado(1, 4);
    t.insertarAUnLado(0, -5);
    t.insertarAUnLado(1, 10);
    vector<int> v;
    v.push_back(-5);
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.push_back(-1);
    v.push_back(-2);
    v.push_back(-2);
    v.push_back(-2); 
    v.push_back(3); 
    v.push_back(4);
    v.push_back(10);
    dfs(t.root);
    assert(v == vdfs);
}

void testSplitAndMerge()
{
    Treap<DummyStuff> t,t2,t3,t4,t5, t6;
    // Split
    assert(t.insertar(DummyStuff(1)).second);
    assert(t.insertar(DummyStuff(2)).second);
    assert(t.insertar(DummyStuff(0)).second);
    assert(t.insertar(DummyStuff(3)).second);
    assert(t.insertar(DummyStuff(6)).second);
    assert(t.insertar(DummyStuff(4)).second);
    assert(t.insertar(DummyStuff(5)).second);
    t.split(2, t2);
    set<int> s1; s1.insert(0); s1.insert(1);
    set<int> s2; s2.insert(2); s2.insert(3); s2.insert(4); s2.insert(5); s2.insert(6);
    assert(t.root);
    assert(t2.root);
    assert(t.root->dat.descendants == s1);
    assert(t2.root->dat.descendants == s2);
    t.split(DummyStuff(1), t3);
    s1.clear(); s1.insert(0);
    s2.clear(); s2.insert(1);
    assert(t.root);
    assert(t3.root);
    assert(t.root->dat.descendants == s1);
    assert(t3.root->dat.descendants == s2);
    t.split(DummyStuff(1), t4);
    assert(t.root);
    assert(!t4.root);
    assert(t.root->dat.descendants == s1);
    t.split(DummyStuff(0), t5);
    assert(!t.root);
    assert(t5.root);
    assert(t5.root->dat.descendants == s1);
    t2.split(DummyStuff(4), t6);
    s1.clear(); s1.insert(2); s1.insert(3);
    s2.clear(); s2.insert(4); s2.insert(5); s2.insert(6);
    assert(t2.root);
    assert(t6.root);
    assert(t2.root->dat.descendants == s1);
    assert(t6.root->dat.descendants == s2);
    // Lazy stuff
    // t  = []
    // t2 = [2,3]
    // t3 = [1]
    // t4 = []
    // t5 = [0]
    // t6 = [4,5,6]
    #define meterUpdate(t, x) t.root->dat.updates.push_back(x); t.root->dat.pendingUpdates.push_back(x);
    meterUpdate(t2, 0);
    meterUpdate(t3, 1);
    meterUpdate(t5, 2);
    meterUpdate(t6, 3);
    t6.split(5, t);
    meterUpdate(t6, 4);
    meterUpdate(t, 5);
    t.split(6, t4);
    meterUpdate(t4, 6);
    meterUpdate(t, 7);
    // Updates:
    // 0 -> 2
    // 1 -> 1
    // 2 -> 0
    // 3 -> 0
    // 4 -> 3 4
    // 5 -> 3 5 7
    // 6 -> 3 5 6
    // Treaps:
    // t  = [5]
    // t2 = [2,3]
    // t3 = [1]
    // t4 = [6]
    // t5 = [0]
    // t6 = [4]
    // Merge
    t2.merge(t6);
    t5.merge(t3);
    t5.merge(t2);
    t.merge(t4);
    t5.merge(t);
    s1.clear(); s1.insert(0); s1.insert(1); s1.insert(2); s1.insert(3); s1.insert(4); s1.insert(5); s1.insert(6);
    assert(s1 == t5.root->dat.descendants);
    vector<int> upd;
    upd.push_back(0);
    assert(t5.insertionPoint(2)->dat.updates == upd);
    assert(t5.insertionPoint(3)->dat.updates == upd);
    upd[0] = 1;
    assert(t5.insertionPoint(1)->dat.updates == upd);
    upd[0] = 2;
    assert(t5.insertionPoint(0)->dat.updates == upd);
    upd[0] = 3;
    upd.push_back(4);
    assert(t5.insertionPoint(4)->dat.updates == upd);
    upd[1] = 5;
    upd.push_back(7);
    assert(t5.insertionPoint(5)->dat.updates == upd);
    upd[2] = 6;
    assert(t5.insertionPoint(6)->dat.updates == upd);
}

void testTreapCopy()
{
    Treap<DummyStuff> a,b,c;
    b.insertar(1);
    c.insertar(1);
    c.insertar(2);
    c.insertar(3);
    {
        Treap<DummyStuff> d,e,f;
        d = a;
        e = b;
        f = c;
        assert(e.root != b.root);
        assert(f.root != c.root);
        set<int> s;
        assert(!d.root);
        s.insert(1);
        assert(e.root->dat.descendants == s);
        s.insert(2);s.insert(3);
        assert(f.root->dat.descendants == s);
    }
    set<int> s;
    assert(!a.root);
    s.insert(1);
    assert(b.root->dat.descendants == s);
    s.insert(2);s.insert(3);
    assert(c.root->dat.descendants == s);
}

int main()
{
    testSetLikeStuff();
    testInsertarAUnLado();
    testSplitAndMerge();
    testTreapCopy();
    return 0;
}
