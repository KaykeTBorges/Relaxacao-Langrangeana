#include "subgradiente.h"
#include "poda.h"
#include <cmath>
#include <algorithm>

void SubgradienteSolver::assegurarBuffers(int n) {
    if (bufferSize == n) return;

    is_forbidden.assign(n + 1, std::vector<bool>(n + 1, false));
    bufferSize = n;
}

double SubgradienteSolver::run(Node& node, double UB, int n) {
    assegurarBuffers(n);

    // marca so os arcos proibidos desse no na matriz booleana
    // assim evita ficar criando matriz nova
    for (const auto& arc : node.forbidden_arcs) {
        is_forbidden[arc.first][arc.second] = true;
        is_forbidden[arc.second][arc.first] = true;
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
        }

        // resolve a 1-arvore passando os arcos proibidos e calcula o w
        double cost1Tree = node.calculateOneTree(is_forbidden, n);
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
        if (is_ciclo || podavel(w, UB) || somatorio_graus == 0) {
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

    // desmarca os arcos proibidos no final pra matriz voltar ao estado limpo
    // ai ela ja fica pronta pro proximo no sem precisar recriar nada
    for (const auto& arc : node.forbidden_arcs) {
        is_forbidden[arc.first][arc.second] = false;
        is_forbidden[arc.second][arc.first] = false;
    }

    return w_star;
}