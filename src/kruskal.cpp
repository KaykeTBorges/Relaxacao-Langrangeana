#include "kruskal.h"

Kruskal::Kruskal(const std::vector<double>& lambda, const std::vector<std::vector<bool>>& is_forbidden, int n) {
    // pega a distancia direto de data e desconta os lambdas so pras arestas que nao tao proibidas
    // ja coloca com custo negativo porque a priority_queue do cpp ordena pelo maxHeap
    for (int i = 2; i <= n; i++) {
        for (int j = i + 1; j <= n; j++) {
            if (!is_forbidden[i][j]) {
                double c = Data::getInstance().getDistance(i, j) - lambda[i] - lambda[j];
                graph.push(make_pair(-c, make_pair(i, j)));
            }
        }
    }
}

void Kruskal::initDisjoint(int n){
	pset.resize(n+1);
	rnk.assign(n+1, 0); // zera o rank de todo mundo no inicio
	for (int i = 1; i <= n; i++){
		pset[i] = i;
	}
}

int Kruskal::findSet(int i){
	return (pset[i] == i) ? i : (pset[i] = findSet(pset[i]));
}

void Kruskal::unionSet(int i, int j){
	int root_i = findSet(i);
	int root_j = findSet(j);
	// union by rank - aplicação de APA
	if (root_i != root_j) {
		if (rnk[root_i] < rnk[root_j]) {
			pset[root_i] = root_j;
		} else if (rnk[root_i] > rnk[root_j]) {
			pset[root_j] = root_i;
		} else {
			pset[root_j] = root_i;
			rnk[root_i]++;
		}
	}
}

bool Kruskal::isSameSet(int i, int j){
	return (findSet(i) == findSet(j));
}

OneTree Kruskal::getEdges(){
	return edges;
}

double Kruskal::MST(int nodes){
	initDisjoint(nodes);

	double cost = 0;
	int arestasNecessarias = nodes - 2;

	while(!graph.empty() && (int)edges.size() < arestasNecessarias){
		pair<double, Edge> p = graph.top();
		graph.pop();

		if(!isSameSet(p.second.first, p.second.second)){
			edges.push_back(make_pair(p.second.first, p.second.second));
			cost += (-p.first);
			unionSet(p.second.first, p.second.second);
		}
	}

	return cost;
}