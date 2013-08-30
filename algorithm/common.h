#ifndef COMMON_H
#define COMMON_H

#include <opencv2/core/core.hpp>
#include <assert.h>
#include <time.h>
#include <ostream>
//#define DEBUG //debug model

#include "../cxsparse/cs.h"

#ifdef DEBUG
#define BEGIN_TIME long ___start___ = clock();
#define PRINT_TIME(x) qDebug()<<x<<((float)(clock()-___start___)/CLOCKS_PER_SEC)<<"s";
#define REBEGIN_TIME ___start___ = clock();
#define PRINT_TIME_REBEGIN(x) PRINT_TIME(x);REBEGIN_TIME;
#else
#define BEGIN_TIME
#define PRINT_TIME(x)
#define REBEGIN_TIME
#define PRINT_TIME_REBEGIN(x)
#endif

/*macro helper*/
#define VEC(x) std::vector<x>
#define REF(x) const x&
#define REF_VEC(x) REF(VEC(x))

void init_resources();


/*color*/
void hsl_to_rgb(double h, double s, double L,int &rr,int &gg,int &bb);


/*buffer*/
void zeros(double *p,int sz);
double* malloc_zeroed_buffer(int sz);


/*draw*/
void draw_tris(cv::Mat &mat,float x,float y,const std::vector<cv::Point2f> &pts,
               const std::vector<cv::Vec3i> &tris);
void draw_tri(cv::Mat &mat,float x,float y,const std::vector<cv::Point2f> &pts,
               const cv::Vec3i &tri);
void draw_poly(cv::Mat &mat, float x,float y,
               const std::vector<cv::Point2f> &vex);

/*operator*/
template <class T>
VEC(T) operator + (const VEC(T) &op1,const VEC(T) &op2) {
    VEC(T) ls;ls.reserve(op1.size()+op2.size());

    for(auto &it : op1) {
        ls.push_back(*it);
    }
    for(auto &it : op2) {
        ls.push_back(*it);
    }
    return ls;
}

template <class T>
std::ostream& operator << (std::ostream& os,const VEC(T) &v) {
    bool first = true;
    os<<"[";
    for(auto e : v) {
        if(first) {
            first = false;
        } else {
            os<<", ";
        }
        os<<e;
    }
    os<<"]";
    return os;
}

cv::Point3f apply_matrix(const cv::Mat &A,const cv::Point3f &p);

inline cv::Point3f operator * (const cv::Mat &A,const cv::Point3f &p) {
    return apply_matrix(A,p);
}

inline VEC(cv::Point3f) operator * (const cv::Mat &A,const VEC(cv::Point3f) &pts) {
    VEC(cv::Point3f) new_pts; new_pts.reserve(pts.size());
    for(auto &it : pts) {
        new_pts.push_back(A*it);
    }
    return new_pts;
}

#include "misc/number.h"
#include "misc/matrix.h"
#include "misc/interpolate.h"
#include "misc/triangle.h"
#include "misc/geometry.h"
#include "misc/cvextend.h"
#include "misc/qtextend.h"
#include "misc/3dhelper.h"
#include "misc/solver.h"


#endif // COMMON_H
