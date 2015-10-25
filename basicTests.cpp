#include "treap.h"

// Si vamos a guardar ints y no hacer nada extra, lo mas practico no es implementar estas cosas que no hacen nada,
// sino borrar las llamadas en el codigo del Treap (muy facil de hacer en competencia).
// Pero esto es ejemplo, y para justamente no modificar el codigo implementacion.
struct DummyInt
{
    int x;
    DummyInt(int xx) {x = xx;}
    void update(Treap::Nodo<DummyInt> *) {}
    void lazyPropagation(Treap::Nodo<DummyInt> *) {}
    bool operator ==(const DummyInt &o) const { return x == o.x; }
    bool operator <(const DummyInt &o) const { return x < o.x; }
    bool operator >(const DummyInt &o) const { return x > o.x; }
};

int main()
{
    Treap::Treap<DummyInt> t;
    t.insertar(1);
    t.insertar(2);
    cout << "SSSSSS" << endl;
    Treap::Nodo<DummyInt> * nodo;
    const int X = 1;
    nodo = t.insertionPoint(X);
    cout << (nodo && nodo->dat.x == X) << endl;
    return 0;
}
