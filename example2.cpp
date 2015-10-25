// Solution to:
// http://www.spoj.com/problems/TAP2014K/

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
    
    char caracter; // Reducido modulo 26 ya, 0 <= caracter < 26
    
    bool invertir; // Se propaga lazy
    int sum;       // Se propaga lazy
    Datos() { cantidad = 1; sum = 0; invertir = false;}
    void lazyPropagation(Treap::Nodo<Datos>*node)
    {
        if (invertir)
        {
            swap(node->_h[0],node->_h[1]); // Es importante no usar node->h() en esta funcion, porque dispararia el lazy y seria auto-llamada recursiva.
            forn(i,2) if (node->_h[i]) node->_h[i]->dat.invertir ^= 1;
            invertir = false;
        }
        forn(i,2) if (node->_h[i]) node->_h[i]->dat.sum += sum;
        caracter = (caracter + sum) % 26;
        sum = 0;
    }
    void update(Treap::Nodo<Datos>*node)
    {
        cantidad = 1;
        forn(i,2) if (node->_h[i]) cantidad += node->_h[i]->dat.cantidad;
    }
};

char cadena[1000000];

// Buen ejemplo de lo simple que es recorrer el arbol, y las operaciones lazy se hacen solas mientras viajamos si usamos ->h()
// Igualmente es mas eficiente usar _h[] cuando sabemos que no hay que propagar (nos ahorramos el propagar).
// Es una constante, pero en SPOJ fue la diferencia entre entrar y no entrar (no parece ser mas que un *3 la diferencia en mediciones locales en mi compu).
Treap::Nodo<Datos> *iesimo(int i, Treap::Nodo<Datos> *raiz)
{
    assert(raiz);
    while (true)
    {
        raiz->h(0);
        const int leftSize = (raiz->_h[0]) ?  (raiz->_h[0]->dat.cantidad) : 0;
        if (i < leftSize)
            raiz = raiz->_h[0];
        else if (i == leftSize)
            return raiz;
        else
        {
            i -= leftSize + 1;
            raiz = raiz->_h[1];
        }
    }
}

void imprimirCadenaFinal(Treap::Nodo<Datos> *node)
{
    if (node)
    {
        imprimirCadenaFinal(node->h(0));
        printf("%c", 'a' + (node->dat.caracter + node->dat.sum) % 26);
        imprimirCadenaFinal(node->h(1));
    }
}

int main() // Notar que este problema es buena onda y no pone strings vacios y esas cosas. El Trie tiene t.root == NULL cuando es vacio, asi que
           // nos estamos ahorrando chequeos de NULL gracias a eso. Pero si no tendriamos que chequearlo.
{
    #ifdef ACMTUYO
        freopen("example2.in","r",stdin);
    #endif
    int TT; scanf("%d", &TT);
    forn(tt,TT)
    {
        scanf("%s", cadena);
        int cadenaLength = strlen(cadena);
        Treap::Treap<Datos> t;
        forn(i,cadenaLength)
        {
            Datos d;
            d.caracter = cadena[i] - 'a';
            t.insertarAUnLado(1, d);
        }
        t.insertarAUnLado(1, Datos()); // Asi tenemos un nodo concreto que juegue de frontera derecha siempre.
        
        int N; scanf("%d", &N);
        forn(i,N)
        {
            int I,J,K,L;
            scanf("%d%d%d%d",&I, &J, &K, &L);
            I--; K--;
            Treap::Nodo<Datos> *iNode = iesimo(I, t.root);
            Treap::Nodo<Datos> *jNode = iesimo(J, t.root);
            Treap::Nodo<Datos> *kNode = iesimo(K, t.root);
            Treap::Nodo<Datos> *lNode = iesimo(L, t.root);
            Treap::Treap<Datos> t2,t3,t4, t5; // Los 5 cachos del string inducidos por nuestros cachos disjuntos [I,J) y [K,L)
            t.split(iNode, t2);
            t2.split(jNode, t3);
            t3.split(kNode, t4);
            t4.split(lNode, t5);
            // Cada cacho es invertido y aumentado individualmente
            t2.root->dat.invertir ^= 1;
            t2.root->dat.sum++;
            
            t4.root->dat.invertir ^= 1;
            t4.root->dat.sum++;

            t.merge(t4); // Aca damos vuelta los cachos entre si
            t.merge(t3);
            t.merge(t2);
            t.merge(t5);
        }
        // Sacamos el extra que habiamos puesto antes asi no lo imprimimos
        t.erase(iesimo(cadenaLength, t.root));
        imprimirCadenaFinal(t.root);
        printf("\n");
    }
    return 0;
}
