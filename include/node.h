#ifndef NODE_H
#define NODE_H

#include <vector>
#include <utility>
#include "kruskal.h"

// ela é um nó de arvore para aplicar como subproblema para branchAndBound
// não é apenas uma 1-arvore, carrega ali a melhor arvore encontrada
struct Node {
    std::vector<Edge> forbidden_arcs; // arcos proibidos
    std::vector<double> lambda; // vetor da aplicação de lambdas
    double lower_bound; // melhor LB para este nó
    std::vector<int> degrees; // graus de cada vértice na melhor 1-arvore
    OneTree one_tree_edges; // arestas da melhor 1-arvore
    bool is_ciclo; // cada nó carrega se é ciclo válido 

    Node(int n = 0) : lambda(n + 1, 0.0), lower_bound(0.0), is_ciclo(false) {}

    // calcula a 1-arvore
    double calculateOneTree(const std::vector<std::vector<bool>>& is_forbidden, int n);
};

#endif