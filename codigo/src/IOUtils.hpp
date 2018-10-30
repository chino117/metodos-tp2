#ifndef IOUTILS_H_
#define IOUTILS_H_

#include <iostream>
#include <fstream>
#include <cstdio>
#include <limits>
#include <algorithm>
#include "pca.hpp"
#include <getopt.h>
#include "../src_catedra/vector_builder.h"
#include <stdlib.h>
#include <ctime>

using namespace std;

//Modo de operacion

enum Modo{
    kNN = 0,
    PCA,

};

struct info_archivo
{
    //pathas a archivos.
    string db_path = "../datos/imdb_tokenized.csv";
    string in_train = "";
    string in_test = "";
    string out_res;
    string output_medidas;
    Modo m;

    unsigned int n_train;
    unsigned int n_test;

    bool experimentacion;
    bool norma_2;

    unsigned int k;
    unsigned int alpha;

    //mapea indice o review id a clase.
    map<int,int> train_cases;
    map<int,int> test_cases;

    //la posicion i-esima contiene la clase correspondiente a la muestra de la fila i-esima de las 
    //matrices de train y test respectivamente. 
    vector<int> train_clase_x_fila;
    vector<int> test_clase_x_fila;

    int iteraciones;
    double lower_filter;
    double upper_filter;
    double epsilon_potencia;

    vector<int> clases_predichas;


};

//Para mostrar/escribir la info de un archivo.in
std::ostream& operator<<(std::ostream& os, const info_archivo& obj)
{
    os<<'\n';
    os<<"Cantidad de reviews a entrenar: "<<obj.n_train<<'\n';
    os<<"Cantidad de componentes principales(k): "<<obj.alpha<<'\n';
    os<<"Cantidad de reviews a testear: "<<obj.n_test<<'\n';
    os<<"path in train: "<<obj.in_train<<'\n';
    os<<"path in test: "<<obj.in_test<<'\n';
    os<<"mdeiciones: "<<obj.output_medidas<<'\n';
    os<<"k de knn: "<<obj.k<<'\n';
    os<<"m "<<obj.m<<'\n';
    os<<"experimentacion "<< obj.experimentacion <<'\n';


    return os;
}

class IOUtils {
public:
	IOUtils();
	~IOUtils();
	int parse(int arc, char** argv, info_archivo& info);
	void leer_archivos_csv(string path_1, string path_2, info_archivo& info, const VectorizedEntriesMap& train_entries, const VectorizedEntriesMap& test_entries);
    Matriz<double> leer_bag_of_words(vector<double>& bag_of_words, int N);
    Matriz<double> armar_base_entrenamiento(info_archivo &info, VectorizedEntriesMap& train_entries, int N);
    Matriz<double> armar_casos_tests(info_archivo &info,  VectorizedEntriesMap& test_entries, int N);
	vector<int>  obtener_clases_train(info_archivo &info);
	vector<int> obtener_clases_test(info_archivo &info);
	void escribir_autovalores(const Matriz<double>& datos, const info_archivo& info,const std::string& output);
	void escribir_output(string &out_res, info_archivo &info, vector<int> &clases_predichas);
	void escribir_medidas(info_archivo &info, medidas_info &r, clock_t total);
};

IOUtils::IOUtils(){
}

IOUtils::~IOUtils(){
}


int IOUtils::parse(int argc, char** argv, info_archivo& info){

    if (argc < 9)
    {
        std::cout<<"Parametros de entrada insuficientes"<<'\n';
        return 1;
    }

    //Cargo los datos del archivo input

    info.k = 1;
    info.alpha = 15;
    info.m = kNN;
    int modo_s;
    info.experimentacion = false;
    info.iteraciones = 6000;
    info.epsilon_potencia = EPSILON;
    info.lower_filter = 0.01;
    info.upper_filter = 0.99;
    info.norma_2 = false;

   	char option;
    while ((option = getopt(argc, argv, "m:d:i:q:o:k:a:r:eit:ep:l:u:n:")) != -1) {

        switch (option) {
            case 'm':{
            	    modo_s = stoi(optarg);
    				if(modo_s == 0) //"kNN"
                         info.m = kNN;
                     else if(modo_s == 1) //"PCA" 
                         info.m = PCA;
                     else
                    {
                         std::cout<<"ERROR:Parametro de operacion incorrecto\n";
                         return 1;
                     }
                     break;
            }
            case 'd':{
                info.db_path = optarg;
                break;
            }
            case 'i':{
            	info.in_train = optarg;
            	break;
            }
            case 'q':{
            	info.in_test = optarg;
            	break;
            }
            case 'o':{
            	info.out_res = optarg;
            	info.output_medidas = info.out_res + ".medidas";
            	break;
            }
            case 'k':{
            	info.k = stoi(optarg);

        		if(info.k == 0)
        		{
            		std::cout<<"ERROR: Valor k para kNN tiene que ser distinto de 0.\n";
            		return 1;
       			 }
            	break;
            }
            case 'a':{
            	info.alpha = stoi(optarg);
            	if(info.alpha == 0)
        		{
           			std::cout<<"ERROR: Valor alpha para PCA tiene que ser distinto de 0.\n";
            		return 1;
        		}
            	break;
            }         
            case 'r':{
            	info.output_medidas = optarg;
            	break;
            }
            case 'e':{
            	info.experimentacion = true;
            	break;
            }
            case 't':{
                info.iteraciones = stoi(optarg);
                break;
            }
            case 'p':{
                info.epsilon_potencia = stod(optarg);
                break;
            }
            case 'l':{ 
                info.lower_filter = stod(optarg);
                break;
            }
            case 'u':{
                info.upper_filter = stod(optarg);
                break;
            }
            case 'n':{
                info.norma_2 = stoi(optarg);
                break;
            }

            default: { // si las opciones son inválidas
           
                exit(1);
                break;
            }
        }

    }


    // Vemos que metodo para clasificar vamos a usar
    // kNN, PCA 
    
    // Si fue proporcionado, leemos el path del out_res de las medidas

    std::ifstream test_output_medida(info.output_medidas.c_str());
    if(test_output_medida.good())
    {
        test_output_medida.close();
        std::remove(info.output_medidas.c_str()); 
    }


    //Si existe el archivo out_res, lo borramos
    std::ifstream test_out_res(info.out_res.c_str());
    if(test_out_res.good())
    {
      (test_out_res).close();
      std::remove(info.out_res.c_str()); 
    }

    return 0;


}    

void IOUtils::leer_archivos_csv(string path_1, string path_2, info_archivo& info, const VectorizedEntriesMap& train_entries, const VectorizedEntriesMap& test_entries){


    string review;
    string etiqueta;
    info.n_train = 0;
    
if(path_1 != ""){
    fstream fs1;
    fs1.open(path_1.c_str(),std::fstream::in);
    
    if(!fs1.good())
    {
        cout<<"Fallo lectura archivo 1 .csv";
        return;
    };


    while(fs1 >> review)
    {   
        fs1 >> etiqueta;

        int id = 0;
        int clase = 0;
       

        id = stoi(review.substr(0, review.find_first_of(",")));
        clase = stoi(etiqueta.substr(0, etiqueta.find_first_of(",")));

        info.train_cases[id] = clase;

        info.n_train++;

    }    
    fs1.close();
    
}else{

    info.n_train = train_entries.size();
}
    ////////////

if(path_2 != ""){


    fstream fs2;
    fs2.open(path_2.c_str(),std::fstream::in);
    
    if(!fs2.good())
    {
        std::cout<<"Fallo lectura archivo 2 .csv";
        return;
    }

	info.n_test = 0;

    while(fs2 >> review)
    {

        fs2 >> etiqueta;

        int id = 0;
        int clase = 0;

        id = stoi(review.substr(0, review.find_first_of(",")));
        clase = stoi(etiqueta.substr(0, etiqueta.find_first_of(",")));
 
        info.test_cases[id] = clase;
        info.n_test++;;
    }

    fs2.close();

}else{
    info.n_test = test_entries.size();
}

}

Matriz<double> IOUtils::leer_bag_of_words(vector<double>& bag_of_words, int N)
{
    
    
    Matriz<double> temp(N, 1, 0);
  
    int i = 0;
    for(auto it = bag_of_words.begin();it != bag_of_words.end(); it++)
    {
        //cout << "leyendo" << endl;
        //cout << (*it) << endl;
        temp[i][0] = (*it);
        i++;
        //temp[i][0] = bag_of_words[i];
    }
    //cout << "leido" << endl;
    return temp;
}


// Devuelve una matriz con las reviews de entrenamiento como vectores fila
Matriz<double> IOUtils::armar_base_entrenamiento(info_archivo &info, VectorizedEntriesMap& train_entries, int N)
{


    Matriz<double> res(info.n_train, N, 0);
    int cant_reviews= 0;

    info.train_clase_x_fila = vector<int>(info.n_train);

if(info.in_train != ""){
    for (auto it = info.train_cases.begin(); it != info.train_cases.end(); it++)
    {
        auto it2 = train_entries.find(it->first);
        if( it2 != train_entries.end()){

            Matriz<double> bow = leer_bag_of_words(it2->second.bag_of_words, N);
          
            res.set_fil(cant_reviews, bow);
        
            // Agrego clase al vector de clase x fila del train.
            info.train_clase_x_fila[cant_reviews] = it->second;


            cant_reviews++;
        }
        else{
            (info.train_cases).erase(it);
            info.n_train--;
        }
    }
}
else{
    
    for (auto it = train_entries.begin(); it != train_entries.end(); it++){

        Matriz<double> bow = leer_bag_of_words(it->second.bag_of_words, N);
          
        res.set_fil(cant_reviews, bow);
        
        // Agrego clase al vector de clase x fila del train.
        info.train_clase_x_fila[cant_reviews] = it->second.is_positive;
        cant_reviews++;   

    }
}


return res;

}

// Devuelve una matriz con las reviews de test como vectores fila
Matriz<double> IOUtils::armar_casos_tests(info_archivo &info, VectorizedEntriesMap& test_entries, int N)
{
    Matriz<double> res(test_entries.size(), N, 0);
    int cant_reviews= 0;

    info.test_clase_x_fila = vector<int>(info.n_test);

if(info.in_test != ""){
    for(auto it = info.test_cases.begin(); it != info.test_cases.end(); it++)
    {   
        auto it2 = test_entries.find(it->first);
        if(it2 != test_entries.end() ){

            Matriz<double> bow = leer_bag_of_words(it2->second.bag_of_words, N);
          
            res.set_fil(cant_reviews, bow);

            info.test_clase_x_fila[cant_reviews] = it->second;

            cant_reviews++;
        }
        else{
            (info.test_cases).erase(it);
            info.n_test--;
        }
        
    }

}
else{

      for (auto it = test_entries.begin(); it != test_entries.end(); it++){

        Matriz<double> bow = leer_bag_of_words(it->second.bag_of_words, N);
          
        res.set_fil(cant_reviews, bow);
        
        // Agrego clase al vector de clase x fila del train.
        info.test_clase_x_fila[cant_reviews] = it->second.is_positive;
        info.test_cases[it->first] = it->second.is_positive;
        cant_reviews++;   

    }  

}    
    return res;
}

//Devuelve un vector tal que para cada fila de la matriz de train, devuelva su clase. 
vector<int> IOUtils::obtener_clases_train(info_archivo &info)
{

    return info.train_clase_x_fila;

}

//Devuelve un vector tal que para cada fila de la matriz de test, devuelve su clase..
vector<int> IOUtils::obtener_clases_test(info_archivo &info)
{

    return info.test_clase_x_fila;
}

// Escribe en el archivo de output los autovalores de la matriz de covarianza de datos
void IOUtils::escribir_autovalores(const Matriz<double>& datos, const info_archivo& info,const std::string& output)
{
    string filename = info.out_res+".autovalores";
    Matriz<double> autoval_pca = pca_autovalores(datos, info.alpha, info.iteraciones, info.epsilon_potencia);

    std::ifstream test_out_autovalroes(filename.c_str());
    if(test_out_autovalroes.good())
    {
      (test_out_autovalroes).close();
      std::remove(filename.c_str()); 
    }

    std::fstream f;
    f.precision(10);
    f.open(filename.c_str(),std::fstream::out | std::fstream::app);
    
    if(!f.good())
    {
        std::cout<<"Fallo al escribir autovalores\n";
        assert(false);
    }

    for(int i = 0;i < autoval_pca.filas();i++)
        f<<std::sqrt(autoval_pca[i][0])<<'\n';

    f.close();
}

void IOUtils::escribir_output(string &out_res, info_archivo &info, vector<int> &clases_predichas){

    ofstream file;
    file.open(out_res.c_str());
    auto it = info.test_cases.begin();
    for (unsigned int i = 0; i < clases_predichas.size(); ++i)
    {
        file << it->first << ", " << clases_predichas[i] << "," << endl;
        it++;
    }
    file.close();
}

void IOUtils::escribir_medidas(info_archivo &info, medidas_info &r, clock_t total){

    fstream f;
    f.open((info.output_medidas).c_str(), std::fstream::out);
    if(!f.good())
        cout<<"Error: No se pudo escribir el archivo con las medidas tomadas\n";
    f<<std::setprecision(10);
    f<<"Tiempo(segundos):\n"<<total/CLOCKS_PER_SEC<<'\n';
    f<<r;
    f.close();
}


#endif