#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cstring>

using namespace std;

#define forn(i,n) for(int i=0;i<(int)(n);i++)
#define forsn(i,s,n) for(int i=(s);i<(int)(n);i++)
#define dforsn(i,s,n) for(int i=(n)-1;i>=(int)(s);i--)

struct Seg
{
    int accum;
    bool flipped;
    int i,j;
    Seg() : accum(0), flipped(false), i(0), j(0) {}
    Seg(int oi,int oj) : accum(0), flipped(false), i(oi), j(oj) {}
    
    void prefix(int l)
    {
        if (flipped)
            i = j - l;
        else
            j = i + l;
    }
    
    void suffix(int l)
    {
        if (flipped)
            j = i + l;
        else
            i = j - l;
    }
    
    void flip()
    {
        flipped ^= 1;
    }
    
    void inc()
    {
        accum = (accum + 1) % 26;
    }
};

/*
ostream &operator << (ostream &o, const Seg & seg)
{
    o << "(" << seg.i << "," << seg.j << "; +" << seg.accum;
    if (seg.flipped) o << " | FLIPPED";
    o << ")";
    return o;
}
*/

void rebuild(char *s, const char *old, const Seg *v, int vs)
{
    forn(i, vs)
    {
        int delta;
        const char *start, *end;
        if (v[i].flipped)
        {
            delta = -1;
            start = old + v[i].j-1;
            end = old + v[i].i-1;
        }
        else
        {
            delta = 1;
            start = old + v[i].i;
            end = old + v[i].j;
        }
        for (const char *p = start; p != end; p += delta)
            *s++ = 'a' + (*p - 'a' + v[i].accum) % 26;
    }
}

int work(int i, int j, int k,int l, Seg *bv, const Seg *old, int vs)
{
    int pi = -1, pj = -1, pk = -1, pl = -1;
    int preI = -1, suJ = -1, preK = -1, suL = -1;
    int x = 0;
    forn(vi, vs)
    {
        const int S = old[vi].j - old[vi].i;
        if (pi == -1 && i <  x + S) pi = vi, preI = i - x;
        if (pj == -1 && j <= x + S) pj = vi, suJ = x + S - j;
        if (pk == -1 && k <  x + S) pk = vi, preK = k - x;
        if (pl == -1 && l <= x + S) pl = vi, suL = x + S - l;
        x += S;
    }
    Seg *v = bv;
    // Parte inicial
    forn(vi, pi) *v++ = old[vi];
    if (preI)
    {
        *v = old[pi];
        v->prefix(preI);
        v++;
    }
    // Segundo dado vuelta
    if (pk == pl)
    {
        *v = old[pk];
        v->suffix(v->j - v->i - preK);
        v->prefix(v->j - v->i - suL);
        v->flip();
        v->inc();
        v++;
    }
    else
    {
        {
            *v = old[pl];
            v->prefix(v->j - v->i - suL);
            v->flip();
            v->inc();
            v++;
        }
        dforsn(vi, pk+1, pl)
        {
            *v = old[vi];
            v->flip();
            v->inc();
            v++;
        }
        {
            *v = old[pk];
            v->suffix(v->j - v->i - preK);
            v->flip();
            v->inc();
            v++;
        }        
    }
    // Del medio
    if (pk == pj)
    {
        *v = old[pk];
        int epk = preK - (v->j - v->i - suJ);
        v->suffix(suJ);
        v->prefix(epk);
        if (v->j - v->i > 0)
            v++;
    }
    else
    {
        if (suJ)
        {
            *v = old[pj];
            v->suffix(suJ);
            v++;
        }
        forsn(vi, pj+1, pk) *v++ = old[vi];
        if (preK)
        {
            *v = old[pk];
            v->prefix(preK);
            v++;
        }
    }
    // Primero dado vuelta
    if (pi == pj)
    {
        *v = old[pi];
        v->suffix(v->j - v->i - preI);
        v->prefix(v->j - v->i - suJ);
        v->flip();
        v->inc();
        v++;
    }
    else
    {
        {
            *v = old[pj];
            v->prefix(v->j - v->i - suJ);
            v->flip();
            v->inc();
            v++;
        }
        dforsn(vi, pi+1, pj)
        {
            *v = old[vi];
            v->flip();
            v->inc();
            v++;
        }
        {
            *v = old[pi];
            v->suffix(v->j - v->i - preI);
            v->flip();
            v->inc();
            v++;
        }
    }
    // Parte del final
    
    if (suL)
    {
        *v = old[pl];
        v->suffix(suL);
        v++;
    }
    forsn(vi, pl+1, vs) *v++ = old[vi];
    return v - bv;
}

const int MAX_LEN = 1000000;
const int MAX_SEGS = 1000;

Seg v[2][MAX_SEGS];
char s[2][MAX_LEN];

int main()
{
    #ifdef ACMTUYO
        freopen("example2.in","r",stdin);
    #endif
    int TT; scanf("%d",&TT);
    forn(tt,TT)
    {
        int N;
        scanf("%s %d", s[0],  &N);
        const int L = strlen(s[0]);
        s[1][L] = 0;
        int STEP = max(25, 3 * (int)sqrt(N));
        int current = 0;
        int vs = 1;
        int cs = 0;
        v[cs][0] = Seg(0, L);
        forn(opnum,N)
        {
            if (vs > STEP)
            {
                rebuild(s[1-current], s[current], v[cs], vs);
                current ^= 1;
                vs = 1;
                v[cs][0] = Seg(0, L);
            }
            int i,j,k,l;
            scanf("%d%d%d%d", &i,&j,&k,&l);
            i--; k--;
            vs = work(i,j,k,l,v[1-cs] , v[cs], vs);
            cs ^= 1;
        }
        rebuild(s[1-current], s[current], v[cs], vs);
        current ^= 1;
        printf("%s\n", s[current]);
    }
    return 0;
}
