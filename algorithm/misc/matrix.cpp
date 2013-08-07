#include "matrix.h"
#include "common.h"
#include <QDebug>

cs* CSBuilder::buildCS(int *buf_x,int *buf_y, double *buf_v) const {
    std::map<long,double>::const_iterator it = this->_elems.begin();

    int i = 0;
    for(; it!=this->_elems.end(); ++i,++it) {
        int ik = it->first;
        double iv = it->second;
        buf_x[i] = ik % this->_ncols;
        buf_y[i] = ik / this->_ncols;
        buf_v[i] = iv;
    }
    cs c;
    c.m = this->_ncols; c.n = this->_nrows;
    c.nz = c.nzmax = this->_elems.size();
    c.p = buf_x; c.i = buf_y; c.x = buf_v;
    return cs_compress(&c);
}

bool cs_divide(double *dividend, CSBuilder *divisor) {
    const int sz = divisor->size();
    int *x = new int[sz], *y = new int[sz];
    double *v = new double[sz];
    cs *a = divisor->buildCS(x,y,v);
    int nRet = cs_qrsol(2,a,dividend);
    cs_spfree(a);
    delete[] x;
    delete[] y;
    delete[] v;
    return 0 != nRet;
}

bool solve_sparse(int* px,int *py,double *pv,double *pB,int sz,int N) {
    cs a;
    a.m = a.n = N;
    a.nz = a.nzmax = sz;

    a.p = px;    a.i = py;
    a.x = pv;

    cs *pa = cs_compress(&a);

    int nRet = cs_qrsol(2,pa,pB);
    cs_spfree(pa);
    return 0 != nRet;
}


bool gauss_elim(double *A,double *R,int n) {
    const int sz = square(n);
    zeros(R,sz);

    //单位阵
    for(int i=0;i<sz;++i)
        R[i] = i%n==i/n?1:0;

    /*gauss求逆*/
    //向下消元
    for(int y=0;y<n;++y) {
        int l = y;  //最大列
        double m = std::abs(A[y*n+y]);
        for(int i=y;i<n;++i) {
            double e = std::abs(A[i*n+y]);
            if(e > m) {
                l = i;
                m = e;
            }
        }



        //交换l,y行
        if(l!=y) for(int i=0;i<n;++i) {
            std::swap(R[l*n+i],R[y*n+i]);
            std::swap(A[l*n+i],A[y*n+i]);
        }

        //消元
        for(int i=y+1;i<n;++i) {
            double co = A[i*n+y] / A[y*n+y];
            if(std::isnan(co)) {
                qDebug()<<"gauss_elim failed";
                return false;
            }
            for(int j=0;j<n;++j) {
                A[i*n+j] -= A[y*n+j] * co;
                R[i*n+j] -= R[y*n+j] * co;
            }
        }
    }

    //向上消元
    for(int y=n-1;y>=0;--y) {
        for(int i=y-1;i>=0;--i) {
            double co = A[i*n+y]/A[y*n+y];
            for(int j=0;j<n;++j) {
                A[i*n+j] -= A[y*n+j]*co;
                R[i*n+j] -= R[y*n+j]*co;
            }
        }
        for(int i=0;i<n;++i)  R[y*n+i] /= A[y*n+y];
        A[y*n+y] = 1;
    }
    return true;
}

void mul(double * const A,double * const B,double *X,int m,int n,int z) {
    memset(X,0,sizeof(X[0])*m*z);
    for(int r=0;r<m;++r) {
        for(int c=0;c<z;++c) {
            for(int i=0;i<n;++i) {
                X[r*z+c] += A[r*n+i] * B[c+i*z];
            }
        }
    }
}

void mata(double * const A,double *AA,int m,int n) {
    matb(A,A,AA,m,n,n);
}

void maat(double * const A,double *AA,int m,int n) {
    mabt(A,A,AA,m,n,m);
}

void mabt(double* const A,double* const B,double *AtB,int m,int n,int z) {
    memset(AtB,0,m*z*sizeof(AtB[0]));
    for(int r=0;r<m;++r) {
        for(int c=0;c<z;++c) {
            for(int i=0;i<n;++i) {
                AtB[r*z+c] += A[r*n+i]*B[c*n+i];
            }
        }
    }
}

void matb(double* const A,double* const B,double *AtB,int m,int n,int z) {
    memset(AtB,0,n*z*sizeof(AtB[0]));
    for(int r=0;r<n;++r) {
        for(int c=0;c<z;++c) {
            for(int i=0;i<m;++i) {
                AtB[r*z+c] += A[r+i*n]*B[c+i*z];
            }
        }
    }
}

bool least_solve(double * const A,//m*n
                double * const B,//m*z
                double *X,//n*z unknown
                double *AA,//A'A n*n temp
                double *TAA,//(A'A)^1 n*n temp
                double *AB,//A'B n*z temp
                int m,int n,int z) {


    //A'A
    mata(A,AA,m,n);

    //A'B
    matb(A,B,AB,m,n,z);


    //(A'A)^-1
    if(!gauss_elim(AA,TAA,n)) {
        return false;
    }

    //X
    mul(TAA,AB,X,n,n,z);
    return true;
}

