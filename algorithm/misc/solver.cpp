#include "solver.h"

double iter_solve(const equations &eqs,std::vector<double> &x,int maxIter,double dx,double eps) {
    double max_err = -NaN;
    const int n = (int)eqs.number();
    if((int)x.size() != n) {
        x.resize(n);
    }

    cv::Mat old_inv_jacobi;
    cv::Mat jacobi(n,n,CV_64F);
    cv::Mat fx(1,n,CV_64F);
    while(maxIter--) {
        max_err = -NaN;
        //fx
        for(int i=0;i<n;++i) {
            fx.at<double>(0,i) = eqs(x,i);
        }
        //jacobi
        for(int i=0;i<n;++i) {
            x[i] += dx;
            for(int j=0;j<n;++j) {
                jacobi.at<double>(i,j) = (eqs(x,j) - fx.at<double>(j))/dx;
            }
            x[i] -= dx;
        }

        //inv
        cv::Mat inv_jacobi;
        double r = cv::invert(jacobi,inv_jacobi);

        //singular check
        if(is_zero(r)) {
            if(old_inv_jacobi.rows == 0) { //old not available
                for(int i = 0;i < n; ++i) {
                    jacobi.at<double>(i,i) += 0.001;
                }
                r = cv::invert(jacobi,inv_jacobi);
                if(is_zero(r)) { //failed
                   return -1;
                }
            } else { //use old
                inv_jacobi = old_inv_jacobi;
            }
        } else {
            old_inv_jacobi = inv_jacobi;
        }

        //delta
        cv::Mat det = fx*inv_jacobi;

        //update
        for(int i=0;i<n;++i) {
            double d = det.at<double>(0,i);
            max_err = std::max(max_err,std::abs(d));
            x[i] -= d;
        }

        //thres
        if(max_err <= eps) {
            return max_err;
        }
    };
    return max_err;
}


class least_square_equations : public equations {
public:
    least_square_equations(const equations *eqs,int nvar,double dx) :
        m_eqs(eqs), m_dx(dx), m_nvar(nvar) {
    }
    double operator () (const VEC(double) &x,int i) const {
        VEC(double) &px = *(VEC(double)*)&x;
        px[i] += m_dx;
        double f1 = this->square_sum(x);
        px[i] -= m_dx;
        double f2 = this->square_sum(x);
        return (f1-f2)/m_dx;
    }

    int number() const {
        return m_nvar;
    }
private:
    double square_sum(const VEC(double) &x) const {
        double s = 0;
        for(int i=0;i<m_eqs->number();++i) {
            double v = (*m_eqs)(x,i);
            s += v*v;
        }
        return s;
    }

private:
    const equations *m_eqs;
    const double m_dx;
    const int m_nvar;
};

double least_square_iter_solve(const equations &eqs,int nvar, std::vector<double> &x,
                    int maxIter,double dx,double eps) {
    least_square_equations lseq(&eqs,nvar,dx);
    return iter_solve(lseq,x,maxIter,dx,eps);
}
