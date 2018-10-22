#include "matriz_aux.hpp"

using namespace std;

const int CANT_POTENCIA = 10000;

//Algoritmo de la diapo para PCA
//Devuelve un vector de autovectores que se utilizan luego como transformacion caracteristica
//Los autovectores son los de la matriz de covarianza de xt*x
//Las columnas son los autovectores


Matriz<double> pca_autovectores(const Matriz<double>& datos, const Matriz<double>& med, unsigned int alfa)
{
    Matriz<double> x(datos);
    Matriz<double> v(datos.filas(), 1, 1);

    x -= v*med;
    x /= std::sqrt(double(datos.filas() - 1));

    Matriz<double> xt(x.traspuesta());


    Matriz<double> autovec = obtener_autovectores(xt*x, CANT_POTENCIA, alfa);

    return autovec;
}


Matriz<double> pca_autovalores(const Matriz<double>& datos, unsigned int alfa)
{
    Matriz<double> x(datos);
    Matriz<double> med = media(x);
    Matriz<double> v(datos.filas(), 1, 1);


    x -= v*med;
    x /= std::sqrt(double(datos.filas() - 1));

    Matriz<double> res = obtener_autovalores(x.traspuesta()*x, CANT_POTENCIA, alfa);
    return res;
}


// Version mas rapida de tc, solo funciona si datos es un vector fila, no una matriz
Matriz<double> tc_vector(const Matriz<double>& autovec, const Matriz<double>& datos, const Matriz<double>& med)
{
    //
    return (datos - med)* autovec;
}
// Version mas rapida de tc, solo funciona si datos es una matriz, no un vector
Matriz<double> tc_matriz(const Matriz<double>& autovec, const Matriz<double>& datos, const Matriz<double>& med)
{
    Matriz<double> canon(datos.filas(),1,1);

    //tc de los datos
    return (datos - canon*med)* autovec;
}