#ifndef TYPES__H
#define TYPES__H

#include <vector>
#include <map>


// Estructuras para las entradas tokenizadas
struct TokenizedEntry {
    bool is_positive;
    std::vector<int> tokens;
};
using TokenizedEntriesMap = std::map<int, TokenizedEntry>;

// Estructuras para las entradas vectorizadas
struct VectorizedEntry {
    bool is_positive;
    std::vector<double> bag_of_words;
};
using VectorizedEntriesMap = std::map<int, VectorizedEntry>;

// Estructura para el vocabulario
using FrecuencyVocabularyMap = std::map<int, double>;


#endif  // TYPES__H

