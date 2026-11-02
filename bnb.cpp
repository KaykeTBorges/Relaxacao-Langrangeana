#include "bnb.h"
#include <iostream>
#include <stack>
#include <algorithm>

int BranchAndBound::selecionarVerticeRamificacao(const Node& node, int n) {
    int max_degree = 2;
    int best_vertice = -1;

    for (int i = 1; i <= n; i++) {
        if (node.degrees[i] > max_degree) {
            max_degree = node.degrees[i];
            best_vertice = i;
        }
    }

    return best_vertice;
}

Solution BranchAndBound::solve(Node& root, double initial_ub, int n) {
    std::stack<Node> tree;

    double best_ub = initial_ub;
    OneTree best_tour;

    // faz a resolução do subgradiente, que vai retornar um valor de lower_bound, daquela construção
    // eu poderia fazer sem retornar nada o run, mas só pra saber e ter controle do resultado
    // ele retorna o lower bound, porque ele recebe o root como referencia, logo ele modifica todas as variaveis contidas nele
    // não precisava disso, mas vou manter por enquanto
    root.lower_bound = solver.run(root, best_ub, n);
    tree.push(root);

    while (!tree.empty()) {
        Node current = tree.top();
        tree.pop();

        // poda do branchAndBound
        if (current.lower_bound >= best_ub) {
            continue;
        }

        // se for tour válido e melhor que o atual, atualiza o upper_bound
        // essa foi a verificação antes já feita lá no node, para não verificar 2 vezes O(n)
        if (current.is_ciclo) {
            if (current.lower_bound < best_ub) {
                best_ub = current.lower_bound;
                best_tour = current.one_tree_edges;
            }
            // se for bom ou ruim ele vai voltar para o inicio
            // porque não faz sentido fazer as checagens abaixo com arvores que são ciclos
            // logo que tem 2 arestas cada vertice
            continue; 
        }

        // escolhe o no com maior grau (numero de arestas) para ramificar, 
        // porque nela vai aplicar a restrição de vertices
        // logo os vertices ligados a ele serão colocados no forbidden
        int v_maior = selecionarVerticeRamificacao(current, n);
        // a verificação que diz que todas as ligações dos vertices são menores ou iguais a 2
        // não sendo ciclo, mas não obtendo um caso válido para criação de filho e ramificar
        if (v_maior == -1) {
            continue;
        }

        // identifica as arestas conectadas a esse vertice na 1-árvore
        std::vector<Edge> v_maior_edges;
        for (const auto& edge : current.one_tree_edges) {
            if (edge.first == v_maior || edge.second == v_maior) {
                v_maior_edges.push_back(edge);
            }
        }

        // criação dos filhos e rodando para ve se entra como resposta
        // porque se já der maior que o best_upperBound, já poda, para não usar em cima de algo que não é válido
        for (const auto& edge : v_maior_edges) {
            Node child = current;
            child.forbidden_arcs.push_back(edge);

            // subgradiente no nó filho
            child.lower_bound = solver.run(child, best_ub, n);

            if (child.lower_bound < best_ub) {
                tree.push(child);
            }
        }
    }

    return {best_ub, best_tour};
}