#ifndef MATRIZ_AUX_HPP
#define MATRIZ_AUX_HPP

#include "matriz.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <limits>
#include <cmath>
#include <stdlib.h>

//Auxiliares de matrices

bool comp_doubles_epsilon(double a, double b,double epsilon)
{
   return (fabs(a-b) < epsilon);
}

//Compara dos matrices, posición por posición, utilizando comp_doubles
bool son_parecidas_epsilon(const Matriz<double>& a, const Matriz<double>& b, 
                   double epsilon)
{
  if (a.filas() != b.filas() || a.columnas() != b.columnas())
     return false;

  for(int i = 0;i < a.filas();i++)
  {
     for(int j = 0;j < a.columnas();j++)
     {
        if (!comp_doubles_epsilon(a[i][j], b[i][j], epsilon))
           return false;
     }
  }  
  return true;
}
//Muestra una matriz vector como una matriz de cant_col columnas
void mostrar_vec_como_matriz(const Matriz<double>& a,int cant_col)
{
   
   for(int i = 0;i < a.filas()/cant_col;i++)
   {
      std::cout<<"[ ";
      for(int j = 0;j < cant_col;j++)
      {
        std::cout<<a[j+i*cant_col][0]<<" ";
      }
      std::cout<<"]\n";
   }

}

//Muestra una matriz, ignorando las posiciones que valen 0.
//Los elementos mostrados tienen la forma (valor_elemento, columna_elemento)
void mostrar_matriz_sin_ceros(const Matriz<double>& a)
{
    for(int i = 0;i < a.filas();i++)
    {
       std::cout<<"[";
       for(int j = 0;j <a.columnas();j++)
       {
      if(!comp_doubles_epsilon(a[i][j],0,0.0000001))
         std::cout<<"("<<a[i][j]<<","<<j<<") ";
       }
       std::cout<<"]\n";
    }
}


Matriz<double> gen_matriz_random(int filas, int columnas)
{
    Matriz<double> res(filas, columnas, 0);
    for(int i = 0;i < filas;i++)
        for(int j = 0;j < columnas;j++)
            res[i][j] = double(rand() % 100000);
    return std::move(res);
}

Matriz<double> gen_matriz_random(int filas, int columnas, int modulo)
{
    Matriz<double> res(filas, columnas, 0);
    for(int i = 0;i < filas;i++)
        for(int j = 0;j < columnas;j++)
            res[i][j] = double(rand() % modulo);
    return res;
}
#endif
