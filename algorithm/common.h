#ifndef COMMON_H
#define COMMON_H

#include <opencv2/core/core.hpp>
#include <assert.h>
#include <time.h>



/*debug*/
#ifdef DEBUG
#define BEGIN_TIME long ___start___ = clock();
#define PRINT_TIME(x) qDebug()<<x<<(clock()-___start___)/CLOCKS_PER_SEC<<"s";
#define REBEGIN_TIME ___start___ = clock();
#else
#define BEGIN_TIME
#define PRINT_TIME(x)
#define REBEGIN_TIME
#endif
/*macro helper*/
#define VEC(x) std::vector<x>
#define REF(x) const x&

#define FOR_EACH(it,container) \
    for(typeof((container).begin()) it = (container).begin();it!=(container).end();++it)


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

#include "misc/number.h"
#include "misc/matrix.h"
#include "misc/interpolate.h"
#include "misc/triangle.h"
#include "misc/geometry.h"
#include "misc/cvextend.h"
#include "misc/qtextend.h"
#include "misc/tps.h"


#endif // COMMON_H
