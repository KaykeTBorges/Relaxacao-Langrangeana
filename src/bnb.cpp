#include "bnb.h"
#include "poda.h"
#include <iostream>
#include <list>
#include <algorithm>
#include <cmath>

int BranchAndBound::selecionarVerticeRamificacao(const Node& node, int n) {
    int max_degree = 2;
    int best_vertice = -1;

    // seleciona o vértice com grau maior que 2 na 1-arvore atual
    for (int i = 1; i <= n; i++) {
        if (node.degrees[i] > max_degree) {
            max_degree = node.degrees[i];
            best_vertice = i;
        }
    }

    return best_vertice;
}


bool BranchAndBound::processarNo(const Node& node, double& best_ub, OneTree& best_tour) {
    // verifica se o lower bound do nó já torna impossível obter uma solução melhor
    if (podavel(node.lower_bound, best_ub)) {
        return false;
    }

    // se for um tour válido e melhor que o atual, atualiza o upper_bound
    // essa foi a verificação antes já feita lá no node de is_ciclo, 
    // para não verificar 2 vezes O(n)
    if (node.is_ciclo) {
        if (node.lower_bound < best_ub) {
            best_ub = node.lower_bound;
            best_tour = node.one_tree_edges;
        }

        // se for bom ou ruim ele vai voltar para o inicio
        // porque não faz sentido fazer as checagens abaixo, 
        // que são de ramificações e criações de filhos com arvores que são ciclos
        // logo que tem 2 arestas cada vertice, já que procuramos o de maior valor de graus
        return false;
    }

    return true;
}

void BranchAndBound::gerarFilhos(const Node& node, double best_ub, int n, std::vector<Node>& filhos) {
    // escolhe o no com maior grau (numero de arestas) para ramificar,
    // porque nela vai aplicar a restrição de vertices
    // a verificação que diz que todas as ligações dos vertices são menores ou iguais a 2
    int v_maior = selecionarVerticeRamificacao(node, n);

    // não sendo ciclo, mas não obtendo um caso válido para criação de filho e ramificar
    if (v_maior == -1) {
        return;
    }

    // identifica as arestas conectadas a esse vertice na 1-arvore
    for (const auto& edge : node.one_tree_edges) {
        if (edge.first != v_maior && edge.second != v_maior) {
            continue;
        }

        // criação do filho e aplicação da restrição
        Node child = node;
        child.forbidden_arcs.push_back(edge);

        // subgradiente no nó filho
        // porque se já der maior que o best_upperBound, já poda, 
        // para não usar em cima de algo que não é válido
        child.lower_bound = solver.run(child, best_ub, n);
        if (!podavel(child.lower_bound, best_ub)) {
            filhos.push_back(std::move(child));
        }
    }
}

Solution BranchAndBound::solveDFS(Node& root, double initial_ub, int n) {
    std::list<Node> tree;

    double best_ub = initial_ub;
    OneTree best_tour;

    // faz a resolução do subgradiente, que vai retornar um valor de lower_bound, daquela construção
    // eu poderia fazer sem retornar nada o run, mas só pra saber e ter controle do resultado
    // ele retorna o lower_bound, porque ele recebe o root como referencia, logo ele modifica todas as variaveis contidas nele
    // não precisava disso, mas vou manter por enquanto
    root.lower_bound = solver.run(root, best_ub, n);
    tree.push_back(std::move(root));

    while (!tree.empty()) {
        Node current = std::move(tree.back());
        tree.pop_back();

        // processa a poda e o tour válido 
        // false indica que não há necessidade de gerar filhos
        if (!processarNo(current, best_ub, best_tour)) {
            continue;
        }

        // separa o método para ramificar em filhos
        std::vector<Node> filhos;
        gerarFilhos(current, best_ub, n, filhos);

        // adiciona somente os filhos que não foram podados pelo lowerBound
        for (Node& filho : filhos) {
            tree.push_back(std::move(filho));
        }
    }

    return {best_ub, best_tour};
}

Solution BranchAndBound::solveBFS(Node& root, double initial_ub, int n) {
    std::list<Node> tree;

    double best_ub = initial_ub;
    OneTree best_tour;

    // faz a resolução do subgradiente, que vai retornar um valor de lower_bound, daquela construção
    // eu poderia fazer sem retornar nada o run, mas só pra saber e ter controle do resultado
    // ele retorna o lower_bound, porque ele recebe o root como referencia, logo ele modifica todas as variaveis contidas nele
    // não precisava disso, mas vou manter por enquanto
    root.lower_bound = solver.run(root, best_ub, n);
    tree.push_back(std::move(root));

    while (!tree.empty()) {
        Node current = std::move(tree.front());
        tree.pop_front();

        // processa a poda e o tour válido 
        // false indica que não há necessidade de gerar filhos
        if (!processarNo(current, best_ub, best_tour)) {
            continue;
        }

        // separa o método para ramificar em filhos
        std::vector<Node> filhos;
        gerarFilhos(current, best_ub, n, filhos);

        // adiciona somente os filhos que não foram podados pelo lowerBound
        for (Node& filho : filhos) {
            tree.push_back(std::move(filho));
        }
    }

    return {best_ub, best_tour};
}

bool compareLowerBound(const Node &a, const Node &b) {
    return a.lower_bound > b.lower_bound;
}

Solution BranchAndBound::solveBBS(Node& root, double initial_ub, int n) {
    std::priority_queue<Node, std::vector<Node>, decltype(&compareLowerBound)> tree(compareLowerBound);

    double best_ub = initial_ub;
    OneTree best_tour;

    // faz a resolução do subgradiente, que vai retornar um valor de lower_bound, daquela construção
    // eu poderia fazer sem retornar nada o run, mas só pra saber e ter controle do resultado
    // ele retorna o lower_bound, porque ele recebe o root como referencia, logo ele modifica todas as variaveis contidas nele
    // não precisava disso, mas vou manter por enquanto
    root.lower_bound = solver.run(root, best_ub, n);
    tree.push(std::move(root));

    while (!tree.empty()) {
        Node current = std::move(tree.top());
        tree.pop();

        // processa a poda e o tour válido 
        // false indica que não há necessidade de gerar filhos
        if (!processarNo(current, best_ub, best_tour)) {
            continue;
        }

        // separa o método para ramificar em filhos
        std::vector<Node> filhos;
        gerarFilhos(current, best_ub, n, filhos);

        // adiciona somente os filhos que não foram podados pelo lowerBound
        for (Node& filho : filhos) {
            tree.push(std::move(filho));
        }
    }

    return {best_ub, best_tour};
}

Solution BranchAndBound::solve(Node& root, double ub, int n, SearchStrategy s) {
    switch (s) {
        case SearchStrategy::BFS:
            return solveBFS(root, ub, n);
        case SearchStrategy::BBS:
            return solveBBS(root, ub, n);
        case SearchStrategy::DFS:
            return solveDFS(root, ub, n);
        default:
            return solveDFS(root, ub, n);
    }
}