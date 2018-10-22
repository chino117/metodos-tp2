//#include "../src_catedra/vector_builder.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdio>
#include <limits>
#include "eigen.hpp"
#include <cmath>
#include <ctime>
#include "clasificador.hpp"
#include <algorithm>
#include "IOUtils.hpp"

using namespace std;

int main() {
    auto filter_out = [] (const int token, const FrecuencyVocabularyMap & vocabulary) {
        /**
         *  Lambda para usar como filtro de vocabulario
         *  Retorna `true` si `token` debe eliminarse
         *  Retorna `false` si `token` no debe eliminarse
         **/
        double token_frecuency = vocabulary.at(token);
        return token_frecuency < 0.005 || token_frecuency > 0.995;
    };
    VectorizedEntriesMap train_entries;
    VectorizedEntriesMap test_entries;
    build_vectorized_datasets(train_entries, test_entries, filter_out);
    cout << "hi" << endl;
    int N = train_entries.begin()->second.bag_of_words.size();

    /*int nnz = 0;
    for (int i = 0; i < train_entries.size(); ++i)
    {   //cout << "counting" << endl;
        for (auto it = (train_entries[i].bag_of_words).begin(); it != (train_entries[i].bag_of_words).end(); it++ )
        {
           if (*it < 0.0000001) nnz++; 
        }
    }

    cout << "nnz: " << nnz << endl;*/

    std::cerr
        << "N: " << N << std::endl
        << "Dataset de entrenamiento: " << train_entries.size() << " entradas" << std::endl
        << "Dataset de testeo: " << test_entries.size() << " entradas" << std::endl;

    int tp = 0;
    int fp = 0;
    int tn = 0;
    int fn = 0;
    int amount = 0;

    IOUtils utilidad;

    int alpha = 0;
    int m = train_entries.size();
    int m_test = 2000;
    int k = 3;

     //Seteamos el seed de random
    srandom(0);
   
    //Cambio la cantidad de digitos con las que nos muestra los doubles en pantalla   

    cout << fixed;std::setprecision(50);   
    //cout << info << endl;
    Matriz<double> train(utilidad.armar_base_entrenamiento(train_entries,N,m));
   
    Matriz<double> test(utilidad.armar_casos_tests(test_entries,N,m_test));

    // Obtenemos los sujetos de cada imagen de la base de train y test
    DiccNatANat s_train(utilidad.obtener_clases_train(train_entries,m));
    vector<unsigned int> s_test(utilidad.obtener_clases_test(test_entries, m_test));
    if(alpha > 0)
    { 

        Matriz<double> med = media(train);

        Matriz<double> autovec = pca_autovectores(train, med, alpha);
        // Aplico la transformacion dada por PCA sobre los datos de la base de train
        train = tc_matriz(autovec, train, med);  

        // Aplico la transformacion dada por PCA sobre los datos de la base de test 
        test = tc_matriz(autovec, test, med);    

    }


    Clasificador c(train, s_train);

    unsigned int cant_pos = 0;
    unsigned int cant_neg = 0;
    for (auto it = s_train.begin(); it != s_train.end(); ++it)
    {
        if((*it).second) cant_pos++;
        else cant_neg++;
    } 

    cout << cant_pos << endl;
    cout << cant_neg  << endl;

    cout << "test" << endl;

    cant_pos = 0;
    cant_neg = 0;
    for (auto it = s_test.begin(); it != s_test.end(); ++it)
    {
        if((*it)) cant_pos++;
        else cant_neg++;
    }
    cout << cant_pos << endl;
    cout << cant_neg  << endl;

/*
for (auto it = s_test.begin(); it != s_test.end(); ++it)
    {
        cout << (*it) << endl;
    }    
*/
    // 
    for (int i = 0;i < m_test;i++) {
        std::cerr << "Prediciendo " << amount << " / " << test_entries.size() << '\r';

        Matriz<double> review_a_testear(test.copy_fil(i));

        bool label = (bool)s_test[i];

        bool predi = (bool)c.kNN(k, review_a_testear);
        if (label && predi) tp++;
        else if (label && !predi) fn++;
        else if (!label && predi) fp++;
        else if (!label && !predi) tn++;
        amount++;
    }

    std::cerr
        << "                                    " << std::endl
        << tp << std::endl
        << fp << std::endl
        << tn << std::endl
        << fn << std::endl
        << "Accuracy: " << (tp + tn)/ (double) m_test<< std::endl
        << "Recall: " << (tp / (double)(tp + fp)) << std::endl
        << "Precision: " << (tp / (double)(tp + fn)) << std::endl;

    return 0;
}

