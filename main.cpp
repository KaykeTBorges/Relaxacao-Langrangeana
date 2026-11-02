#include <iostream>
#include <chrono>
#include <iomanip>

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

int main(int argc, char* argv[]) {
    Data::getInstance().read(argc, argv);
    
    int n = Data::getInstance().getDimension();
    std::string instance_file = argv[1];

    std::cout << "Instância: " << instance_file << "\n";
    std::cout << "Dimensão (N): " << n << " cidades\n";

    double initial_ub = buildTrivial(n);
    std::cout << "[UB Inicial] Ciclo Trivial Sequencial: " 
              << std::fixed << std::setprecision(2) << initial_ub << "\n\n";

    Node root(n);

    BranchAndBound bnb;

    auto start_time = std::chrono::high_resolution_clock::now();
    Solution sol = bnb.solve(root, initial_ub, n);
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "\nRESULTADOS DA EXECUÇÃO\n";
    std::cout << "Custo Ótimo:       " << std::fixed << std::setprecision(2) << sol.best_ub << "\n";
    std::cout << "Tempo de Execução:  " << std::fixed << std::setprecision(6) << elapsed.count() << " s\n";

    return 0;
}