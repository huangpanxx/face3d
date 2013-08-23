#include "newton.h"

#include <stdlib.h>
#include <string.h>

// 计算矩阵的逆
static int inverse( double** dfX, int n )
{
    int i, j,k, p;
    double maxV, tmp;
    double* A = *dfX;
    double* T = (double*)malloc(sizeof(double) * n * n);
    double* tArr = (double*)malloc(sizeof(double) * n);
    int row_size = sizeof(double) * n;

    memset( T, 0, sizeof(double)*n*n);
    for ( i = 0; i < n; ++i )
    {
        T[i*n+i] = 1.0;
    }

    for ( j = 0; j < n; ++j )
    {
        p = j;
        tmp = A[j*n+j];
        maxV = (tmp>=0)?(tmp):(-tmp);
        for ( i = j +1; i < n; ++i )
        {
            tmp = A[i*n+j];
            if ( tmp < 0 ) tmp = -tmp;
            if ( maxV < tmp )
            {
                p = i;
                maxV = tmp;
            }
        }

        if ( maxV < 1e-20 )
        {
            return -1;
        }

        if ( j != p )
        {
            memcpy(  tArr, A+j*n, row_size);
            memcpy( A+j*n, A+p*n, row_size);
            memcpy( A+p*n,  tArr, row_size);
            memcpy(  tArr, T+j*n, row_size);
            memcpy( T+j*n, T+p*n, row_size);
            memcpy( T+p*n,  tArr, row_size);
        }

        tmp = A[j*n+j];
        for ( i = j; i < n; ++i ) A[j*n+i] /= tmp;
        for ( i = 0; i < n; ++i ) T[j*n+i] /= tmp;
        for ( i = 0; i < n; ++i )
        {
            if ( i != j )
            {
                tmp = A[i*n+j];
                for ( k = j; k < n; ++k )
                    A[i*n+k] -= tmp * A[j*n+k];
                for ( k = 0; k < n; ++k )
                    T[i*n+k] -= tmp * T[j*n+k];
            }
        }
    }
    memcpy( A, T, row_size * n );
    free( T );
    free( tArr );

    return 0;
}

// 计算步长dx
static void calc_dx( double** dx
                   , double*  df
                   , double*  dfx
                   , double   lamda
                   , int      n
                   )
{
    int i, j;
    double* x = *dx;
    memset( x, 0, sizeof(double) * n);
    for ( i = 0; i < n; ++i )
    {
        for ( j = 0; j < n; ++j )
        {
            x[i] += -lamda * df[j] * dfx[i*n+j];
        }
    }
}

// 计算向量的无穷范数
static double norm_inf( double* A, int n )
{
    int i;
    double t = A[0];
    double ret = t;

    if ( t < 0 )
    {
        ret = -t;
    }

    for ( i = 1; i < n; ++i )
    {
        t = A[i];
        if ( t < 0 ) t = -t;
        if ( ret < t ) ret = t;
    }

    return ret;
}

// 牛顿下山法求解非线性方程组，求解成功返回0，失败返回-1
int  newton  ( double** X0                     // 迭代起始点
              , int n                           // 方程组维数
              , double lamda                    // 起始下山因子
              , double eps_x                    // 阈值
              , double eps_f                    // 阈值
              , void   (*f)( double** X, int n) // 带求解非线性方程组函数
              , void   (*df)(double** X, int n) // 带求解非线性方程组的导函数（Jacobi矩阵）
              )
{
    int i, ret = 0;
    int row_size = sizeof(double) * n;
    double*   X = *X0;
    double*  dx = (double*)malloc( row_size );
    double*  fX = (double*)malloc( row_size );
    double* dfX = (double*)malloc( n * row_size );
    double max_f, max_f1;

    memcpy( fX, X, row_size );
    f ( &fX, n );
    for ( ;; )
    {
        memcpy( dfX, X, row_size);
        df( &dfX, n );

        ret = inverse( &dfX, n ); // 计算逆

        if ( ret < 0 ) // Jacobi矩阵不可逆
        {
            goto end;
        }
        calc_dx( &dx, fX, dfX, lamda, n); // 计算步长

        max_f = norm_inf( fX, n );
        for ( i = 0; i < n; ++i )
        {
            X[i] += dx[i];
        }

        memcpy( fX, X, row_size);
        f( &fX, n );
        max_f1 = norm_inf( fX, n );

        if ( max_f1 < max_f )
        {
            if ( norm_inf( dx, n ) < eps_x )
            {
                break;
            }
            else
            {
                continue;
            }
        }
        else
        {
            if ( max_f1 < eps_f )
            {
                break;
            }
            else
            {
                lamda /= 2.0;
            }
        }
    }

end:
    free( dx );
    free( fX );
    free( dfX);

    return ret;
}
