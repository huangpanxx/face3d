#include <QDebug>
#include <opencv2/imgproc/imgproc.hpp>

#include "geometry.h"

class _cmp_convex_hull{
public:
    _cmp_convex_hull(cv::Point2f point): p(point),x(1,0){}
    bool operator() (const cv::Point2f &p1, const cv::Point2f &p2) const {
        cv::Point2f dp1 = p1-p,dp2 = p2-p;
        float d1 = abs(dp1),d2 = abs(dp2);
        float t1 = x.dot(dp1)/d1;
        float t2 = x.dot(dp2)/d2;
        if(t1!=t2) return t1>t2;
        else return d1 > d2;
    }

private:
    cv::Point2f p,x;
};




std::vector<cv::Point2f> convex_hull(const std::vector<cv::Point2f>& _points,float eps) {
    assert(_points.size() >= 3);

    std::vector<cv::Point2f> points = remove_near_points2d(_points,eps); //unique

    uint n = 0;
    for(uint i = 0;i<points.size();++i) {
        const cv::Point2f &p = points[i];
        const cv::Point2f &q = points[n];
        if((p.y>q.y) || (p.y==q.y && p.x<q.x))
            n = i;
    }

    const cv::Point2f p = points[n];
    points.erase(points.begin() + n);

    _cmp_convex_hull cmp(p);
    std::sort(points.begin(),points.end(),cmp); //sort

    std::vector<cv::Point2f> vex; vex.reserve(points.size());//vex
    vex.push_back(p); vex.push_back(points[0]); vex.push_back(points[1]);
    //push and pop
    for(uint i=2;i<points.size();++i) {
        const cv::Point2f &p1 = vex.back();
        const cv::Point2f &p2 = vex.at(vex.size()-2);
        for(uint j=i;j<points.size();++j) {
            cv::Point2f v1 = p2-p1,v2 = points[j]-p1;
            float s = sin(v1,v2);
            if(s<0) {vex.pop_back();break;}
        }
        vex.push_back(points[i]);
    }

    return vex;
}

bool _cmp_find_counters(const std::vector<cv::Point> &v1,const std::vector<cv::Point> &v2) {
    return v1.size()<v2.size();
}

std::vector<cv::Point> find_counters(const cv::Mat &src,float thres, float minDis) {
    assert(src.channels() == 1);
    cv::Mat im = src.clone();
    im.setTo(0,im<thres);

    std::vector<std::vector<cv::Point> > counters;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(im,counters,hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    std::sort(counters.begin(),counters.end(),_cmp_find_counters);
    std::vector<cv::Point> &ct = counters.back();

    if(ct.size() <= 2) return ct;


    cv::Point p = ct[0];
    std::vector<cv::Point> ss;ss.push_back(p);
    for(uint i=1;i<ct.size();++i) {
        cv::Point q = ct[i];
        if(abs(p-q)>=minDis) {
            ss.push_back(q);
            p = q;
        }
    }
    return ss;

}



inline bool _on_segment(const cv::Point2f& p,const cv::Point2f& x1,const cv::Point2f& x2,float eps=0) {
    return fmin(x1.x,x2.x)<=p.x+eps && fmax(x1.x,x2.x)>=p.x-eps
            && fmin(x1.y,x2.y)<=p.y+eps && fmax(x1.y,x2.y)>=p.y-eps;
}

inline bool is_on_segment(const cv::Point2f& p,const cv::Point2f& x1,const cv::Point2f& x2,float eps) {
    cv::Point2f x = x2-x1;
    return (p-x1).cross(x)/abs(x)<=eps && _on_segment(p,x1,x2);
}

PolyResult is_in_poly(const cv::Point2f &p, const std::vector<cv::Point2f> &points, int &index, float eps) {
    assert(points.size() >= 2);
    float last = -1;
    for(uint i=1;i<=points.size();++i) {
        const cv::Point2f &p1 = points[i-1], &p2 = points[i%points.size()];
        const cv::Point2f d = p2-p1;
        float v = (p-p1).cross(p2-p1)/abs(d);
        if(is_zero(v,eps))
        {
            if(_on_segment(p,p1,p2,eps)) {
                index = i-1;
                return on;
            } else {
                return out;
            }
        }

        if(i!=1 && v*last < 0) {
            return out;
        }

        last = v;
    }
    return in;
}

PolyResult is_in_poly(const cv::Point2f& p, const std::vector<cv::Point2f> &points, float eps) {
    int x;
    return is_in_poly(p,points,x,eps);
}



