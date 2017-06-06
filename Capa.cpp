#include "Capa.h"

Capa::Capa()
{
    m = 0;
    n = 0;
    u = 0;
    y = 0;
    peso = 0;
    error = 0;
}

Capa::Capa(int m, int n)
{
    redimensionar(m, n);
}

Capa::~Capa()
{
    delete []u;
    delete []y;
    delete []error;

    for(int i = 0; i < m; ++i)
        delete []peso[i];
    delete []peso;
}

void Capa::redimensionar(int m, int n)
{
    this->m = m;
    this->n = n;
    u = new double[m];
    y = new double[m];
    error = new double[m];

    peso = new double*[m];
    for(int i = 0; i < m; ++i)
        peso[i] = new double[n];
}


