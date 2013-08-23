#ifndef NEWTON_H
#define NEWTON_H

// 牛顿下山法求解非线性方程（组）
int newton( double** X0
    , int n
    , double lmada
    , double eps_x
    , double eps_f
    , void   (*f)( double** X, int n)
    , void   (*df)(double** X, int n)
    );

#endif // NEWTON_H
