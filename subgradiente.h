#ifndef SUBGRADIENTE_H
#define SUBGRADIENTE_H

#include "node.h"
#include "data.h"

class SubgradienteSolver {
private:
    int n;
    double** custoModificado;
    bool** isForbidden;

    void alocarBuffers(int n);
    void liberarBuffers(int n);

public:
    // só a função que executa geral o subgradiente
    // coloca um upper_bound / e cada nó carrega as suas regras advindas do branchAndBounds
    // n é o tamanho de vertices
    double run(Node& node, double upperBound, int n);

    
};

#endif