#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace std;

// Convencion: esta la funcion foo(const T&), y su amiguita fooPointer(Nodo<T>*), que en general es llamada por la otra (y se puede usar de una perfectamente)
//             Antes las sobrecargaba ambas como foo, pero en c++ un literal int=0 se convierte a puntero null sin aviso ni warning ni nada,
//              y esa posibilidad oculta me parece demasiado peligrosa (ocurrio sutilmente mientras escribia los tests).

template <typename T>
struct Nodo
{
    T dat;
    Nodo *padre;
    Nodo *_h[2]; // 0: hijo izquierdo (los menores). 1: hijo derecho (los mayores)
    int p;

    // Al insertar aqui la propagacion lazy, se hace automaticamente en cualquier recorrida del arbol
    // (si se miran los hijos con h en lugar de _h). Es lindo pero duele en la constante abusar del h() vs _h[i].
    // Como "regla de eficiencia", cuando uno habla varias veces seguidas de los hijos de un nodo sin hacer ningun cambio
    // en el medio, puede usar h() la primera vez, y luego usar _h directamente porque ya fue propagado.
    Nodo * & h(int i)
    {
        dat.lazyPropagation(this);
        return _h[i];
    }
    
    // Va desde nodo hasta la raiz updateando
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
    
    // Rota un nodo X (this) con su padre Z (this->padre)
    // En los Treap, es la *unica* rotacion :D
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
    
    // Cuelga un hijo de este padre.
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
    
    Nodo<T>* treapify(T v[], int N, int levelP, int STEP)
    {
        if (N)
        {
            int rootIndex = N/2;
            Nodo<T>*node = new Nodo<T>(v[rootIndex]);
            node->p = levelP;
            node->_h[0] = treapify(v,rootIndex, levelP - STEP, STEP);
            if (node->_h[0]) node->_h[0]->padre = node;
            node->_h[1] = treapify(v+rootIndex+1,N-rootIndex-1, levelP - STEP, STEP);
            if (node->_h[1]) node->_h[1]->padre = node;
            node->dat.update(node);
            return node;
        }
        else
            return NULL;
    }
    
    // Arma un treap con los N elementos en el orden dado en O(N), vs N lg N que tomaria irlos insertando.
    Treap(T v[], int N)
    {
        int niveles = 0;
        int k=1; while(k <= N) k *= 2, niveles++;
        
        root = treapify(v, N, RAND_MAX, max(1, RAND_MAX / max(1, niveles-1)));
    }
    
    void reroot() { while (root->padre) root = root->padre; }
    
    // Estas 7 funciones que vienen a continuacion muchas veces no se usan.
    // Asumen que los elementos de T tienen comparadores (usa ==, <, >=. etc)
    // Las funciones posteriores no asumen tal cosa (preservan el ordenamiento que los elementos ya tienen en el arbol, asi que no necesitan comparar).
    // Trabajan el Treap como si fuera un set (sin repetidos).
    
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
    
    bool esta(const T&x)
    {
        Nodo<T>* p = insertionPoint(x);
        return p && p->dat == x;
    }
    
    // True si se inserto el nodo. Si ya estaba, el puntero es al nodo que ya estaba (sino, es al nodo nuevo que se acaba de meter).
    pair<Nodo<T>*, bool> insertarPointer(Nodo<T> *nodo)
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
        pair<Nodo<T>*, bool> par = insertarPointer(nuevo);
        if (!par.second) delete nuevo;
        return par;
    }
    
    bool erase(const T&x)
    {
        Nodo<T> *nodo = insertionPoint(x);
        if (nodo && nodo->dat == x)
        {
            erasePointer(nodo);
            return true;
        }
        else
            return false;
    }
    
    // Siempre inserta el nodo, como si fuera el menor / mayor (segun indique lado). No mira los valores.
    void insertarAUnLadoPointer(int lado, Nodo<T> *nodo)
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
        insertarAUnLadoPointer(lado, nuevo);
        return nuevo;
    }
    
    void erasePointer(Nodo<T> *p)
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
    
    void internalSplit(pair<Nodo<T> *, bool> par, Treap &larger)
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
    
    // Toma los datos de this, destruye lo que hubiera en larger
    // Deja los (-inf, x) en this
    // Deja los [x, +inf) en larger
    void splitPointer(Nodo<T> *p, Treap &larger) { internalSplit(make_pair(p, false), larger); }
    void split(const T&x , Treap &larger) { internalSplit(insertar(x), larger); }
    
    // Mergea this con larger (Debe ser *this < larger en todos sus elementos)
    // Vacia larger y deja el resultado en this
    void merge(Treap &larger)
    {
        Nodo<T>*p1 = root, *p2 = larger.root;
        larger.root = NULL;
        root = new Nodo<T>;
        if (p1) root->hang(0, p1);
        if (p2) root->hang(1, p2);
        erasePointer(root);
    }
  
    // Generalmente esto no lo vamos a usar, porque no copiamos Treaps...
    // Pero tener en cuenta que los "default" definitivamente no andan si los usamos.
    // (Generan aliasing, y si tenemos el destructor puede ser particularmente desastroso).
    
    static void copiar(Nodo<T>* parent, Nodo<T>* &destino, Nodo<T>*origen)
    {
        if (origen)
        {
            destino = new Nodo<T>(origen->dat);
            destino->padre = parent;
            destino->p     = origen->p;
            copiar(destino, destino->_h[0], origen->_h[0]);
            copiar(destino, destino->_h[1], origen->_h[1]);
        }
        else
            destino = NULL;
    }
    
    Treap(const Treap &o) {
        copiar(NULL, root, o.root);
    }
    
    Treap &operator =(const Treap &o) {
        liberar(root);
        copiar(NULL, root, o.root);
        return *this;
    }
    
    // Si liberar la memoria nos importa tres carajos
    // (por ejemplo, si usamos un solo Treap grande, y hay un solo caso de prueba en la corrida)
    // esto se puede obviar.
    
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
