#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace std;

namespace Treap
{

    template <typename T>
    struct Nodo
    {
        T dat;
        Nodo *padre;
        Nodo *h[2];
        int p;
        
        // *****************************************************************************
        // Estas que vienen aca son las unicas funciones donde se hacen cambios estructurales al arbol, y las que hay que tocar si se aumenta con mas datos.
        // Normalmente solo hay que agregar un "update" de los nodos cuyos subarboles han cambiando (X y Z en una rotacion por ejemplo)
        
        // Se crea un nuevo nodo sueltito, aislado, raiz de su propio arbol.
        Nodo (const T &d) { dat = d; padre = h[0] = h[1] = NULL;  p = rand(); }
        
        // Rota un nodo X con su padre Z
        // Todos los nodos en el dibujo pueden ser NULL salvo X y Z.
        /*
           Si childIndex == 0:
        
                 P                 P
                 |                 |
                 Z                 X
                / \       --->    / \
               X   Q             Y   Z
              / \                   / \
             Y   W                 W   Q

           Si childIndex == 1:
                 P                 P
                 |                 |
                 Z                 X
                / \       --->    / \
               Q   X             Z   W
                  / \           / \   
                 Y   W         Q   Y  
        */
        void rotar()
        {
            assert(padre);
            int childIndex = padre->h[1] == this;
            Nodo *oldDad = padre;
            
            padre = oldDad->padre;
            if (padre) padre->h[padre->h[1] == oldDad] = this;
            
            oldDad->h[childIndex] = h[childIndex ^ 1];
            if (oldDad->h[childIndex]) oldDad->h[childIndex]->padre = oldDad;
            
            h[childIndex ^ 1] = oldDad;
            oldDad->padre = this;
        }
        
        // Cuelga un hijo de un padre.
        void hang(int pos, Nodo *child)
        {
            h[pos] = child;
            child->padre = this;
        }
        
        // Descuelga este hijo de su padre.
        void chop()
        {
            padre->h[padre->h[1] == this] = NULL;
            padre = NULL;
        }
        
        // Fin de funciones que hacen cambios estructurales
        // ************************************************************************

        void flotar() { while (padre && p >= padre->p) rotar(); }
        
        void hundir()
        { 
            while ((h[0] && h[0]->p > p) || (h[1] && h[1]->p > p))
                h[h[0] == NULL || (h[1] && h[1]->p > h[0]->p)].rotar(); 
        }
    };

    template <typename T>
    struct Treap
    {
        Nodo<T> *root;        
        Treap() : root(NULL) {}
        
        void reroot() { while (root->padre) root = root->padre; }
        
        Nodo<T>* lowerBound(const T&x)
        {
            Nodo<T> *res = NULL, *p = root;
            while (p)
            {
                if (p->dat >= x) res = p;
                p = p->h[x > p->dat];
            }
            return res;
        }
        
        Nodo<T>* upperBound(const T&x)
        {
            Nodo<T> *res = NULL, *p = root;
            while (p)
            {
                if (p->dat > x) res = p;
                p = p->h[x >= p->dat];
            }
            return res;
        }
        
        Nodo<T>* insertionPoint(const T&x)
        {
            Nodo<T>* res = root, *prev = NULL;
            while (res)
            {
                prev = res;
                if (res->dat == x) return res;
                res = res->h[x > res->dat];
            }
            return prev;
        }
                
        pair<Nodo<T>*, bool> insertar(const T&x)
        {
            if (!root) return make_pair(root = new Nodo<T>(x), true);
            Nodo<T> *p = insertionPoint(x);
            if (p->dat == x) return make_pair(p, false);
            Nodo<T> *nu = new Nodo<T>(x);
            p->hang(x > p->dat, nu);
            nu->flotar();
            reroot();
            return make_pair(nu, true);
        }
        
        void erase(Nodo<T> *p)
        {
            p->p = -1;
            p->hundir();
            if (p->padre)
            {
                reroot();
                p->chop();
            }
            else
                root = NULL;
            delete p;
        }
        
        // Toma los datos de this, destruye lo que hubiera en larger
        // Deja los (-inf, x) en this
        // Deja los [x, +inf) en larger
        void split(const T&x, Treap &larger)
        {
            pair<Nodo<T> *, bool> par = insertar(x);
            Nodo<T>* p = par.first;
            p->p = RAND_MAX;
            p->flotar();
            assert(p->padre == NULL);
            
            liberar(larger.root);
            larger.root = p->h[1];
            root = p->h[0];
            delete p;
            if (!par.second)
                larger.insertar(x);
        }
        
        // Mergea this con larger (Debe ser this < larger en todos sus elementos)
        // Vacia larger y deja el resultado en this
        void merge(Treap &larger)
        {
            Nodo<T>*p1 = root, *p2 = larger.root;
            larger.root = NULL;
            root = new Nodo<T>;
            root->h[0] = p1;
            root->h[1] = p2;
            erase(root);
        }
        
        static void liberar(Nodo<T> *p)
        {
            if (p)
            {
                liberar(p->h[0]);
                liberar(p->h[1]);
                delete p;
            }
        }
        
        ~Treap() { liberar(root);}
    };

}

#define pertenece(t, x) (t.lowerBound(x) && t.lowerBound(x)->dat == x)

int main()
{
    Treap::Treap<int> t;
    t.insertar(1);
    t.insertar(2);
    cout << "SSSSSS" << endl;
    cout << pertenece(t,0) << endl;
    return 0;
}
