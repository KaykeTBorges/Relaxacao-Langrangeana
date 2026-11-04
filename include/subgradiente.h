#ifndef SUBGRADIENTE_H
#define SUBGRADIENTE_H

#include "node.h"
#include "data.h"
#include <vector>

class SubgradienteSolver {
private:
    std::vector<std::vector<double>> custoModificado;
    // aqui é uma melhoria, visto que as distancias são usadas n2 / 2 vezes
    // foi visto muito em APA queria trazer para cá, para testar
    std::vector<std::vector<double>> dist;
    int bufferSize = 0;

    void assegurarBuffers(int n);

public:
    // só a função que executa geral o subgradiente
    // coloca um upper_bound / e cada nó carrega as suas regras advindas do branchAndBounds
    // n é o tamanho de vertices
    double run(Node& node, double upperBound, int n);

    
};

#endif