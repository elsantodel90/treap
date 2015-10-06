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
        
        // Cuelga un hijo de un padre. El hijo antes era una raiz que no tenia padre.
        void hang(int pos, Nodo *child)
        {
            h[pos] = child;
            child->padre = this;
        }
        
        // Fin de funciones que hacen cambios estructurales
        // ************************************************************************

        void flotar() { while (padre && p < padre->p) rotar(); }
    };

    template <typename T>
    struct Treap
    {
        Nodo<T> *root;        
        Treap() : root(NULL) {}
                
        Nodo<T>* lowerBound(const T&x)
        {
            Nodo<T> *res = NULL, *p = root;
            while (p)
            {
                if (p->dat == x) return p;
                else if (p->dat >= x) res = p;
                p = p->h[x > p->dat];
            }
            return res;
        }
        
        Nodo<T>* upperBound(const T&x)
        {
            Nodo<T> *res = NULL, *p = root;
            while (p)
            {
                if (p->data > x) res = p;
                p = p->h[x >= p->dat];
            }
            return res;
        }
                
        bool pertenece(const T& x)
        {
            Nodo<T> *p = lowerBound(x);
            return p && p->dat == x; 
        }

        Nodo<T> *insertar(const T&x)
        {
            if (!root)
            {
                root = new Nodo<T>(x);
                return root;
            }
            Nodo<T> *p = lowerBound(x);
            if (p && p->dat == x) return p; // Ya estaba
            Nodo<T> *nu = new Nodo<T>(x);
            int childIndex = 1;
            if      (p == NULL)    p = root;
            else if (p->h[0]  )    p = p->h[0];
            else                   childIndex = 0;
            while (p->h[childIndex]) p = p->h[1];
            p->hang(childIndex, nu);
            nu->flotar();
            if (nu->padre == NULL) root = nu;
            return nu;
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


int main()
{
    Treap::Treap<int> t;
    t.insertar(1);
    t.insertar(2);
    cout << "SSSSSS" << endl;
    cout << t.pertenece(3) << endl;
    return 0;
}
