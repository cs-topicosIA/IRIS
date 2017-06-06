#ifndef REDNEURONAL_H
#define REDNEURONAL_H

#include <iostream>
#include <algorithm>
#include <fstream>
#include "Capa.h"

using namespace std;

class RedNeuronal
{
    public:
        RedNeuronal();
        RedNeuronal(int numeroCapas);
        ~RedNeuronal();
        void ejecutar();

    protected:

    private:
        int nCapas;
        int *neuronasXCapa;
        Capa **capas;
        double **entradas;
        double **salidas;
        int nEntradas, nSalidas;
        double fa;
        int nIteracions;
        double maximoError;

    private:
        void inicializarIOIris();
        void inicializarUmbrales();
        void inicializarPesos();
        void copiarEntrada(int pos);
        void guardarPesos(int nError, int iteracion);
        void cargarPesosEntrenados();

    private:
        double funcionActivacion(double u);
        double derivadaFuncion(double u);

        void avanzar(Capa &cOrigen, Capa &cDestino);
        void verificarCapaFinal(int sal, bool &cambio);
        void actualizarErrorCapaFinal(int sal);

        void actualizarPeso(Capa &cOrigen, Capa &cDestino);
        void actualizarError(Capa &cOrigen, Capa &cDestino);

};

#endif // REDNEURONAL_H
