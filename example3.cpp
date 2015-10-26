// Solucion para
// http://www.spoj.com/problems/GSS6/

#include <cstdio>
#include <cassert>

#include "treap.h"

using namespace std;

#define forn(i,n) for(int i=0;i<(int)(n);i++)

struct Datos
{
    int x;
    
    int treeSize;
    int bestSum, bestPrefix, bestSuffix, totalSum;
    
    Datos(int _x = 0) {totalSum = bestSum = x = _x; bestPrefix = bestSuffix = max(0, _x);  treeSize = 1;}
    void lazyPropagation(Nodo<Datos> *) { }
    void update(Nodo<Datos> *node)
    {
        // Este codigo tiene copy paste donde originalmente hacia un par de forn(i,2)
        // Sepan disculpar, pero eso fue relevante para que entre en SPOJ, un server altamente estricto.
        int BP[2];
        int BS[2];
        int TSum[2], TSize[2];
        if (node->h(0))
        {
            BP[0] = node->h(0)->dat.bestPrefix;
            BS[0] = node->h(0)->dat.bestSuffix;
            TSum[0] = node->h(0)->dat.totalSum;
            TSize[0] = node->h(0)->dat.treeSize;
        }
        else
            BP[0] = BS[0] = TSum[0] = TSize[0] = 0;
        if (node->h(1))
        {
            BP[1] = node->h(1)->dat.bestPrefix;
            BS[1] = node->h(1)->dat.bestSuffix;
            TSum[1] = node->h(1)->dat.totalSum;
            TSize[1] = node->h(1)->dat.treeSize;
        }
        else
            BP[1] = BS[1] = TSum[1] = TSize[1] = 0;
        
        treeSize = 1 + TSize[0] + TSize[1];
        bestSum = BS[0] + x + BP[1];
        if (node->h(0)) bestSum = max(bestSum, node->h(0)->dat.bestSum);
        if (node->h(1)) bestSum = max(bestSum, node->h(1)->dat.bestSum);
        bestPrefix = max(BP[0], TSum[0] + x + BP[1]);
        bestSuffix = max(BS[1], TSum[1] + x + BS[0]);
        totalSum = TSum[0] + TSum[1] + x;
    }
};

ostream & operator <<(ostream &o, const Datos &d)
{
    return o << "bestSum=" << d.bestSum;
}

#define tam(p) ((p) ? (p)->dat.treeSize : 0)

Nodo<Datos> *iesimo(int i, Treap<Datos> &t)
{
    Nodo<Datos> *p = t.root;
    while (true)
    {
        assert(p);
        int L = tam(p->h(0));
        if (i < L)
            p = p->h(0);
        else if (i == L)
            return p;
        else
        {
            i -= L + 1;
            p = p->h(1);
        }
    }
}

int main()
{
    #ifdef ACMTUYO
        freopen("example3.in","r",stdin);
    #endif
    int N; scanf("%d", &N);
    const int MAXN = 100100;
    static Datos v[MAXN];
    forn(i,N)
    {
        int x;
        scanf("%d", &x);
        v[i] = Datos(x);
    }
    Treap<Datos> t(v, N+1); // Un dummy extra para evitar irnos de rango con iesimo 
    int Q; scanf("%d", &Q);
    forn(q,Q)
    {
        char op[8];
        scanf("%s", op);
        if (op[0] == 'I')
        {
            int x,y;
            scanf("%d%d", &x,&y);
            x--;
            // Esto es parecido (pero bien diferente :P) al codigo de insertarAUnladoPointer, pero empezando en p en lugar de en root
            // De entrada me comi que era hacer lo mismo exacto que insertarAUnLado, y pegue WA.
            // Y luego me comi otro WA por olvidarme la linea p = p->h(0); :P
            // Moraleja: esto seria bastante mas error prone que el split obvio.
            Nodo<Datos> *p = iesimo(x, t), *nodo = new Nodo<Datos>(y);
            int lado = 0;
            if (p->h(0))
            {
                p = p->h(0);
                lado = 1;
                while (p->h(1)) p = p->h(1);
            }
            p->hang(lado, nodo);
            nodo->flotar();
            t.reroot();
        }
        else if (op[0] == 'D')
        {
            int x;
            scanf("%d", &x);
            x--;
            t.erasePointer(iesimo(x, t));
        }
        else if (op[0] == 'R')
        {
            int x,y;
            scanf("%d%d", &x, &y);
            x--;
            Nodo<Datos> *p = iesimo(x, t);
            p->dat.x = y;
            p->fullUpdate();
        }
        else if (op[0] == 'Q')
        {
            int x,y;
            scanf("%d%d", &x,&y);
            x--;
            assert(x < y);
            Treap<Datos> t2,t3;
             t.splitPointer(iesimo(x, t), t2);
            t2.splitPointer(iesimo(y-x, t2), t3);
            printf("%d\n", t2.root->dat.bestSum);
            t.merge(t2);
            t.merge(t3);
        }
        else
            assert(false);
    }
    return 0;
}
