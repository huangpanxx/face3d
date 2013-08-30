#ifndef MATRIX_H
#define MATRIX_H

#include <map>
#include <vector>
#include <cmath>
#include <QDebug>
#include "../common.h"



class CSBuilder {
public:
    CSBuilder(int rows,int cols) {
        this->_nrows = rows; this->_ncols = cols;
    }

    CSBuilder(int size) {
        this->_nrows = size; this->_ncols = size;
    }

    void setVal(int x,int y,double v) {
        if(x<0 || x>=_ncols) throw "x is out of boundary";
        if(y<0 || y>=_nrows) throw "y is out of boundary";
        this->_elems[this->_makeIndex(x,y)] = v;
    }

    double getVal(int x,int y) const {
           std::map<long,double>::const_iterator it
                   = this->_getIterator(x,y);
           if(it == this->_elems.end()) throw "element does not exist";
           return it->second;
    }

    void addTo(int x,int y,double v) {
        std::map<long,double>::const_iterator it
                = this->_getIterator(x,y);
        if(it != this->_elems.end()) {
            v += it->second;
        }
        this->setVal(x,y,v);
    }

    void subTo(int x,int y,double v) {
        this->addTo(x,y,-v);
    }

    bool contains(int x,int y) const {
        return this->_elems.end() !=
                this->_getIterator(x,y);
    }

    int size() const { return this->_elems.size(); }
    int cols() const { return this->_ncols; }
    int rows() const { return this->_nrows; }
    cs* buildCS(int *buf_x,int *buf_y, double *buf_v) const;

private:
    std::map<long,double>::const_iterator
    _getIterator(int x,int y) const {
        return this->_elems.find(this->_makeIndex(x,y));
    }
    long _makeIndex(int x,int y) const {
        return (long)y*this->_ncols+x;
    }
    std::map<long,double> _elems;
    int _nrows,_ncols;
};


bool cs_divide(double *dividend, CSBuilder *divisor);
bool solve_sparse(int* px,int *py,double *pv,double *pB,int sz,int N);
void maat(double * const A,double *AA,int m,int n);//aa'
void mata(double * const A,double *AA,int m,int n);//a'a
void matb(double* const A,double* const B,double *AtB,int m,int n,int z);//a'b
void mabt(double* const A,double* const B,double *AtB,int m,int n,int z);//ab' b=z*n
void mul(double * const A,double * const B,double *X,int m,int n,int z);//ab
bool gauss_elim(double *A,double *R,int n);//a^-1
bool least_solve(double *const A, //m*n
                double *const B, //m*z
                double *X, //n*z unknown
                double *AA, //A'A n*n temp
                double *TAA, //(A'A)^1 n*n temp
                double *AB, //A'B n*z temp
                int m, int n, int z);

#endif // MATRIX_H
