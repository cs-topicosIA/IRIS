#include "RedNeuronal.h"

RedNeuronal::RedNeuronal()
{
    nCapas = 0;
    capas = 0;

    neuronasXCapa = 0;

    nEntradas = 0;
    entradas = 0;

    nSalidas = 0;
    salidas = 0;

    fa = 1;

    nIteracions = 10;

    maximoError = 0.1;
}

RedNeuronal::~RedNeuronal()
{

    for(int i = 0; i < nCapas; ++i)
        delete capas[i];
    delete []capas;

    for(int i = 0; i < neuronasXCapa[0]; ++i)
        delete []entradas[i];
    delete []entradas;

    for(int i = 0; i < neuronasXCapa[nCapas - 1]; ++i)
        delete []salidas[i];
    delete []salidas;

    delete []neuronasXCapa;

}

void RedNeuronal::ejecutar()
{
    int errores;
    bool retrocedo;
    int minimo = 150;
    bool sigoEntrenando = true;
    for(int it = 0; it < nIteracions  && sigoEntrenando; ++it)
    {
        errores = 0;
        sigoEntrenando = false;
        for(int entr = 0; entr < nEntradas; ++entr)
        {
            retrocedo = false;
            copiarEntrada(entr);
            for(int cap = 0; cap < nCapas - 1; ++cap)
                avanzar(*capas[cap], *capas[cap + 1]);

            verificarCapaFinal(entr, retrocedo);
            if(retrocedo) ++ errores;//Cuenta las entradas q faltan ajustar

            if(!retrocedo) continue;
            sigoEntrenando = true;

            actualizarErrorCapaFinal(entr);

            for(int cap = nCapas - 2; cap > 0; --cap)
            {
                actualizarPeso(*capas[cap], *capas[cap + 1]);
                actualizarError(*capas[cap], *capas[cap + 1]);
            }

            actualizarPeso(*capas[0], *capas[1]);

        }


        if(errores < minimo) {
            minimo = errores;
            guardarPesos(minimo,it);
        }
        cout << it << " " << errores << endl;
    }
    cout << "el minimo fue:" << minimo << endl;
}

RedNeuronal::RedNeuronal(int numeroCapas)
{
    nCapas = numeroCapas;
    neuronasXCapa = new int[nCapas];

    cout << "Numero de neuronas en la capa de entrada: ";
    cin >> neuronasXCapa[0];

    for(int i = 1; i < nCapas - 1; ++i){
        cout << "Numero de neuronas en la capa Oculta " << i << ": ";
        cin >> neuronasXCapa[i];
    }

    cout << "Numero de neuronas en la capa de salida: ";
    cin >> neuronasXCapa[nCapas - 1];

    capas = new Capa*[nCapas];

    //Inicializar Capas,excepto la ultima capa
    for(int i = 0; i < nCapas - 1; ++i)
        capas[i] = new Capa(neuronasXCapa[i] + 1, neuronasXCapa[i + 1] + 1);

    capas[nCapas - 1] = new Capa(neuronasXCapa[nCapas - 1] + 1, 1);

    fa = 0.5;

    nIteracions = 100000;
    maximoError = 0.2;

    //Inicializar datos de prueba y salida
    inicializarIOIris();

    //Inicializar umbrales
    inicializarUmbrales();

    inicializarPesos();
    //cargarPesosEntrenados();

}


void RedNeuronal::inicializarIOIris()
{
    int nRegistros, salida;
    ifstream lee("data/train.txt");

    lee >> nRegistros;

    nEntradas = nRegistros;
    nSalidas = nRegistros;

    entradas = new double*[nRegistros];
    for(int i = 0; i < nRegistros; ++i)
        entradas[i] = new double[4];

    salidas = new double*[nRegistros];
    for(int i = 0; i < nRegistros; ++i)
        salidas[i] = new double[3];

    double maximo = -10, minimo =  10;
    for(int reg = 0; reg < nRegistros; ++reg)
    {
        for(int entr = 0; entr < 4; ++entr)
        {
            lee >> entradas[reg][entr];
            if(entradas[reg][entr] > maximo) maximo = entradas[reg][entr];
            if(entradas[reg][entr] < minimo) minimo = entradas[reg][entr];

        }

        lee >> salida;
        salidas[reg][salida] = 1;
    }

    for(int reg = 0; reg < nRegistros; ++reg)
    {
        for(int entr = 0; entr < 4; ++entr)
            entradas[reg][entr] =  (entradas[reg][entr] - minimo) / (maximo - minimo);
    }

    lee.close();
}

void RedNeuronal::inicializarUmbrales()
{
    for(int i = 0; i < nCapas; ++i){
        capas[i]->u[0] = -1;
        capas[i]->y[0] = -1;
    }

}

void RedNeuronal::copiarEntrada(int pos)
{
    copy(entradas[pos], entradas[pos] + neuronasXCapa[0], capas[0]->y + 1);
}

double RedNeuronal::funcionActivacion(double u)
{
    return 1.0 /(1 + exp(-u));
    //return tanh(u);
}

double RedNeuronal::derivadaFuncion(double u)
{
    double eu = exp(u);
    double rp = eu / pow(eu + 1, 2);
    return rp;
    //return 1.0 / pow(cosh(u), 2);
}

void RedNeuronal::avanzar(Capa &cOrigen, Capa &cDestino)
{
    double suma;
    for(int j = 1; j < cDestino.m; ++j)
    {
        suma = 0;
        for(int i = 0; i < cOrigen.m; ++i)
            suma += cOrigen.y[i] * cOrigen.peso[i][j];

        cDestino.u[j] = suma;
        cDestino.y[j] = funcionActivacion(suma);

    }
}

void RedNeuronal::verificarCapaFinal(int sal, bool &retrocedo)
{
    for(int neur = 0; neur < neuronasXCapa[nCapas - 1]; ++neur)
    {
        if(abs(salidas[sal][neur] - capas[nCapas - 1]->y[neur + 1]) > maximoError)
        {
            retrocedo = true;
            return;
        }
    }
}

void RedNeuronal::actualizarErrorCapaFinal(int sal)
{
    double dif;
    for(int neur = 0; neur < neuronasXCapa[nCapas - 1]; ++neur)
    {
        dif = salidas[sal][neur] - capas[nCapas - 1]->y[neur + 1];
        capas[nCapas - 1]->error[neur + 1] = dif * derivadaFuncion(capas[nCapas - 1]->u[neur + 1]);
    }
}

void RedNeuronal::actualizarPeso(Capa &cOrigen, Capa &cDestino)
{
    for(int i = 0; i < cOrigen.m; ++i)
        for(int j = 1; j < cDestino.m; ++j)
            cOrigen.peso[i][j] += (fa * cOrigen.y[i] * cDestino.error[j]);
}

void RedNeuronal::actualizarError(Capa &cOrigen, Capa &cDestino)
{
    double suma;
    for(int i = 1; i < cOrigen.m; ++i)
    {
        suma = 0;
        for(int j = 1; j < cDestino.m; ++j)
            suma +=  (cOrigen.peso[i][j] * cDestino.error[j]);

        cOrigen.error[i] = derivadaFuncion(cOrigen.u[i]) * suma;

    }
}

void RedNeuronal::inicializarPesos()
{
    srand(time(0));
    for(int cap = 0; cap < nCapas - 1; ++cap)
    {
        for(int m = 0; m < capas[cap]->m; ++m)
            for(int n = 0; n < capas[cap]->n; ++n)
                capas[cap]->peso[m][n] = 1.0 * (rand() % 2001 - 1000) / 1000;
    }
}

void RedNeuronal::cargarPesosEntrenados()
{
    int aux, m, n;

    FILE *lee = fopen("pesos/pesos(32k-0.2)", "rb");

    fread(&aux, sizeof(aux), 1, lee);
    cout << "nerror " << aux<< endl;
    fread(&aux, sizeof(aux), 1, lee);
    cout << "it " << aux<< endl;
    fread(&aux, sizeof(aux), 1, lee);
    cout << "capas " << aux<< endl;

    int a;
    for(int cap = 0; cap < nCapas - 1; ++cap)
    {
        fread(&m, sizeof(m), 1, lee);
        fread(&n, sizeof(n), 1, lee);
        for(int m = 0; m < capas[cap]->m; ++m)
            fread(capas[cap]->peso[m], sizeof(double), n, lee);

        cout << m << " " << n << endl;
        for(int m = 0; m < capas[cap]->m; ++m)
            for(int n = 0; n < capas[cap]->n; ++n)
                cout << capas[cap]->peso[m][n] << " ";

    }

    cout << "cargo todos los pesos" << endl;

    fclose(lee);
}

void RedNeuronal::guardarPesos(int nError, int iteracion)
{
    FILE *esc = fopen("pesos/pesosFinales", "wb");
    fwrite(&nError, sizeof(nError), 1, esc);
    fwrite(&iteracion, sizeof(iteracion), 1, esc);
    fwrite(&nCapas, sizeof(nCapas), 1, esc);
    for(int c = 0; c < nCapas - 1; ++c)
    {
        int m = neuronasXCapa[c]  + 1;
        int n = neuronasXCapa[c + 1] + 1;
        fwrite(&m, sizeof(m), 1, esc);
        fwrite(&n, sizeof(n), 1, esc);

        for(int i = 0; i < m; ++i)
            fwrite(capas[c]->peso[i], sizeof(double), n, esc);
    }

    fclose(esc);
}
