#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "data.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <utility>

using namespace std;

typedef pair<int, int> Edge;
typedef vector<Edge> OneTree;

class Kruskal{
public:
	Kruskal(const std::vector<double>& lambda, const std::vector<std::vector<bool>>& is_forbidden, int n);

	double MST(int nodes);
	OneTree getEdges();

private:
	priority_queue <pair<double,Edge>> graph;
	vector <int> pset;
	vector <int> rnk;
	OneTree edges;

	void initDisjoint(int n);
	int findSet(int i);
	void unionSet(int i, int j);
	bool isSameSet(int i, int j);
};

#endif