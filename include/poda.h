#ifndef PODA_H
#define PODA_H

#include <cmath>

constexpr double EPS = 1e-9;

inline bool podavel(double lb, double ub) {
    return std::ceil(lb - EPS) >= ub - EPS;
}

#endif
