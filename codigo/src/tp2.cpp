#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdio>
//#include "../src_catedra/vector_builder.h"

#include <limits>
#include "eigen.hpp"
#include <cmath>
#include <ctime>
#include "clasificador.hpp"
#include <algorithm>
#include "IOUtils.hpp"


using namespace std;

int tp2(IOUtils &utilidad, info_archivo &info){

    double lower_filter = info.lower_filter;
    double upper_filter = info.upper_filter;
    
    auto filter_out = [lower_filter, upper_filter] (const int token, const FrecuencyVocabularyMap & vocabulary) {
        /**
         *  Lambda para usar como filtro de vocabulario
         *  Retorna `true` si `token` debe eliminarse
         *  Retorna `false` si `token` no debe eliminarse
         **/
        double token_frecuency = vocabulary.at(token);
        return token_frecuency < lower_filter || token_frecuency > upper_filter;
    };
    VectorizedEntriesMap train_entries;
    VectorizedEntriesMap test_entries;
    build_vectorized_datasets( info.db_path,train_entries, test_entries, filter_out);

    int N = train_entries.begin()->second.bag_of_words.size();


    //Seteamos el seed de random
    srandom(0);
    
    utilidad.leer_archivos_csv(info.in_train,info.in_test,info);
    cout << "hi" << endl;
    //Cambio la cantidad de digitos con las que nos muestra los doubles en pantalla
    cout << fixed;std::setprecision(50);   

    Matriz<double> train(utilidad.armar_base_entrenamiento(info, train_entries, N));
    Matriz<double> test(utilidad.armar_casos_tests(info, test_entries, N));

    std::cerr
        << "N: " << N << std::endl
        << "Dataset de entrenamiento: " << info.n_train << " entradas" << std::endl
        << "Dataset de testeo: " << info.n_test << " entradas" << std::endl;

    // Escribimos los autovalores de la matriz con la que hacemos el cambio de variables
    if (info.experimentacion) {utilidad.escribir_autovalores(train, info, info.out_res);}

    clock_t total = 0;
    // Si usamos PCA realizamos la transformacion de las bases
    if(info.m == PCA)
    { 
        clock_t conv = clock();

        Matriz<double> med = media(train);

        Matriz<double> autovec = pca_autovectores(train, med, info.alpha, info.iteraciones, info.epsilon_potencia);
        // Aplico la transformacion dada por PCA sobre los datos de la base de train
        train = tc_matriz(autovec, train, med);  

        // Aplico la transformacion dada por PCA sobre los datos de la base de test 
        test = tc_matriz(autovec, test, med);    

        conv = (clock() - conv);
        total = conv;
    }
     
    clock_t clasif = clock();
    // Clasificamos las review de test usando las de train y obtenemos mediciones
    Clasificador c(train, info.train_clase_x_fila, info.norma_2);
    medidas_info r = c.clasificar_y_medir(test, info.test_clase_x_fila, info.k);
    clasif = ((clock() - clasif));
    total = total + clasif;

    utilidad.escribir_output(info.out_res, info, r.clases_predichas);

    // Escribimos las mediciones en un archivo

    utilidad.escribir_medidas(info, r, total);

    if (!info.experimentacion){
        //cout << "modo:  " << info.m << endl;
         //cout << "k: " << info.k << endl;
         //cout << "alpha: " << info.alpha << endl;
         cout << r << endl;
     }

     return 0;

}


int main(int argc, char* argv[]){
    //Filtramos los datos de entrada

    IOUtils utilidad;
    info_archivo info;

    utilidad.parse(argc,argv, info);

    tp2(utilidad,info);
   

    return 0;
}
