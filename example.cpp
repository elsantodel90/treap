// Solution to:
// http://codeforces.com/gym/100796/problem/J

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <vector>


#include "treap.h"

using namespace std;

#define forn(i,n) for(int i = 0; i < (int)(n);i++)

struct Datos
{
    int cantidad; // Tamanio subarbol
    Datos() { cantidad = 1; }
    void lazyPropagation(Treap::Nodo<Datos>*) {}
    void update(Treap::Nodo<Datos>*node)
    {
        cantidad = 1;
        forn(i,2) if (node->h(i)) cantidad += node->h(i)->dat.cantidad;
    }
};


ostream & operator << (ostream &o, const Datos &d)
{
    return o << "cantidad = " << d.cantidad;
}

int main()
{
    #ifdef ACMTUYO
        freopen("example.in","r",stdin);
    #endif
    int N; scanf("%d", &N);
    Treap::Treap<Datos> t;
    vector<Treap::Nodo<Datos> *> v;
    int total = 0;
    forn(stopNumber,N)
    {
        char comando[8];
        scanf("%s",comando);
        if (strcmp(comando,"F") == 0)
        {
            v.push_back(t.insertarAUnLado(0, Datos()));
            total++;
        }
        else if (strcmp(comando,"B") == 0)
        {
            total++;
            v.push_back(t.insertarAUnLado(1, Datos()));
        }
        else
        {
            int i;
            scanf("%d",&i);
            i--;
            Treap::Treap<Datos> t2;
            t.split(v[i], t2);
            int alFrente  = t.root ? t.root->dat.cantidad : 0;
            int porDetras = total - 1 - alFrente;
            printf("%d\n", min(alFrente, porDetras));
            t2.erase(v[i]);
            t2.merge(t);
            swap(t2.root, t.root);
            total--;
        }
    }
    return 0;
}
