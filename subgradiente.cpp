#include "subgradiente.h"
#include <cmath>
#include <algorithm>

void SubgradienteSolver::alocarBuffers(int n) {
    custoModificado = new double*[n + 1];
    isForbidden = new bool*[n + 1];
    for (int i = 1; i <= n; i++) {
        custoModificado[i] = new double[n + 1];
        isForbidden[i] = new bool[n + 1];
    }
}

void SubgradienteSolver::liberarBuffers(int n) {
    for (int i = 1; i <= n; i++) {
        delete[] custoModificado[i];
        delete[] isForbidden[i];
    }
    delete[] custoModificado;
    delete[] isForbidden;
}

double SubgradienteSolver::run(Node& node, double UB, int n) {
    alocarBuffers(n);

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            isForbidden[i][j] = false;
        }
    }
    for (const auto& arc : node.forbidden_arcs) {
        isForbidden[arc.first][arc.second] = true;
        isForbidden[arc.second][arc.first] = true;
    }

    // melhor lower_bound, começa -infinito porque queremos maximizar
    // quanto maior, mais proximo do otimo com base no dual lagrangiano
    // o otimo é sempre o limitante superior, porque os lambdas são aplicados
    // aos que que tem quantidades de arestas diferentes de 2, logo a busca é pelo otimo
    // de todas as cidades com ligação 2
    // e o kruskal sempre vai encontrar a arvore de tamanho minimo
    double w_star = -INFINITE; 
    vector<double> lambda_star = node.lambda; 
    vector<int> degrees_star(n + 1);
    OneTree edges_star;
    bool is_ciclo_star = false;

    double epsilon = 1.0;
    double epsilon_min = 1e-5;
    int k = 0;
    int k_max = 30; 

    while (epsilon > epsilon_min) {
        double lambda_sum = 0.0;
        for (int i = 1; i <= n; i++) {
            lambda_sum += node.lambda[i];
            for (int j = 1; j <= n; j++) {
                if (i == j || isForbidden[i][j]) {
                    custoModificado[i][j] = INFINITE; // diagonal principal ou arco proibido
                } else {
                    custoModificado[i][j] = Data::getInstance().getDistance(i, j) - node.lambda[i] - node.lambda[j];
                }
            }
        }

        // resolve 1-arvore e calcula w
        double cost1Tree = node.calculateOneTree(custoModificado, n);
        double w = cost1Tree + 2.0 * lambda_sum;

        // calculo do somatorio
        int somatorio_graus = 0;
        bool is_ciclo = true;

        // coloquei isso antes para poder por uma verificação no nó se ele é ciclo
        // isso vai evitar várias checagens de ciclo, só faço uma vez O(N) e uso essa checagem
        for (int i = 1; i <= n; i++) {
            int calculoGrau = 2 - node.degrees[i]; // (2 - degree(i))
            somatorio_graus += calculoGrau * calculoGrau;
            if (calculoGrau != 0) {
                is_ciclo = false; // se algum nó não possui grau 2, não é um ciclo
            }
        }

        if (w > w_star) {
            w_star = w;
            lambda_star = node.lambda;
            degrees_star = node.degrees;
            edges_star = node.one_tree_edges;
            is_ciclo_star = is_ciclo;
            k = 0;
        } else {
            k++; 
            
            if (k >= k_max) {
                k = 0;
                epsilon = epsilon / 2.0;
            }
        }     

        // parada antecipada se encontrou ciclo hamiltoniano / ultrapassou o upper_bound -> quebra / gradiente nulo -> evitar divisão por zero
        if (is_ciclo || w >= UB || somatorio_graus == 0) {
            break;
        }

        double mu = epsilon * ((UB - w) / somatorio_graus);

        for (int i = 1; i <= n; i++) {
            node.lambda[i] += mu * (2 - node.degrees[i]); 
        }
    }

    // restaura no nó os melhores dados encontrados durante o subgradiente
    node.lambda = lambda_star;
    node.lower_bound = w_star;
    node.degrees = degrees_star;
    node.one_tree_edges = edges_star;
    node.is_ciclo = is_ciclo_star;

    liberarBuffers(n);

    return w_star;
}