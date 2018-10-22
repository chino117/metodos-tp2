#ifndef CLASIFICADOR_HPP
#define CLASIFICADOR_HPP
#include "matriz.hpp"
#include "normas_aux.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <stdlib.h>
#include <algorithm>
#include <utility>

using namespace std;

typedef std::unordered_map<unsigned int, unsigned int> DiccNatANat;
typedef std::map<unsigned int, double> DiccNatADouble;

struct medidas_info
{
    double accuracy_general;
    double precision_general;
    double recall_general;
    DiccNatADouble accuracy_x_clase;
    DiccNatADouble precision_x_clase;
    DiccNatADouble recall_x_clase;
    vector<unsigned int> clases_predichas;

};

std::ostream& operator<<(std::ostream& os, medidas_info& obj)
{
    os<<"Accuracy general\n";
    os<<obj.accuracy_general<<'\n';
    os<<"Precision general\n";
    os<<obj.precision_general<<'\n';
    os<<"Recall general\n";
    os<<obj.recall_general<<'\n';

    os<<"Accuracy por clase\n";
    for(DiccNatADouble::const_iterator it = obj.accuracy_x_clase.cbegin();it != obj.accuracy_x_clase.cend();++it)
        os<<it->first<<": "<<it->second<<'\n';
    
    os<<"Precision por clase\n";
    for(DiccNatADouble::const_iterator it = obj.precision_x_clase.cbegin();it != obj.precision_x_clase.cend();++it)
        os<<it->first<<": "<<it->second<<'\n';

    os<<"Recall por clase\n";
    for(DiccNatADouble::const_iterator it = obj.recall_x_clase.cbegin();it != obj.recall_x_clase.cend();++it)
        os<<it->first<<": "<<it->second<<'\n';

    return os;
}



class Clasificador
{
    public:
        Clasificador(const Matriz<double>&, const DiccNatANat&);

        std::vector<unsigned int> clasificar(unsigned int k, const Matriz<double>& casos_tests) const; 
        medidas_info clasificar_y_medir(const Matriz<double>&, std::vector<unsigned int>, unsigned int k) const;
        void asignar_base_de_datos(const Matriz<double>&, const DiccNatANat&);
        unsigned int kNN(unsigned int k, const Matriz<double>& caso_test) const;

    private:

        Matriz<double> base_datos;
        DiccNatANat clase_x_review;
};

Clasificador::Clasificador(const Matriz<double>& datos, const DiccNatANat& clases):base_datos(datos),clase_x_review(clases){};

void Clasificador::asignar_base_de_datos(const Matriz<double>& datos, const DiccNatANat& clases)
{
    base_datos = datos;
    clase_x_review = clases;
};

// Dada una matriz de reviews casos_test, devuelve un vector con las predicciones de clases hecha por kNN
std::vector<unsigned int> Clasificador::clasificar(unsigned int k, const Matriz<double>& casos_tests) const
{
    std::vector<unsigned int> res(casos_tests.filas(), 0);
    for(int i = 0;i < casos_tests.filas();i++)
    {
        Matriz<double> review_a_testear(casos_tests.copy_fil(i));

        res[i] = kNN(k, review_a_testear);
    }  


    return res;
};

// Clasifica los datos de test sobre la base de datos y devuelve un sumario con toda la info
// tests es la matriz con reviews a clasificar
// clase test contiene las clases  a los que corresponde cada review de tests, lo usamos para medir como clasificamos
// k es el valor de vecinos a comparar en kNN
medidas_info Clasificador::clasificar_y_medir(const Matriz<double>& tests, std::vector<unsigned int> clases_tests, unsigned int k) const
{
    medidas_info info;

    unsigned int aciertos = 0;
    unsigned int total = tests.filas();

    DiccNatADouble total_x_cat;
    DiccNatADouble verdaderos_pos_cat;
    DiccNatADouble verdaderos_neg_cat;
    DiccNatADouble falsos_pos_cat;
    DiccNatADouble falsos_neg_cat;

    // Clasificamos las reviews de tests
    std::vector<unsigned int> clases_predichas(clasificar(k, tests));
    info.clases_predichas = clases_predichas;

    for(int i = 0; i < tests.filas();i++)
    {
        if(clases_predichas[i] == clases_tests[i])
        {
            aciertos++;
            if(verdaderos_pos_cat.count(clases_predichas[i]) == 0)
                verdaderos_pos_cat[clases_predichas[i]] = 0;

            verdaderos_pos_cat[clases_predichas[i]]++;

            for(int j = 0; j < tests.filas();j++){
                if(clases_tests[j] != clases_tests[i] ){
                    if(verdaderos_neg_cat.count(clases_tests[j])){
                        verdaderos_neg_cat[clases_tests[j]] = 0;
                    }
                    verdaderos_neg_cat[clases_tests[j]]++;
                }
            }
        }
        else
        {
            if(falsos_neg_cat.count(clases_tests[i]) == 0)
                falsos_neg_cat[clases_tests[i]] = 0;

            if(falsos_pos_cat.count(clases_predichas[i]) == 0)
                falsos_pos_cat[clases_predichas[i]] = 0;

            for(int j = 0; j < tests.filas();j++){
                if(clases_tests[j] != clases_predichas[i] && clases_tests[j] != clases_tests[i] ){
                    if(verdaderos_neg_cat.count(clases_tests[j])){
                        verdaderos_neg_cat[clases_tests[j]] = 0;
                    }
                    verdaderos_neg_cat[clases_tests[j]]++;
                }
            }

            falsos_neg_cat[clases_tests[i]]++;
            falsos_pos_cat[clases_predichas[i]]++;
        }

        if(total_x_cat.count(clases_tests[i]) == 0)
            total_x_cat[clases_tests[i]] = 0;

        total_x_cat[clases_tests[i]]++;
    }
    info.accuracy_general = 0;
    info.accuracy_general = double(aciertos) / double(total);

    // Calculo accuracy, precision y recall general
    for(DiccNatADouble::const_iterator i = verdaderos_pos_cat.cbegin();i != verdaderos_pos_cat.cend();++i)
		if(i->second > 0)
			info.precision_general +=
				(i->second + verdaderos_neg_cat[i->first]) /
				(i->second + falsos_pos_cat[i->first] + verdaderos_neg_cat[i->first] + falsos_neg_cat[i->first]);
   

    info.precision_general = 0;
    for(DiccNatADouble::const_iterator i = verdaderos_pos_cat.cbegin();i != verdaderos_pos_cat.cend();++i)
        if(i->second > 0)
	        info.precision_general += (i->second)/(i->second + falsos_pos_cat[i->first]);

    info.precision_general /= double(total_x_cat.size());

    info.recall_general = 0;
    for(DiccNatADouble::const_iterator i = verdaderos_pos_cat.cbegin();i != verdaderos_pos_cat.cend();++i)
        if(i->second > 0)
	        info.recall_general += (i->second)/(i->second + falsos_neg_cat[i->first]);

    info.recall_general /= double(total_x_cat.size());

    // Calculo accuracy, precision y recall por categoria
    for(DiccNatADouble::const_iterator it = total_x_cat.cbegin();it != total_x_cat.cend();++it)
    {
        double v_pos = verdaderos_pos_cat[it->first];
        double f_pos = falsos_pos_cat[it->first];
        double v_neg = verdaderos_neg_cat[it->first];
        double f_neg = falsos_neg_cat[it->first];
        if((v_pos + f_pos + v_neg + f_neg) > 0)
            info.accuracy_x_clase[it->first] = (v_pos + v_neg) / (v_pos + f_pos + v_neg + f_neg);
        else
            info.accuracy_x_clase[it->first] = 0;
    }      

    for(DiccNatADouble::const_iterator it = total_x_cat.cbegin();it != total_x_cat.cend();++it)
    {
        double v_pos = verdaderos_pos_cat[it->first];
        double f_pos = falsos_pos_cat[it->first];
        if(v_pos + f_pos > 0)
            info.precision_x_clase[it->first] = v_pos / (v_pos + f_pos);
        else
            info.precision_x_clase[it->first] = 0;
    }

    for(DiccNatADouble::const_iterator it = total_x_cat.cbegin();it != total_x_cat.cend();++it)
    {
        double v_pos = verdaderos_pos_cat[it->first];
        double f_neg = falsos_neg_cat[it->first];

        if(v_pos + f_neg > 0)
            info.recall_x_clase[it->first] = v_pos / (v_pos + f_neg);
        else
            info.recall_x_clase[it->first] = 0;
    }

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

unsigned int Clasificador::kNN(unsigned int k, const Matriz<double>& caso_test) const
{
    std::vector<double> min(k,std::numeric_limits<double>::max());
    std::vector<unsigned int> res(k, 0);
    Matriz<double> resta(caso_test);

    //Buscamos los k vecinos mas cercanos
    for(int i=0;i < base_datos.filas();i++)
    {
        Matriz<double> otra(base_datos.copy_fil(i));
        resta -= otra;
        
        double distancia = norma_2_vec(resta);
        unsigned int actual = i;

        for(unsigned int j=0;j < k;j++)
        {
            if(distancia < min[j])
            {
                std::swap(min[j], distancia);
                std::swap(res[j], actual);
                break;
            } 
        } 
        resta = caso_test;
    }

    //Votamos para ver a cual clase le corresponde a cada review

    DiccNatANat clases_votadas;
    for(unsigned int i = 0;i < k;i++)
    {
        if(clases_votadas.count(clase_x_review.at(res[i])) == 0)
            clases_votadas[clase_x_review.at(res[i])] = 1;
        else 
            clases_votadas[clase_x_review.at(res[i])] += 1;
    }

    //Vemos cual clase recibio la mayor cantidad de votos y lo devolvemos como resultado
    unsigned int max     = 0;
    unsigned int arg_max = 0;
    for(DiccNatANat::const_iterator it = clases_votadas.cbegin(); it != clases_votadas.cend();++it)
    {
        if(it->second > max)
        {
            max = it->second;
            arg_max = it->first;
        }
    }

    return arg_max;

};


#endif
