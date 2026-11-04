#include "kruskal.h"

Kruskal::Kruskal(const std::vector<std::vector<double>>& dist, int n) {
    for (int i = 2; i <= n; i++) {
        for (int j = i + 1; j <= n; j++) {
			// Adiciona aresta apenas se não for proibida custo < INFINITE
			// é um maxHeap, ele indica 
            if (dist[i][j] < INFINITE) {
                graph.push(make_pair(-dist[i][j], make_pair(i, j)));
            }
        }
    }
}

void Kruskal::initDisjoint(int n){
	pset.resize(n+1);
	for (int i = 1; i <= n; i++){
		pset[i] = i;
	}
}

int Kruskal::findSet(int i){
	return (pset[i] == i) ? i : (pset[i] = findSet(pset[i]));
}

void Kruskal::unionSet(int i, int j){
	pset[findSet(i)] = findSet(j);
}

bool Kruskal::isSameSet(int i, int j){
	return (findSet(i) == findSet(j))? true:false;
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