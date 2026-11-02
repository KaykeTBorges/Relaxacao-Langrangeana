#include "node.h"
#include "data.h"
#include <queue>

double Node::calculateOneTree(double** custoModificado, int n){
    this->one_tree_edges.clear();
    this->degrees.assign(n+1, 0);

    Kruskal kruscal(custoModificado, n);
    double costMst = kruscal.MST(n);

    // vector de pares -> arestas
    OneTree oneTreeEdges = kruscal.getEdges();

    // uma checagem em que se o kruscal não conseguiu juntar todos nós
    // por conta das proibições dos arcos
    if ((int)oneTreeEdges.size() < (n - 2)) {
        return INFINITE;
    }
    
    for (const auto& edge : oneTreeEdges) {
        this->one_tree_edges.push_back(edge);
        this->degrees[edge.first]++;
        this->degrees[edge.second]++;
    }

    // é a fila de prioridade para a ligação com os depositos, volta para o inicio
    // logo só vai guardar a ligação de 1
    priority_queue<pair<double, int>> cheapestRootEdges;
    int root = 1;

    for (int j = 2; j <= n; ++j) {
        if (custoModificado[root][j] < INFINITE) {
            cheapestRootEdges.push({custoModificado[root][j], j});
            if (cheapestRootEdges.size() > 2) {
                // mantém apenas os 2 menores, retirando se encontrar um menor do que o que está
                cheapestRootEdges.pop(); 
            }
        }
    }

    double rootCost = 0.0;

    if (cheapestRootEdges.size() < 2){
        return INFINITE;
    }

    while (!cheapestRootEdges.empty()) {
        auto p = cheapestRootEdges.top();
        cheapestRootEdges.pop();

        double cost = p.first;
        int vertice = p.second;
        
        this->one_tree_edges.push_back({root, vertice});
        this->degrees[root]++;
        this->degrees[vertice]++;
        rootCost += cost;
    }

    return costMst + rootCost;
}