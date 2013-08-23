#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <opencv2/core/core.hpp>
#include "../common.h"
#include <QDebug>


inline float distance(float x,float y) {
    return std::sqrt(x*x+y*y);
}

inline float distance(float x1,float y1,float x2,float y2) {
    return distance(x1-x2,y1-y2);
}

inline float abs(const cv::Point2f &p) {
    return distance(p.x,p.y);
}

inline float cos(const cv::Point2f& p1,const cv::Point2f &p2) {
    return p1.dot(p2)/(abs(p1)*abs(p2));
}
inline float cos(float x1,float y1,float x2,float y2) {
    return (x1*x2+y1*y2) / (distance(x1,y1)*distance(x2,y2));
}
inline float sin(const cv::Point2f& p1,const cv::Point2f &p2) {
    return p1.cross(p2)/(abs(p1)*abs(p2));
}
inline float sin(float x1,float y1,float x2,float y2) {
    return (x1*y2-x2*y1) / (distance(x1,y1)*distance(x2,y2));
}
inline cv::Point2f operator - (const cv::Point2f &p1,const cv::Point2f &p2) {
    return cv::Point2f(p1.x-p2.x,p1.y-p2.y);
}





template<class T>
class point_equal_comparater_2d {
public:
    inline point_equal_comparater_2d(float eps) : d(eps) { }
    bool operator ()(const T &p1, const T &p2) const {
        return distance(p1.x-p2.x,p1.y-p2.y) < d;
    }
private:
    float d;
};
template <class T>
bool point_x_comparater_2d(const T &p1,const T &p2) {
    return p1.x<p2.x;
}
template <class T>
bool point_y_comparater_2d(const T &p1,const T &p2) {
    return p1.y < p2.y;
}

#define _remove_near_points2d_builder(T) \
inline VEC(T) remove_near_points2d(const VEC(T) &points,float eps) { \
    VEC(T) set(points);  \
    VEC(T)::iterator it_end =  \
        std::unique(set.begin(),set.end(),point_equal_comparater_2d<T>(eps)); \
    return VEC(T)(set.begin(),it_end); \
}
_remove_near_points2d_builder(cv::Point2f)
_remove_near_points2d_builder(cv::Point3f)



std::vector<cv::Point> find_counters(const cv::Mat &src,float thres, float minDis);

std::vector<cv::Point2f> convex_hull(const std::vector<cv::Point2f>& points, float eps = EPS);

/*! poly points must be organized in clockwise or counterclockwise */
enum PolyResult{ out = 0, on, in};
PolyResult is_in_poly(const cv::Point2f &p, const std::vector<cv::Point2f> &points, float eps = EPS);
PolyResult is_in_poly(const cv::Point2f &p, const std::vector<cv::Point2f> &points, int &index, float eps = EPS);


bool is_on_segment(const cv::Point2f& p,const cv::Point2f& x1,const cv::Point2f& x2,float eps);


template<class T>
cv::Rect rectf_from_points(const std::vector<T> &points) {

    float left = NaN,right = -NaN,top = NaN,bottom = -NaN;
    for(uint i=0;i<points.size();++i) {
        const T &p = points[i];
        left= fmin(left,p.x); right = fmax(right,p.x);
        top = fmin(top,p.y);  bottom = fmax(bottom,p.y);
    }
    return cv::Rect(left,top,right-left,bottom-top);
}
#endif // GEOMETRY_H
