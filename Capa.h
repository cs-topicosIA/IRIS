#ifndef CAPA_H
#define CAPA_H


class Capa
{
    public:
        Capa();
        Capa(int m, int n);
        ~Capa();

    public:
        int m, n;
        double *u;
        double *y;
        double  **peso;
        double *error;

    public:
        void redimensionar(int m, int n);

};

#endif // CAPA_H
