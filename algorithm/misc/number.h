#ifndef NUMBER_H
#define NUMBER_H


#include <vector>
#include <cmath>


const float EPS = 1e-10;
const float NaN = 1e20;

inline bool is_nan(float x) {
    return x==NaN;
}

template <class T>
inline T min(const T &x1,const T &x2,const T &x3) {
    return std::min(std::min(x1,x2),x3);
}

template <class T>
inline T max(const T &x1,const T &x2,const T &x3) {
    return std::max(std::max(x1,x2),x3);
}

inline double range(double v,double _min,double _max) {
    return std::max(std::min(v,_max),_min);
}

inline double abs_max(double a,double b) {
    return fmax(fabs(a),fabs(b));
}
inline double is_zero(double s,float eps = EPS) {
    return fabs(s) < eps;
}
inline double is_equal(double a,double b,double eps = EPS) {
    return fabs(a-b) < eps;
}
inline bool is_in_range(float v,float min,float max) {
    return v>=min && v<=max;
}
inline double square(double x) { return x*x; }


#endif // NUMBER_H
