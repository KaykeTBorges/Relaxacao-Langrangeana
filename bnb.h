#ifndef BNB_H
#define BNB_H

#include "node.h"
#include "subgradiente.h"
#include <vector>

struct Solution {
    double best_ub;
    OneTree best_tour;
};

class BranchAndBound {
private:
    SubgradienteSolver solver;

    // função para ajudar nas ramificações que vão ser feitas
    // porque vamos criar ramificações nos vertices de maior grau, que tem mais arestas 
    // ligadas a ele, poderia fazer isso direto no solve, mas pra separação fiz fora
    // ele retorna o indice
    int selecionarVerticeRamificacao(const Node& node, int n);

public:
    Solution solve(Node& root, double initial_ub, int n);
};

#endif