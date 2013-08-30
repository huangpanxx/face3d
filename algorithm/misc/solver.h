#ifndef SOLVER_H
#define SOLVER_H

#include "../common.h"

class equations {
public:
    virtual double operator () (const VEC(double) &x,int i) const = 0;
    virtual int number() const = 0;
};


double iter_solve(const equations &eqs,std::vector<double> &x,int maxIter=1e3,double dx=1e-3,double eps=1e-3);

double least_square_iter_solve(const equations &eqs,int nvar, std::vector<double> &x,
                    int maxIter=1e3,double dx=1e-3,double eps=1e-3);

#endif // SOLVER_H
