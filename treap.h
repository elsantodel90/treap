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
        Nodo *_h[2];
        int p;

        Nodo * & h(int i)
        {
            // Al insertar aqui la propagacion lazy, se hara automaticamente en cualquier recorrida del arbol (si se miran los hijos con h en lugar de _h).
            dat.lazyPropagation(this);
            return _h[i];
        }
        
        // Va desde nodo hasta la raiz
        void fullUpdate()
        {
            Nodo<T> *nodo = this;
            while (nodo)
            {
                nodo->dat.update(nodo);
                nodo = nodo->padre;
            }
        }
        
        // *****************************************************************************
        // Estas que vienen aca son (deberian ser) las unicas funciones donde se hacen cambios estructurales al arbol

        // Se crea un nuevo nodo sueltito, aislado, raiz de su propio arbol.
        Nodo ()                    { padre = _h[0] = _h[1] = NULL;  p = rand(); }
        Nodo (const T &d) : dat(d) { padre = _h[0] = _h[1] = NULL;  p = rand(); }
        
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
            int childIndex = padre->_h[1] == this;
            Nodo *oldDad = padre;
            
            padre = oldDad->padre;
            if (padre) padre->_h[padre->_h[1] == oldDad] = this;
            
            oldDad->_h[childIndex] = _h[childIndex ^ 1];
            if (oldDad->_h[childIndex]) oldDad->_h[childIndex]->padre = oldDad;
            
            _h[childIndex ^ 1] = oldDad;
            oldDad->padre = this;
        }
        
        // Cuelga un hijo de un padre.
        void hang(int pos, Nodo *child)
        {
            _h[pos] = child;
            child->padre = this;
        }
        
        // Descuelga este hijo de su padre.
        void chop()
        {
            padre->_h[padre->_h[1] == this] = NULL;
            padre = NULL;
        }
        
        // Fin de funciones que hacen cambios estructurales
        // ************************************************************************

        // Flota y actualiza
        void flotar()
        {
            h(0);
            while (padre && p >= padre->p)
            {
                Nodo<T> *aux = padre;
                rotar();
                aux->dat.update(aux);
            } 
            fullUpdate();
        }
        
        void hundir()
        { 
            h(0);
            while ((_h[0] && _h[0]->p > p) || (_h[1] && _h[1]->p > p))
            {
                int childToRotate = _h[0] == NULL || (_h[1] && _h[1]->p > _h[0]->p);
                _h[childToRotate]->h(0);
                _h[childToRotate]->rotar(); 
            }
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
                p = p->h(x > p->dat);
            }
            return res;
        }
        
        Nodo<T>* upperBound(const T&x)
        {
            Nodo<T> *res = NULL, *p = root;
            while (p)
            {
                if (p->dat > x) res = p;
                p = p->h(x >= p->dat);
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
                res = res->h(x > res->dat);
            }
            return prev;
        }
        
        pair<Nodo<T>*, bool> insertar(Nodo<T> *nodo)
        {
            if (!root) return make_pair(root = nodo, true);
            Nodo<T> *p = insertionPoint(nodo->dat);
            if (p->dat == nodo->dat) return make_pair(p, false);
            p->hang(nodo->dat > p->dat, nodo);
            nodo->flotar();
            reroot();
            return make_pair(nodo, true);
        }

        pair<Nodo<T>*, bool> insertar(const T&x)
        {
            Nodo<T> *nuevo = new Nodo<T>(x);
            pair<Nodo<T>*, bool> par = insertar(nuevo);
            if (!par.second) delete nuevo;
            return par;
        }
        
        // Siempre inserta el nodo, no mira los valores.
        void insertarAUnLado(int lado, Nodo<T> *nodo)
        {
            if (!root) root = nodo;
            else
            {
                Nodo<T> *p = root;
                while (p->h(lado)) p = p->_h[lado];
                p->hang(lado, nodo);
                nodo->flotar();
                reroot();
            }
        }
        
        Nodo<T>* insertarAUnLado(int lado, const T&x)
        {
            Nodo<T>* nuevo = new Nodo<T>(x);
            insertarAUnLado(lado, nuevo);
            return nuevo;
        }
        
        void erase(Nodo<T> *p)
        {
            p->p = -1;
            p->hundir();
            Nodo<T> *parent = p->padre;
            if (parent)
            {
                reroot();
                p->chop();
                parent->fullUpdate();
            }
            else
                root = NULL;
            delete p;
        }
        
        bool erase(const T&x)
        {
            Nodo<T> *nodo = insertionPoint(x);
            if (nodo && nodo->dat == x)
            {
                erase(nodo);
                return true;
            }
            else
                return false;
        }
        
        void split(pair<Nodo<T> *, bool> par, Treap &larger)
        {
            Nodo<T>* p = par.first;
            p->p = RAND_MAX;
            p->flotar();
            assert(p->padre == NULL);
            
            liberar(larger.root);
            root = p->h(0);
            if (root) root->chop();
            larger.root = p->_h[1]; // Ya propagamos arriba
            if (par.second)
            {
                if (larger.root) larger.root->chop();
                delete p;
            }
            else
            {
                p->p = rand();
                p->hundir();
                if (!p->padre) larger.root = p;
                p->fullUpdate();
            }
        }
        
        void split(Nodo<T> *p, Treap &larger) { split(make_pair(p, false), larger); }
        
        // Toma los datos de this, destruye lo que hubiera en larger
        // Deja los (-inf, x) en this
        // Deja los [x, +inf) en larger
        void split(const T&x, Treap &larger) { split(insertar(x), larger); }
        
        // Mergea this con larger (Debe ser *this < larger en todos sus elementos)
        // Vacia larger y deja el resultado en this
        void merge(Treap &larger)
        {
            Nodo<T>*p1 = root, *p2 = larger.root;
            larger.root = NULL;
            root = new Nodo<T>;
            if (p1) root->hang(0, p1);
            if (p2) root->hang(1, p2);
            erase(root);
        }
        
        static void liberar(Nodo<T> *p)
        {
            if (p)
            {
                liberar(p->_h[0]);
                liberar(p->_h[1]);
                delete p;
            }
        }
        
        ~Treap() { liberar(root); }

        // Unicamente para debugging

        void printNode(Nodo<T> *p)
        {
            cerr << p << "(padre: " << p->padre << " )" << endl;
            cerr << "Datos:" << endl;
            cerr << p->dat << endl;
            cerr << "Hijos:" << p->_h[0] << " " << p->_h[1] << endl;
            if (p->_h[0]) printNode(p->_h[0]);
            if (p->_h[1]) printNode(p->_h[1]);
        }
        
        void printTreap()
        {
            if (root)
                printNode(root);
            else
                cerr << "<EMPTY TREAP>" << endl;
        }
    };

}
