#ifndef CLASIFICADOR_HPP
#define CLASIFICADOR_HPP
#include "matriz.hpp"
#include "normas.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <stdlib.h>
#include <algorithm>
#include <utility>

using namespace std;

//typedef unordered_map<int, int> DiccNatANat;
//typedef map<int, double> DiccNatADouble;

struct medidas_info
{
    double accuracy_general;
    double precision_general;
    double recall_general;
    vector<int> clases_predichas;

};

std::ostream& operator<<(std::ostream& os, medidas_info& obj)
{
    os<<"Accuracy general\n";
    os<<obj.accuracy_general<<'\n';
    os<<"Precision general\n";
    os<<obj.precision_general<<'\n';
    os<<"Recall general\n";
    os<<obj.recall_general<<'\n';

   /* os<<"Accuracy por clase\n";
    for(DiccNatADouble::const_iterator it = obj.accuracy_x_clase.cbegin();it != obj.accuracy_x_clase.cend();++it)
        os<<it->first<<": "<<it->second<<'\n';
    
    os<<"Precision por clase\n";
    for(DiccNatADouble::const_iterator it = obj.precision_x_clase.cbegin();it != obj.precision_x_clase.cend();++it)
        os<<it->first<<": "<<it->second<<'\n';

    os<<"Recall por clase\n";
    for(DiccNatADouble::const_iterator it = obj.recall_x_clase.cbegin();it != obj.recall_x_clase.cend();++it)
        os<<it->first<<": "<<it->second<<'\n';*/

    return os;
}

class Clasificador
{
    public:
        Clasificador(const Matriz<double>&, const vector<int>&, const bool norma_2);

        vector<int> clasificar(int k, const Matriz<double>& casos_tests) const; 
        medidas_info clasificar_y_medir(const Matriz<double>&, const vector< int>&, int k) const;
        void asignar_base_de_datos(const Matriz<double>&, const vector<int>&);
        int kNN(int k, const Matriz<double>& caso_test) const;

    private:

        Matriz<double> base_datos;
        vector<int> train_clase_x_fila;
        bool norma_2;
};

Clasificador::Clasificador(const Matriz<double>& datos, const vector<int>& clases, const bool norma_2):base_datos(datos),train_clase_x_fila(clases),norma_2(norma_2){};

void Clasificador::asignar_base_de_datos(const Matriz<double>& datos, const vector<int>& clases)
{
    base_datos = datos;
    train_clase_x_fila = clases;
};

// Dada una matriz de reviews casos_test, devuelve un vector con las predicciones de clases hecha por kNN
vector<int> Clasificador::clasificar(int k, const Matriz<double>& casos_tests) const
{
    vector<int> clases_predichas(casos_tests.filas(), 0);
    for(int i = 0;i < casos_tests.filas();i++)
    {
        Matriz<double> review_a_testear(casos_tests.copy_fil(i));

        clases_predichas[i] = kNN(k, review_a_testear);
    }  

    return clases_predichas;
};

// Clasifica los datos de test sobre la base de datos y devuelve un sumario con toda la info
// tests es la matriz con reviews a clasificar
// clase test contiene las clases  a los que corresponde cada review de tests, lo usamos para medir como clasificamos
// k es el valor de vecinos a comparar en kNN
medidas_info Clasificador::clasificar_y_medir(const Matriz<double>& tests, const vector<int>& clases_tests, int k) const
{
    medidas_info info;
    int tp = 0;
    int fp = 0;
    int tn = 0;
    int fn = 0;
    // Clasificamos las reviews de tests
    vector<int> clases_predichas(clasificar(k, tests));
    info.clases_predichas = clases_predichas;

    for (int i = 0; i < tests.filas(); ++i)
    {
        
        std::cerr << "Prediciendo " << i << " / " << tests.filas() << '\r';

        bool label = (bool)clases_tests[i];

        bool predi = (bool)clases_predichas[i];
        if (label && predi) tp++;
        else if (label && !predi) fn++;
        else if (!label && predi) fp++;
        else if (!label && !predi) tn++;
    }

    info.accuracy_general = (tp + tn)/ (double) clases_predichas.size();
    info.recall_general = (tp / (double)(tp + fp));
    info.precision_general = (tp / (double)(tp + fn));



    return info;
};
               	 
/*
 *
 * Implementaciones de kNN
 *
 */


// Metodo kNN canonico
// Primero busca los k vecinos mas cercanos usando norma2
// Luego realiza la votacion para ver a que clase corresponde el caso de test

int Clasificador::kNN(int k, const Matriz<double>& caso_test) const
{
    // Vector que guarda las distancias de los k mas cercanos
    vector<double> min(k,numeric_limits<double>::max());

    //vector que guarda el numero de fila de los k minimos.
    vector<int> res(k, 0);

    Matriz<double> resta(caso_test);

    //Buscamos los k vecinos mas cercanos
    for(int i=0;i < base_datos.filas();i++)
    {
        Matriz<double> otra(base_datos.copy_fil(i));
        resta -= otra;
        
        double distancia;
        if(norma_2){distancia = norma_2_vec(resta);}
        else {distancia = norma_1(resta);}



        int actual = i;

        for(int j=0;j < k;j++)
        {
            if(distancia < min[j])
            {
                swap(min[j], distancia);
                swap(res[j], actual);
                for (int l = j+1; l < k; ++l)
                {   swap(min[l], distancia);
                    swap(res[l],actual);    
                }
                break;
            } 
        } 
        resta = caso_test;
    }

    //Votamos para ver a cual clase le corresponde a cada review

   vector<int> clases_votadas = vector<int>(2,0);
    for(int i = 0;i < k;i++)
    {
        clases_votadas[train_clase_x_fila.at(res[i])] += 1;
    }

    //Vemos cual clase recibio la mayor cantidad de votos y lo devolvemos como resultado
    int max     = 0;
    int moda    = 0;
    for(uint i = 0; i < clases_votadas.size(); i++)
    {
        if(clases_votadas[i] > max)
        {
            max = clases_votadas[i];
            moda = i;
        }
    }

    return moda;

};


#endif
