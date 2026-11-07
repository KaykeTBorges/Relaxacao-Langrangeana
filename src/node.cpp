#include "node.h"

double Node::calculateOneTree(const std::vector<std::vector<bool>>& is_forbidden, int n) {
    this->one_tree_edges.clear();
    this->degrees.assign(n + 1, 0);

    // roda o kruskal de 2 ate n pra pegar a arvore geradora minima sem o vertice 1
    // eu tive a primeira versão em que eu passava uma matriz, só que eu criava essa matriz
    // todas as vezes apenas depois eu modificava e aplicava o valor de lambda dela, ai preencher uma matriz é custoso
    // v2 logo pensei de uma forma de mandar e fazer o calculo direto, e conseguir os dados diretos, logo, passar as informações para quem usa
    Kruskal kruscal(this->lambda, is_forbidden, n);
    double costMst = kruscal.MST(n);

    OneTree oneTreeEdges = kruscal.getEdges();

    // se o kruskal nao conseguiu juntar todos nós por causa das proibicoes dos arcos
    if ((int)oneTreeEdges.size() < (n - 2)) {
        return INFINITE;
    }

    for (const auto& edge : oneTreeEdges) {
        this->one_tree_edges.push_back(edge);
        this->degrees[edge.first]++;
        this->degrees[edge.second]++;
    }

    // pega direto as 2 menores arestas ligadas no vertice 1 (raiz)
    // assim nao precisa criar uma priority_queue toda vez na heap
    int root = 1;
    double min1 = INFINITE, min2 = INFINITE;
    int v1 = -1, v2 = -1;

    for (int j = 2; j <= n; ++j) {
        if (!is_forbidden[root][j]) {
            double c = Data::getInstance().getDistance(root, j) - this->lambda[root] - this->lambda[j];
            if (c < min1) {
                min2 = min1;
                v2 = v1;

                min1 = c;
                v1 = j;
            } else if (c < min2) {
                min2 = c;
                v2 = j;
            }
        }
    }

    if (v1 == -1 || v2 == -1 || min2 >= INFINITE) {
        return INFINITE;
    }

    this->one_tree_edges.push_back({root, v1});
    this->degrees[root]++;
    this->degrees[v1]++;

    this->one_tree_edges.push_back({root, v2});
    this->degrees[root]++;
    this->degrees[v2]++;

    return costMst + min1 + min2;
}
