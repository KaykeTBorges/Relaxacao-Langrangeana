#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "data.h"
#include "node.h"
#include "bnb.h"

double buildTrivial(int n) {
    double total_cost = 0.0;

    for (int i = 1; i < n; ++i) {
        total_cost += Data::getInstance().getDistance(i, i + 1);
    }

    total_cost += Data::getInstance().getDistance(n, 1);

    return total_cost;
}

double buildVizinhoMaisProximo(int n) {
    std::vector<bool> visitado(n + 1, false);
    int atual = 1;
    visitado[atual] = true;
    double total_cost = 0.0;

    for (int quantidade = 1; quantidade < n; ++quantidade) {
        int maisProximo = -1;
        double menorDistancia = INFINITE;

        for (int candidato = 1; candidato <= n; ++candidato) {
            if (!visitado[candidato]) {
                double distancia = Data::getInstance().getDistance(atual, candidato);
                if (distancia < menorDistancia) {
                    menorDistancia = distancia;
                    maisProximo = candidato;
                }
            }
        }

        if (maisProximo == -1) {
            return INFINITE;
        }

        visitado[maisProximo] = true;
        total_cost += menorDistancia;
        atual = maisProximo;
    }

    total_cost += Data::getInstance().getDistance(atual, 1);
    return total_cost;
}

SearchStrategy parseStrategy(const char* value) {
    std::string strategy(value);

    if (strategy == "BFS") {
        return SearchStrategy::BFS;
    }
    if (strategy == "BBS") {
        return SearchStrategy::BBS;
    }

    return SearchStrategy::DFS;
}

int main(int argc, char* argv[]) {
    Data::getInstance().read(argc, argv);
    
    int n = Data::getInstance().getDimension();
    std::string instance_file = argv[1];

    std::cout << "Instância: " << instance_file << "\n";
    std::cout << "Dimensão (N): " << n << " cidades\n";

    double initial_ub = buildVizinhoMaisProximo(n);

    std::cout << "[UB Vizinho Mais Próximo]: "
              << std::fixed << std::setprecision(2) << initial_ub << "\n\n";

    Node root(n);

    BranchAndBound bnb;

    SearchStrategy strategy = SearchStrategy::DFS;
    if (argc > 2) {
        strategy = parseStrategy(argv[2]);
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    Solution sol = bnb.solve(root, initial_ub, n, strategy);
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "\nRESULTADOS DA EXECUÇÃO\n";
    std::cout << "Custo Ótimo:       " << std::fixed << std::setprecision(2) << sol.best_ub << "\n";
    std::cout << "Tempo de Execução:  " << std::fixed << std::setprecision(6) << elapsed.count() << " s\n";

    return 0;
}