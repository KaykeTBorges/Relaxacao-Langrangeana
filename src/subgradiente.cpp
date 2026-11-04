#include "subgradiente.h"
#include "poda.h"
#include <cmath>
#include <algorithm>

void SubgradienteSolver::assegurarBuffers(int n) {
    if (bufferSize == n) return;

    custoModificado.assign(n + 1, std::vector<double>(n + 1, 0.0));
    dist.assign(n + 1, std::vector<double>(n + 1, 0.0));

    for (int i = 1; i <= n; i++)
        for (int j = i + 1; j <= n; j++)
            dist[i][j] = Data::getInstance().getDistance(i, j);

    bufferSize = n;
}

double SubgradienteSolver::run(Node& node, double UB, int n) {
    assegurarBuffers(n);

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

            for (int j = i+1; j <= n; j++) {
                    custoModificado[i][j] = dist[i][j] - node.lambda[i] - node.lambda[j];
            }
        }

        // no calculo do Kruskal só entra o triangulo superior da matrix, por isso min e max de indices
        // isso evitar ficar criando uma matrix de forbidden como eu estava fazendo e aplica só depois de todos os calculos
        // já percorrendo direto os forbidden advindos do node em especifico
        for (const auto& arc : node.forbidden_arcs)
            custoModificado[std::min(arc.first, arc.second)][std::max(arc.first, arc.second)] = INFINITE;

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

    return w_star;
}