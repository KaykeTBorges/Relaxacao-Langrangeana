#ifndef BNB_H
#define BNB_H

#include "node.h"
#include "subgradiente.h"
#include <vector>

enum class SearchStrategy {
    DFS,
    BFS,
    BBS
};

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

    Solution solveDFS(Node& root, double initial_ub, int n);
    Solution solveBFS(Node& root, double initial_ub, int n);
    Solution solveBBS(Node& root, double initial_ub, int n);

    bool processarNo(const Node& node, double& best_ub, OneTree& best_tour);
    void gerarFilhos(const Node& node, double best_ub, int n, std::vector<Node>& filhos);

public:
    Solution solve(Node& root, double initial_ub, int n, SearchStrategy strategy = SearchStrategy::DFS);
};

#endif